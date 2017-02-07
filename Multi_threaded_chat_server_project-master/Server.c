#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define MAX_THREADS 10

struct client_data{
    int sock;
};

int g_sockID_array[MAX_THREADS];

void* process (void* parameters)
{
    int send_count;
    int sock;
    struct client_data *client = (struct client_data*)parameters;
    sock = client->sock;
    printf("In thread %d\n",sock);
    int n;
    char drop_connect[] = {"quit"};
    char buffer[256];
    printf("socket descriptor: %d\n", client->sock);
    bzero(buffer,256);

    while(1){
        bzero(buffer,256);
        printf("in loop 1:\n");
        n = read(client->sock,buffer,255);
        printf("in loop 2:\n");
        if (n < 0){
            perror("ERROR reading from socket");
            return 1;
        }
        send_count = 0;
        printf("in loop 3:\n");
        while(send_count < MAX_THREADS){
            printf("in loop 3.1:\n");
            if(g_sockID_array[send_count] == 0){
                printf("in loop 3.2:\n");
                send_count++;
                break;
            }
            if(g_sockID_array[send_count] == client->sock){
                printf("in loop 3.3:\n");
                send_count++;
                continue;
            }
            else{
                printf("in loop 4:\n");
                n = write(g_sockID_array[send_count],buffer,256);
                if (n < 0){
                    perror("ERROR writing to socket");
                    return 1;
                }
            }
            send_count++;
            printf("in loop 5:\n");
        }
        printf("in loop 6:\n");
    }
    printf("END END\n");
}

int main()
{
    memset(g_sockID_array,0, 40);
    int n;
    int next_empty = 0;
    int success_thread;
    int sockfd;
    int newsockfd;
    int portnum;
    int clilen;
    char buffer[256];
    char ipstr[INET6_ADDRSTRLEN];
    socklen_t len;
    struct sockaddr_storage addr;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;


    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        perror("ERROR opening socket");
        exit(1);
    }
    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portnum = 22000;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portnum);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
         perror("ERROR on binding");
         return 1;
    }
    ///allocate p-array for client threads
    pthread_t *client_threads = (pthread_t*)malloc(sizeof(pthread_t)*MAX_THREADS);
    struct client_data thread_struct[MAX_THREADS];
    /* Now start listening for the clients, here
     * process will go in sleep mode and will wait
     * for the incoming connection
     */
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    while (next_empty < 10)
    {
        success_thread = 1;
        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0){
            perror("ERROR on accept");
            return 1;
        }
        len = sizeof addr;
        getpeername(newsockfd, (struct sockaddr*)&addr, &len);

        // deal with both IPv4
        if (addr.ss_family == AF_INET) {
            struct sockaddr_in *s = (struct sockaddr_in *)&addr;
            //port = ntohs(s->sin_port);
            inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
        }
        printf("Incoming IP address: %s\n", ipstr);
        printf("Connection handle: %d\nEmpty Position value: %d\n", newsockfd, next_empty);
        write(newsockfd,"\n\n\t\tWelcome to the Chat Server\n\n",40);
        if (n < 0){
            perror("ERROR writing to socket");
                return 1;
        }

        ///get the socket
        g_sockID_array[next_empty] = newsockfd;
        thread_struct[next_empty].sock = newsockfd;
        ///create thread, pass structure as parameter
        success_thread = pthread_create(&client_threads[next_empty], NULL, &process, &thread_struct[next_empty]);
        next_empty++;
        //close(newsockfd);

    }///END WHILE
    return 0;
}
