/*  ***** Features to add ******
	1. Command flags - a flag that represents that a command for this type was
						issued, if the flag is set, the flag will prevent more messages
						of the same type from being sent (possibly could use semaphores?)
						once recv code corresponding to flag received, flag is reset

	2.1 Switch order of timer functions, have it test for file not open, this will be the most common case
	
	2. synchronize access - synchronize access to Knowledge bases

	3. Move to external Files - Create a proper .h and .cpp file to clean and organize code
								for application module functions.

	4. Create standard send function - Create a standard send function, send any command.

	5. Finish update all command, finds already saved hyper, replaces all data

	6. Create mesg queue/tasking queue for BOTH kernel messeges and SUPer messeges

*/


#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <iomanip>
#include <fstream>
#include <queue>
#include <ctime>
#include <time.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "hypervisor.h"
#include "interface.h"

using namespace std;

#define A_CHANGE 0x43
#define A_UPDATE 0x55		
#define T_WLAN   0x57
#define T_ETH	 0x45
#define T_VIRT	 0x56
#define SPEC  	 0x01
#define RAND	 0x00

#define SOURCEKERN 0x11
#define SOURCESUPR 0x22

#define RECMESGSIZE 100
#define BUFFSIZE 10
#define MAXTHREAD 10

struct km_params{
	char task[100];
	short thread_index;
	unsigned char tskSource;
};
struct sv_params{
	int sock;
};

//Create a MUTEX
pthread_mutex_t kb_mutex = PTHREAD_MUTEX_INITIALIZER; //mutex to access knowledge base
pthread_mutex_t thread_array_mutex = PTHREAD_MUTEX_INITIALIZER; //mutex to access thread pool array
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

//create a CONDITION VARIABLE [GLOBAL]
pthread_cond_t condition_cond = PTHREAD_COND_INITIALIZER;

int shared_sock;
struct sockaddr_in to_supervisor;
vector<Hypervisor> hypervisor_v;
queue<km_params> task_queue;
short valid_threads[MAXTHREAD];
clock_t upd_timer;
clock_t mtd_timer;
ofstream updateFile("updateTimes.txt", ios::app);
ofstream mtdFile("mtdTimes.txt", ios::app);
char CCID[3] = {0xB7, 0xE2, 0xAC};

void parse_update(string (&out)[30], char in[]){
	char *split;
	const char s[2] = "|";
 	split = strtok(in, s);
 	int i = 0;
 	while(split != NULL){
 		out[i].assign(split, strlen(split));
 		i++;
 		split = strtok(NULL, s);
 	}
}
void write_updateTime(char time_in[]){
	cout<< time_in <<endl;
	char buffer[35];
	time_t rawt;
	string local;
	struct tm * timeinfo;
	time(&rawt);
	timeinfo = localtime(&rawt);
	sprintf(buffer, "%s", asctime(timeinfo));
	for(int i = 0; i<strlen(buffer); i++){
		if(buffer[i] == '\n'){
			buffer[i] = '\0';
			break;
		}
	}
	string dateTime(buffer);
	string timer(time_in);
	if(updateFile.is_open()){
		updateFile << dateTime <<": " <<timer;
	}
	else{
		updateFile.exceptions(ifstream::failbit | ifstream::badbit);
		try {
			updateFile.open("updateTimes.txt", ios::app);
		}catch (ifstream::failure e) {
			cout << "ERROR!! trying to open updateTimes.txt file";
		}		
		if(updateFile.is_open()){
			updateFile << dateTime<<": " <<timer;
		}
	}
	updateFile.close();
}

void write_mtdTime(char time_in[]){
	cout<< time_in <<endl;
	char buffer[35];
	time_t rawt;
	string local;
	struct tm * timeinfo;
	time(&rawt);
	timeinfo = localtime(&rawt);
	sprintf(buffer, "%s", asctime(timeinfo));
	for(int i = 0; i<strlen(buffer); i++){
		if(buffer[i] == '\n'){
			buffer[i] = '\0';
			break;
		}
	}
	string dateTime(buffer);
	string timer(time_in);
	if(mtdFile.is_open()){
		mtdFile << dateTime <<": " <<timer;
	}
	else{
		mtdFile.exceptions(ifstream::failbit | ifstream::badbit);
		try {
			mtdFile.open("mtdTimes.txt", ios::app);
		}catch (ifstream::failure e) {
			cout << "ERROR!! trying to open updateTimes.txt file";
		}		
		if(mtdFile.is_open()){
			mtdFile << dateTime<<": " <<timer;
		}
	}
	mtdFile.close();
}

void kern_send(char cmd[]){
	printf("Kernel Send thread is up...\n");
	int sendlen;
	char buffer[BUFFSIZE];
	strncpy(buffer, cmd, sizeof(buffer));
	printf("TEST SEND: %s", buffer);
	struct sockaddr_in sendsocket;

	/* kernel address */
	memset(&sendsocket, 0, sizeof(sendsocket));
	sendsocket.sin_family = AF_INET;
	sendsocket.sin_addr.s_addr = inet_addr("127.0.0.1");
	sendsocket.sin_port = htons(1215);

	sendlen = strlen(buffer) + 1;
	if (sendto(shared_sock, buffer, sendlen, 0, (struct sockaddr *) &sendsocket, sizeof(sendsocket)) != sendlen) {
		perror("sendto");
		return;
	}
	return;
}

void* handle_SUPR_mesg(void* parameters){
	//printf("got here...\n");
	struct km_params *kp = (struct km_params*)parameters;
	//long double t;
	//char time_buff[16];
	char supr_mesg[10];
	char cmd[10];
	memset(cmd, 0, 10);
	unsigned short index = -1;
	memcpy(supr_mesg, kp->task, 3); 
	switch(supr_mesg[0]){
		case A_UPDATE:
			upd_timer = clock();
			strncpy(cmd, CCID, 3);
			cmd[3] = 0x55;
			printf("%s", cmd);
			kern_send(cmd);
			break;
		case A_CHANGE:
			mtd_timer = clock();
			strncpy(cmd, CCID, 3);
			cmd[3] = supr_mesg[1];
			kern_send(cmd);
			break;
		default:
			printf("SOMETHING WENT WRONG W/handle_SUPR_mesg\n");
	}
	pthread_mutex_lock(&thread_array_mutex);
	valid_threads[kp->thread_index] = 0;
	pthread_mutex_unlock(&thread_array_mutex);
}


void* handle_kern_mesg(void* parameters){
	//printf("got here...\n");
	
	struct km_params *kp = (struct km_params*)parameters;
	long double t;
	char time_buff[16];
	Hypervisor shared_h;
	unsigned char id[3];
	string break_mesg[30];
	unsigned short index = -1;
	memcpy(id, kp->task, 3); 
	unsigned char code =(unsigned char)kp->task[3];
	//for(int h = 0; h < 3; h++){
	//	printf("Copied ID %01X\n",id[h]);
	//}
	pthread_mutex_lock(&kb_mutex);
	switch(code){
		case A_UPDATE:
			parse_update(break_mesg, kp->task);
			//NOT REQUIRED -> DEBUGGING PURPOSES (below)
			printf("Message after parsing: \n");
			for(int k = 0; k < 9; k++){
				printf("%s\n",break_mesg[k].c_str());
				if(break_mesg[k+1].empty()){k=9;}
			}
			printf("\n");
			
			if(hypervisor_v.empty()){
				shared_h.set_id(id); //If empty, create temp; fill temp; then push back
				int n = 1;
				while(!break_mesg[n].empty()){
					shared_h.add_interface(break_mesg[n], break_mesg[n+1]);
					n+=2;
				}
				//pthread_mutex_lock(&thread_array_mutex);
				hypervisor_v.push_back(shared_h);
				hypervisor_v[0].print_cur_stats();
				sprintf(time_buff,"%.9f\n", (double)(clock() - upd_timer)/CLOCKS_PER_SEC);
				pthread_mutex_unlock(&kb_mutex);
				write_updateTime(time_buff);
				break;
			}
			/*NEED TO FINISH, NEEDS TO FIND CURRENT HYPERV AND REPLACE ALL PASSED DATA
			else{
				//find correct hyper; use id to validate; 
				for(int j = 0; j < hypervisor_v.size(); j++){
					if(hypervisor_v[j].comp_id(id) == 0){
						index = j;
						j = hypervisor_v.size();
					}
				}
				if(index<0){
					printf("Error-> HYPERVISOR NOT FOUND IN KNOWLEDGE BASE\n");
					break;
				}
				else{
					int n = 1;
					while(!break_mesg[n].empty()){
						hypervisor_v[index].add_interface(break_mesg[n], break_mesg[n+1]);
						n+=2;
					}
				}
			}
			*/
			break;

		case T_VIRT:
			parse_update(break_mesg, kp->task);
			//printf("Message after parsing: \n");
			//for(int k = 0; k < 9; k++){
			//	printf("%s\n",break_mesg[k].c_str());
			//	if(break_mesg[k+1].empty()){k=9;}
			//}
			//pthread_mutex_lock(&thread_array_mutex);
			printf("\n");
			printf("Searching for hypervisor...");
			
			//pthread_mutex_unlock(&thread_array_mutex);
			//pthread_mutex_lock(&kb_mutex);
			for(int i = 0; i < hypervisor_v.size(); i++){
				if(hypervisor_v[i].comp_id(id) == 0){
					index = i;
					printf("FOUND!\n\n");
					break;
				}
			}
			if(index<0){
				printf("Compare ID failed...\n");
				break;
			}
			hypervisor_v[index].update_interface(break_mesg[1], break_mesg[2]);
			hypervisor_v[index].print_cur_stats();
			sprintf(time_buff,"%.9f\n", (double)(clock() - mtd_timer)/CLOCKS_PER_SEC);
			write_mtdTime(time_buff);
			pthread_mutex_unlock(&kb_mutex);
			break;
		default:
			pthread_mutex_unlock(&kb_mutex);
			printf("SOMETHING WENT WRONG\n");
	}
	pthread_mutex_lock(&thread_array_mutex);
	valid_threads[kp->thread_index] = 0;
	pthread_mutex_unlock(&thread_array_mutex);
}


void* task_manager(void*){

	pthread_t thread_pool[MAXTHREAD];
	struct km_params km[MAXTHREAD];
	struct km_params temp;
	while(true){

		while(task_queue.empty()){
			sleep(1);
		}
		temp = task_queue.front();
		bool findFree = false;
		switch(temp.tskSource){
			case SOURCEKERN:
				while(!findFree){
					pthread_mutex_lock(&thread_array_mutex);
					for(int i = 0; i < MAXTHREAD; i++){
						if(valid_threads[i] == 0){
							km[i].thread_index = i;
							strncpy(km[i].task, temp.task, 100);
							valid_threads[i] = 1;
							printf("creating\n");
							pthread_create(&thread_pool[i], NULL, &handle_kern_mesg, &km[i]);
							findFree = true;
							break;
						}
					}
					pthread_mutex_unlock(&thread_array_mutex);
					if(!findFree){sleep(2);}
				}
				task_queue.pop();
				break;
			case SOURCESUPR:
				while(!findFree){
					pthread_mutex_lock(&thread_array_mutex);
					for(int i = 0; i < MAXTHREAD; i++){
						if(valid_threads[i] == 0){
							km[i].thread_index = i;
							strncpy(km[i].task, temp.task, 100);
							valid_threads[i] = 1;
							printf("creating\n");
							pthread_create(&thread_pool[i], NULL, &handle_SUPR_mesg, &km[i]);
							findFree = true;
							break;
						}
					}
					pthread_mutex_unlock(&thread_array_mutex);
					if(!findFree){sleep(2);}
				}
				task_queue.pop();
				break;
			default:
				printf("Something went wrong!\n");

		}
	}
}

void* kernel_listener(void*){
	printf("Kernel Listener Thread is up...\n");
	char rec_mesg_buff[RECMESGSIZE];
	struct sockaddr_in receivesocket;
	struct km_params tsk_M;
	//struct tm_queue task;
	int receivelen;
	int received = 0;
	pthread_t handler_t; 
	/* Create the UDP socket */
	if ((shared_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("socket");
		return NULL;
	}

	/* my address */
	memset(&receivesocket, 0, sizeof(receivesocket));
	receivesocket.sin_family = AF_INET;
	receivesocket.sin_addr.s_addr = htonl(INADDR_ANY);
	receivesocket.sin_port = htons(9999);

	receivelen = sizeof(receivesocket);
	if (bind(shared_sock, (struct sockaddr *) &receivesocket, receivelen) < 0) {
		perror("bind");
		return NULL;
	}

	while(1){
		memset(rec_mesg_buff, 0, RECMESGSIZE);
		if ((received = recvfrom(shared_sock, rec_mesg_buff, RECMESGSIZE, 0, NULL, NULL)) < 0){
			perror("recvfrom");
			return NULL;
		}
		printf("\n\n*****MESSEGE RECEIVED FROM KERNEL*****\n");
		printf("Kernel ID: ");
		printf("0x%01X ", (unsigned char)rec_mesg_buff[0]);
		printf("0x%01X ", (unsigned char)rec_mesg_buff[1]);
		printf("0x%01X \n", (unsigned char)rec_mesg_buff[2]);
		printf("Return Code: ");
		printf("0x%02X \n", (unsigned char)rec_mesg_buff[3]);
		printf("Message: %s\n",rec_mesg_buff+4);
		strncpy(tsk_M.task, rec_mesg_buff, sizeof(rec_mesg_buff));
		tsk_M.tskSource = SOURCEKERN;
		printf("Adding kernel message to task task_manager...\n");
		task_queue.push(tsk_M);
	}
}


void* kern_init(void*){
	printf("\nKernel initializtion thread is up...\n");
	int sendlen;
	char buffer[BUFFSIZE] = {0xB7, 0xE2, 0xAC, 0x55}; //Initial ID verify
	struct sockaddr_in sendsocket;

	/* kernel address */
	memset(&sendsocket, 0, sizeof(sendsocket));
	sendsocket.sin_family = AF_INET;
	sendsocket.sin_addr.s_addr = inet_addr("127.0.0.1");
	sendsocket.sin_port = htons(1215);

	sendlen = strlen(buffer) + 1;
	if (sendto(shared_sock, buffer, sendlen, 0, (struct sockaddr *) &sendsocket, sizeof(sendsocket)) != sendlen) {
		perror("sendto");
		return NULL;
	}
	printf("\nKernel initializtion thread is finished...\n");
	return NULL;
}

void* supr_listener(void* parameters){
	struct sv_params* p = (struct sv_params*) parameters;
	int m, n, s, len;
	char buff[BUFFSIZE] = {};
	struct km_params tsk_M;
	struct sockaddr_in cli2_addr;
	s = sizeof(cli2_addr);
	while(1){
		bzero(buff, BUFFSIZE);
		if((m = read(p->sock, buff, sizeof(buff))) < 0){
			perror("read err");
			return NULL;
		}
		if(m == 0){
			printf("Connection lost, attempting reconnect...\n");
			close(p->sock);
			while(m == 0){
				close(p->sock);
				if ((p->sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
				perror("socket ERR");
				return NULL;
				}
				len = sizeof(struct sockaddr_in);
				if (connect(p->sock, (struct sockaddr *)&to_supervisor, len) == -1) {
					perror("connect");
					//sleep(3);
					//m = 0;
				}
				if((m = read(p->sock, buff, sizeof(buff))) < 0){
					sleep(3);
					m = 0;
				}
			}
			printf("Reconnected!\n");
		}
		else{
			printf("Command Recieved from supervisor\n");
			printf("Command: %s\n", buff);
			strncpy(tsk_M.task, buff, sizeof(buff));
			tsk_M.tskSource = SOURCESUPR;
			printf("Adding kernel message to task task_manager...\n");
			task_queue.push(tsk_M);
		}
	}
	//close(p->sock);
	return NULL;
}

int main(int argc, char *argv[]){

	int sup_sock, len; //sup_sock = supervisor socket fda
	struct sv_params sv_conn_params;
	struct hostent *server;
	pthread_t task_manager_t;
	pthread_t supervisor_t;
	//pthread_t kernel_s_t;
	pthread_t kernel_l_t;
	pthread_create(&kernel_l_t, NULL, &kernel_listener, NULL);
	pthread_create(&task_manager_t, NULL, &task_manager, NULL);
	//pthread_create(&kernel_s_t, NULL, &kern_init, NULL);
	//pthread_join(kernel_s_t, NULL);
	if ((sup_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket ERR");
		return 1;
	}
    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        return 1;
    }
	//pthread_create(&kernel_s_t, NULL, &kern_demo, NULL);
	
	printf("Trying to connect to supervisor...\n");
    bzero((char *) &to_supervisor, sizeof(to_supervisor));
    to_supervisor.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&to_supervisor.sin_addr.s_addr, server->h_length);
    to_supervisor.sin_port = htons(20000);
    len = sizeof(struct sockaddr_in);

    if (connect(sup_sock, (struct sockaddr *)&to_supervisor, len) == -1) {
        perror("\nSupervisor connection error: ");
        close(sup_sock);
        //exit(1);
    }
    else{
    	sv_conn_params.sock = sup_sock;
	    printf("Connected to supervisor.\n");
	    pthread_create(&supervisor_t, NULL, &supr_listener, &sv_conn_params);
	    pthread_join(supervisor_t, NULL);
    }
	//pthread_join(kernel_s_t, NULL);
    pthread_join(kernel_l_t, NULL);
    pthread_join(task_manager_t, NULL);
 	close(sup_sock);
 	close(shared_sock);
	return 0;
}
