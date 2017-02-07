/*
	Author:	Anthony Portante 
	
	Last Modified: 13 March, 2015
*/
#include <iostream>
#include <ctime>
#include <unistd.h>
#include <stdio.h>
#include "apTimer.h"

using namespace std;

int end_hour = 0;
int end_min = 0;
int end_sec = 0;
int flags = 0;
void* threadable_aptimer(void* parameters){
	struct t_params *temp = (struct t_params*)parameters;
	time_t now;
	struct tm end_time;
	double seconds;

	time(&now);
	end_time = *localtime(&now);

	end_time.tm_hour = temp->hour; end_time.tm_min = temp->min; end_time.tm_sec = temp->sec;

	seconds = difftime(now,mktime(&end_time));
	seconds = (int)-1*seconds;
	if(flags == DEBUG){
		printf("Num secs %d\n", (int)seconds );
		printf("Sleep til %d:%d:%d\n",temp->hour, temp->min, temp->sec);
	}
	sleep(seconds);
}
/*
	Repeatable timer, uses the global time variables to mark termination
	Can alter end time while thread running via set_time()
*/
void* ap_repeatable(void){
	if(!end_hour){return (void*)NULL;} //If no time is set, return
	time_t now;
	struct tm end_time;
	double seconds;

	while(1){
		time(&now);
		end_time = *localtime(&now);

		end_time.tm_hour = end_hour; end_time.tm_min = end_min; end_time.tm_sec = end_sec;

		seconds = difftime(now,mktime(&end_time));
		seconds = (int)-1*seconds;
		if(flags == DEBUG){
			printf("Num secs %d\n", (int)seconds );
			printf("Sleep til %d:%d:%d\n",end_hour, end_min, end_sec);
		}
		sleep(seconds);
	}
}

void ap_timer(int h, int m, int s){
	time_t now;
	struct tm end_time;
	double seconds;

	time(&now);
	end_time = *localtime(&now);

	end_time.tm_hour = h; end_time.tm_min = m; end_time.tm_sec = s;

	seconds = difftime(now,mktime(&end_time));
	seconds = (int)-1*seconds;
	if(flags == DEBUG){
		printf("Num secs %d\n", (int)seconds );
	}
	sleep(seconds);
}

int set_flags(int ftset){ //flag to set
	switch(ftset){
		case DEBUG:
			flags = DEBUG;
			printf("FLAG SET => DEBUG\n");
			break;
		default:
			flags = 0;
			break;
	}
	return flags;
}
void set_time(int h, int m, int s){
	end_hour = h;
	end_min = m;
	end_sec = s;
}

void set_hour(int h){
	end_hour = h;
}
void set_min(int m){
	end_min = m;
}
void set_sec(int s){
	end_sec = s;
}
