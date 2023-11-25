#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define PORT "9000"
#define BACKLOG 10                 // how many pending connections queue will hold
#define MAX_DATASIZE (1024 * 1024) // how many pending connections queue will hold

#define IS_SOCKET 0
#define IS_FILE 1

char s[INET6_ADDRSTRLEN];
int sockfd, fd_client, fd_file;

ssize_t
readLine(int fd, void *buffer, size_t n, int f_or_s)
{
    ssize_t numRead;                    /* # of bytes fetched by last read() */
    size_t totRead;                     /* Total bytes read so far */
    char *buf;
    char ch;

    if (n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;                       /* No pointer arithmetic on "void *" */

    totRead = 0;
    for (;;) {
        if (f_or_s == IS_SOCKET){
           numRead  = recv (fd, &ch, 1, 0); 
        }
        if (f_or_s == IS_FILE){
           numRead = read(fd, &ch, 1);
        }
        

        if (numRead == -1) {
            if (errno == EINTR)         /* Interrupted --> restart read() */
                continue;
            else
                return -1;              /* Some other error */

        } else if (numRead == 0) {      /* EOF */
            if (totRead == 0)           /* No bytes read; return 0 */
                return 0;
            else                        /* Some bytes read; add '\0' */
                break;

        } else {                        /* 'numRead' must be 1 if we get here */
            if (totRead < n - 1) {      /* Discard > (n - 1) bytes */
                totRead++;
                *buf++ = ch;
            }

            if (ch == '\n')
                break;
        }
    }

    *buf = '\0';
    return totRead;
}


// Write file
void write_file(const char *buf, int n)
{


    fd_file = open("/var/tmp/aesdsocketdata", (O_WRONLY | O_CREAT| O_APPEND), S_IRUSR | S_IWUSR);

    if (fd_file == -1)
    {printf("ERRRR %s\n", buf);
        printf("%s", buf);
        exit(-1);
    }
    else
    {
        printf("ESCRIBIENDO %s\n", buf);
        write(fd_file, buf, n);
        
    }
    close(fd_file);
}





// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

static void signal_action_hdlr(int signal){

    if (signal == SIGTERM){
        close(fd_client);
        close(sockfd);
        remove("/var/tmp/aesdsocketdata");
        syslog(LOG_INFO, "Closed connection from %s", s);
        
    }

    if (signal == SIGINT){
        close(fd_client);
        close(sockfd);
        remove("/var/tmp/aesdsocketdata");
        syslog(LOG_INFO, "Closed connection from %s", s);
    }

    exit(0);
}

int main(void)
{
    int  status, len;
    struct addrinfo hints;
    struct addrinfo *srvinfo, *p;
    int yes = 1;
    socklen_t sin_size;
    struct sockaddr_storage their_addr; // connector's address information
    
    char buf[MAX_DATASIZE];
    char *buf_pt;
    struct sigaction sa;

    openlog(NULL, 0, LOG_USER);
    memset(&sa, 0, sizeof(struct sigaction));
    memset(&hints, 0, sizeof(hints));

    sin_size = sizeof(their_addr);

    /*register signal action handler*/
    sa.sa_handler = signal_action_hdlr;

    if (sigaction(SIGTERM,&sa,NULL) != 0){
        exit(-1);
    }
    if (sigaction(SIGINT,&sa,NULL) != 0){
        exit(-1);
    }

    hints.ai_family = AF_UNSPEC; /*dont care IP4 or IP6*/
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, PORT, &hints, &srvinfo)) != 0)
    {
        exit(-1);
    }

    // loop through all the results and bind to the first we can
    for (p = srvinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(-1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            exit(-1);
        }

        break;
    }

    freeaddrinfo(srvinfo); // avoid memory leaks

    if (p == NULL)
    {

        exit(-1);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        exit(-1);
    }

    while (1)
    { // main accept() loop
        
        fd_client = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        

        if (fd_client == -1)
        {
            continue;
        }

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        syslog(LOG_INFO, "Accepted connection from %s", s);
 

        /*SEND THE CONTENTS OF THE WHOLE FILE BACK */
        fd_file = open("/var/tmp/aesdsocketdata", O_RDONLY, S_IRUSR | S_IWUSR); 

        if (fd_file == -1)
        {
            printf("ERROR ABRIENDO");
        }
        lseek(fd_file,0,SEEK_SET);  

        memset(buf, 0, MAX_DATASIZE);
        buf_pt = buf;
        len = readLine(fd_file, buf_pt, MAX_DATASIZE, IS_FILE);
        
        buf_pt = buf;
        while (len > 0){
            printf("ENVIANDO DE VUELTA %s\n", buf_pt);
            send (fd_client, buf_pt , len, 0);

            len = readLine(fd_file, buf_pt, MAX_DATASIZE, IS_FILE);


            buf_pt = buf;
        }
        /*restore last offset*/
        lseek(fd_file,0 ,SEEK_END);  

        close(fd_file);




        /*READ DATA AND WRITE FILE */
        memset(buf, 0, MAX_DATASIZE);
        buf_pt = buf;
        len = readLine(fd_client, buf_pt, MAX_DATASIZE, IS_SOCKET);
        
        while (len > 0){
            printf("ESCRIBIENDO %s\n", buf_pt);
            send (fd_client, buf_pt , len, 0);
            write_file(buf_pt, len);
            buf_pt = buf;
            len = readLine(fd_client, buf_pt, MAX_DATASIZE, IS_SOCKET);
            
        }
       
        


        

        printf("FIN DE LINEAS %s\n", buf_pt);
    }
    

    return 0;
}
