#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define DONT_ACCESS -1
#define OK_ACCESS 1
#define BUFFER_MAX 10
#define FILE_LENGTH 0x300

int produce_widget()
{
	return 1;
}

int main(int argc, char* const argv[])
{
  int fd;
  int buff_num;
  int widget;
  int access_cmd;
  void* buffer_memory;
  void* access_memory;

  //OPEN BUFFER FILE & CREATE MEMORY MAPPING
  fd = open("buffer.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  buffer_memory = mmap(0, FILE_LENGTH, PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd); //closing file access in HD

  
  //OPEN ACCESS CONTROL FILE & CREATE MEMORY MAPPING
  fd = open("access.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  access_memory = mmap(0, FILE_LENGTH, PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd); //closing file access in HD

  //Read Access Integer, print it out
  sscanf(access_memory, "%d", &access_cmd); 
  printf("Value: %d\n", access_cmd);
  access_cmd = OK_ACCESS;
  while(1)
  {
	widget = 0;
	while(access_cmd == -1) //wait until told its ok to access
	{
		sscanf(access_memory, "%d", &access_cmd); 
		printf("producer waiting\n");
	}
	//TELL CONSUMER TO NOT ACCESS BUFFER
	sprintf((char*) access_memory, "%d\n", DONT_ACCESS);
	sscanf(buffer_memory, "%d", &buff_num);
	printf("buffer: %d \n", buff_num);
	if(buff_num == BUFFER_MAX)
	{
		//Since buffer full, tell the consumer its ok to access the buffer
		printf("producer->memory is full\n");
		sprintf((char*) access_memory, "%d\n", OK_ACCESS);
	}
	else
	{
		widget = produce_widget(); //produce widget
		buff_num += widget; //insert widget
		sprintf((char*)buffer_memory, "%d\n", buff_num);//update shared buffer
		sprintf((char*) access_memory, "%d\n", OK_ACCESS); //tell consumer its ok to access		
	}
	access_cmd = -1; // reset access
	sleep(0.5); //used to make it easier to see the acton
  }

  //Release the memory --> memory will automatically be released when 
  munmap (buffer_memory, FILE_LENGTH);
  munmap (access_memory, FILE_LENGTH);

  return 0;

}
