#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include "codes.h"

#define MAXBYTES 10
#define MAXCOMMS 5

using namespace std;
int appacc_global, idsaacc_global, hoiacc_global;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct processor_params{
	unsigned char command[MAXBYTES];
	int acc;
};
void* processor(void* parameters){
	struct processor_params* p = (struct processor_params*) parameters;
	int comm_process = process(p->command);
	unsigned char message[MAXBYTES];
	memcpy(message, p->command, sizeof(message));
	if(comm_process==0){ // 0 == app
		//message is valid, send to application module
		write(appacc_global, message, sizeof(message));
	}else if(comm_process==1){
		//message is valid, send to IDSA
		write(idsaacc_global, message, sizeof(message));
	}else if(comm_process==2){
		//message is valid, send to HOI
		write(hoiacc_global, message, sizeof(message));
	}else{
		//message invalid, return 'invalid' to sender
		write(p->acc, "invalid message", 16);
	}
}
void create_process(unsigned char* msg, int acc){
	//no thread limit on high level of incomming messages...
	pthread_t comm_thread;
	struct processor_params pro_args;
	pro_args.acc = acc;
	memcpy(pro_args.command, msg, sizeof(pro_args.command));
	pthread_create(&comm_thread, NULL, &processor, &pro_args);

	/*
	int thread_counter = 0;
	int* comms_array = (int *)malloc(sizeof(int)*MAXCOMMS);
	pthread_t* comm_threads = (pthread_t*)malloc(sizeof(pthread_t) * MAXCOMMS);
	struct processor_params pro_args;

	memcpy(buff, pro_args.command, sizeof(buff));
	pthread_create(&comm_threads[thread_counter], NULL, &processor, &pro_args);
	thread_counter++;
	for(int i = 0; i < MAXCOMMS; i++){
		if(pthread_kill(comm_threads[i], 0) != 0){
			thread_counter--;
		}
	}
	*/
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct listener_params{
    int sock, acc, port;
    char ver;
};
void* listener(void* parameters){
	struct listener_params* p = (struct listener_params*) parameters;
	int m, clien, clien2;
	unsigned char buff[MAXBYTES];
	unsigned char check[] = "c_est";
   	struct sockaddr_in res_addr, cli_addr, cli2_addr;
   	socklen_t cli_addr_size = sizeof(struct sockaddr_in);
   	socklen_t cli2_addr_size = sizeof(struct sockaddr_in);

   	if(p->ver=='A'){
   		puts("Opening Application Monitor");
   	}
	else if(p->ver=='I'){
		puts("Opening IDSA Monitor");
	}
	else if(p->ver=='H'){
		puts("Opening HOI Monitor");
	}
	bzero((char *) &res_addr, sizeof(res_addr));
	res_addr.sin_family = AF_INET;
	res_addr.sin_addr.s_addr = INADDR_ANY;
	res_addr.sin_port = htons((int)p->port);

    if(bind(p->sock,(struct sockaddr *)&res_addr, sizeof(res_addr))<0){
        perror("binding ERR");
        return NULL;
    }
    if(listen(p->sock,5) == -1){
    	perror("listen ERR");
    	return NULL;
    }
    if ((p->acc = accept(p->sock,(struct sockaddr*) &cli_addr, &cli_addr_size)) < 0){
		perror("accept ERR");
		return NULL;
	}
	//write(p->acc,"connected", 10);
   	bzero(buff, MAXBYTES);
   	/*
    if((m = read(p->acc, buff, sizeof(buff))) < 0){
		perror(strerror(errno));
		return NULL;
	}
	*/
   	if(p->ver=='A'){
   		appacc_global = p->acc;
   	}
	else if(p->ver=='I'){
		idsaacc_global = p->acc;
	}
	else if(p->ver=='H'){
		hoiacc_global = p->acc;
	}
	//printf("Here is the message: %s\n",buff);
	//create_process(buff, p->acc); 
    while(1){
    	bzero(buff, MAXBYTES);
    	if((m = read(p->acc, buff, sizeof(buff))) < 0){
			perror(strerror(errno));
			return NULL;
		}
		if(m == 0){
			close(p->acc);
			while(m == 0){
				printf("Connection lost on %c, attempting reconnect...\n", p->ver);
				if ((p->acc = accept(p->sock,(struct sockaddr*) &cli2_addr, &cli2_addr_size)) < 0){
					perror("acception err");
					return NULL;
				}
			   	if(p->ver=='A'){
			   		appacc_global = p->acc;
			   	}
				else if(p->ver=='I'){
					idsaacc_global = p->acc;
				}
				else if(p->ver=='H'){
					hoiacc_global = p->acc;
				}
				if((m = read(p->acc, buff, sizeof(buff))) < 0){
					perror("read1 err");
					return NULL;
				}
			}
			printf("%c reconnected!\n", p->ver);
			write(1, buff, sizeof(buff));
			//printf("Here is the message: %s\n",buff);
			create_process(buff, p->acc);
			write(p->acc, "reconnected", 11);
		}
		else{
			printf("Ver: %c\n", p->ver);
			printf("Here is the message: %s",buff);
			create_process(buff, p->acc); 
		}
	}
	return NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int main(int argc, char *argv[]){
	pthread_t thread_id1;
	pthread_t thread_id2;
	pthread_t thread_id3;

	struct listener_params thread_id1_args;
	struct listener_params thread_id2_args;
	struct listener_params thread_id3_args;

	int app_sock, idsa_sock, hoi_sock;
	int app_acc, idsa_acc, hoi_acc;
	char ver;

	ver = 'A';
	if ((app_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket ERR");
		return -1;
	}
	thread_id1_args.sock = app_sock;
	thread_id1_args.acc = app_acc;
	thread_id1_args.ver = ver;
	thread_id1_args.port = 20000;
 	pthread_create(&thread_id1, NULL, &listener, &thread_id1_args);

	ver = 'I';
	if ((idsa_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket ERR");
		return -1;
	}
	thread_id2_args.sock = idsa_sock;
	thread_id2_args.acc = idsa_acc;
	thread_id2_args.ver = ver;
	thread_id2_args.port = 21000;
 	pthread_create(&thread_id2, NULL, &listener, &thread_id2_args);

	ver = 'H';
	if ((hoi_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket ERR");
		return -1;
	}
	thread_id3_args.sock = hoi_sock;
	thread_id3_args.acc = hoi_acc;
	thread_id3_args.ver = ver;
	thread_id3_args.port = 22000;
 	pthread_create(&thread_id3, NULL, &listener, &thread_id3_args);

	while(1){
		if(pthread_kill(thread_id1, 0) != 0){
			close(app_sock);
			ver = 'A';
			if ((app_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
				perror("socket ERR");
				return -1;
			}
			thread_id1_args.sock = app_sock;
			thread_id1_args.acc = app_acc;
			thread_id1_args.ver = ver;
			thread_id1_args.port = 20000;
		 	pthread_create(&thread_id1, NULL, &listener, &thread_id1_args);
		}
		if(pthread_kill(thread_id2, 0) != 0){
			close(idsa_sock);
			ver = 'I';
			if ((idsa_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
				perror("socket ERR");
				return -1;
			}
			thread_id2_args.sock = idsa_sock;
			thread_id2_args.acc = idsa_acc;
			thread_id2_args.ver = ver;
			thread_id1_args.port = 21000;
		 	pthread_create(&thread_id2, NULL, &listener, &thread_id2_args);
 		}
		if(pthread_kill(thread_id3, 0) != 0){
			close(hoi_sock);
			ver = 'H';
			if ((hoi_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
				perror("socket ERR");
				return -1;
			}
			thread_id3_args.sock = hoi_sock;
			thread_id3_args.acc = hoi_acc;
			thread_id3_args.ver = ver;
			thread_id3_args.port = 22000;
		 	pthread_create(&thread_id3, NULL, &listener, &thread_id3_args);
		}
		sleep(5);
	}

	pthread_join(thread_id1, NULL);
	pthread_join(thread_id2, NULL);
	pthread_join(thread_id3, NULL);

 	close(app_sock);
 	close(idsa_sock);
 	close(hoi_sock);

	return 0;
}
