#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 1

struct thread_param{
    int sock;
};

void *listener(void *parameters){
    struct thread_param *listen_p =(struct thread_param*)parameters;
    char recvline[256];
    while(1){
        bzero(recvline, 256);
        read(listen_p->sock,recvline,256); //read message back
        printf("%s",recvline); //print message
    }

}
char* get_time_string(){
    struct tm *tm;
    time_t t;
    char *str_time = (char *)malloc(100*sizeof(char));
    t = time(NULL);
    tm = localtime(&t);
    strftime(str_time, 100, "%H:%M:%S", tm);
    return str_time;
}

int main()
{
    int sockfd,n, connection;
    char sendline[256];
    pthread_t listen_thread;
    struct thread_param t_params;
    struct sockaddr_in servaddr;
    char username[20];
    char *mytime;
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof servaddr);

    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(22000);

    inet_pton(AF_INET,"76.90.227.209",&(servaddr.sin_addr));

    printf("Enter a user name:  ");
    scanf("%s", username);
    printf("Attempting to connect...\n");


    connection = connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    if(connection < 1){
        mytime = get_time_string();
        printf("\n\tConnection successful at %s\n", mytime);
    }
    printf("\tMay type 250 characters max...\n\n\n");
    t_params.sock = sockfd;
    pthread_create(&listen_thread, NULL, &listener,&t_params);
    while(1)
    {

        bzero(sendline, 256); //place n amount of zero-valued bytes
        fgets(sendline,256,stdin); //stdin = 0 , for standard input
        write(sockfd,sendline,strlen(sendline)+1); //send the message
        mytime = get_time_string();
        printf("%s [%s]:  ",username, mytime);
    }

}
