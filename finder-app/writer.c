/**
* Writer program for assignment 2
# Author: Abel Balbis
*/

#include <sys/types.h> 
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>  
#include <syslog.h>        

#define NUM_PARAMS 3


int main (int argc, char *argv[])
{
FILE *fd;
char *file_name;

openlog(NULL, 0 , LOG_USER);


if ((argc == NUM_PARAMS - 2) || (strcmp(argv[1], "--help") == 0)){
    
    printf("Frist parameter 'writefile' not specified");
    syslog (LOG_ERR, "Frist parameter 'writefile' not specified");
    return 1;
}   

if ((argc == NUM_PARAMS - 1) || (strcmp(argv[1], "--help") == 0)){
    
    printf("Second parameter 'writestr' not specified");
    syslog (LOG_ERR, "Second parameter 'writestr' not specified");
    return 1;
}   

file_name = basename(argv[1]);




/*create file*/
fd = fopen((const char *)argv[1], "w+");
if (fd == NULL){
    //printf("%s cannot be created", argv[1]);
    syslog (LOG_ERR, "%s cannot be created", argv[1]);
    return 1;
}
else{
    /*put the contents within the file*/
    syslog (LOG_DEBUG, "Writing %s to %s", argv[2], file_name);
    fprintf(fd, (const char *)argv[2]);

}
fclose(fd);


return 0;
}

