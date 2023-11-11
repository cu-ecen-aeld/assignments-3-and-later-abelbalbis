#include "systemcalls.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>


#define PCHILD_ERROR 0XAA

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{
    int ret;

    ret = system(cmd);
    if (ret != 0){
        return false;
    } else{
        return true;
    }
    
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    pid_t pid = 0;
    int st, ret;


    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

   
/*
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
    pid = fork();
    if(pid == 0){
        /*Im the child*/
        if (command[2][0] != '/'){exit(PCHILD_ERROR);}
        ret = execvp(command[0], &command[0]);
        exit(PCHILD_ERROR);
    }  
    ret = wait (&st);
    if (ret == -1)
        return false;
    if (WIFEXITED (st)){
        if (WEXITSTATUS (st) == PCHILD_ERROR) return false;
        else return true;
    }
    return false;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    int pid, st, ret;
    int fd;
    
    /*To avoid environment expansion for the second test (count ==2) */
    char *const envp[] = {"HOME=$HOME", NULL};

    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;



/*
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/

   
    fd = open(outputfile, O_WRONLY|O_EXCL, 0644); 

    switch (pid = fork()) {
    case -1: return false;
    case 0:    
        if (fd < 0) { return false;}
        dup2(fd, 1);
        close(fd);

        /*Avoid environment expansion for the second test */
        if (count == 2){
            execve(command[0], &command[0],envp);
        /*Dont avoid environment expansion for the first test */
        } else {
            execv(command[0], &command[0]);
        }

        exit(PCHILD_ERROR);
    default:
 
        ret = waitpid (pid, &st, 0);

        close(fd);

        if (ret == -1) return false;
        if (WIFEXITED (st)){
            if (WEXITSTATUS (st) == PCHILD_ERROR) return false;
            else return true;
        }
        return false;
    }

 


    
}


