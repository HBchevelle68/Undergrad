/*
	Author:	Anthony Portante 
	
	Last Modified: 13 March, 2015
*/
#ifndef __apTimer_H  
#define __apTimer_H 

#define DEBUG 1

extern int end_hour;
extern int end_min;
extern int end_sec;
extern int flags;

/* ONLY USED FOR THREAD PARAMS*/
struct t_params{
	int hour;
	int min;
	int sec;
};

void set_time(int h, int m, int s); 
void set_hour(int h);
void set_min(int m);
void set_sec(int s);
void ap_timer(int h, int m, int s);
void* threadable_aptimer(void* parameters);
int set_flags(int ftset);

#endif
