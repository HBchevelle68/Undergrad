/*
 *     Author: Anthony Portante
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


//Structure is used to pass arguments to thread
//thread is calling spawn
struct char_parms
{
  char* m_program;    
};

//Spawn is called by a thread, this allows execvp
//to be called and main program can continue on to fork again
//since execvp essentially blocks until function returns
//but the functions will never return
void* spawn( void* parameters)
{
  struct char_parms* p = (struct char_parms*) parameters;
  pid_t child_pid;  //PID = 16 bit value 
  char* empty_arg[] = {NULL};

  child_pid = fork();  //create two process 
  if (child_pid != 0)  //parent process
    return;

  else   //child process
    {
      printf("%s", p->m_program);
      execvp(p->m_program, empty_arg);   //I passed in an ampty char*[] because the compiler kept yelling at me
      
      //will only return if there is an error
      fprintf(stderr, "An error occured in execvp\n");
      abort(); 
    }
}

int main()
{
  int child_status;   

  pthread_t prodThread, conThread; //create threads to fork and execvp

  struct char_parms thread1_args;  //argument for thread 1 (producer)
  struct char_parms thread2_args;  //argument for thread 2 (consumer)

  //insert executable name into thread arg struct
  thread1_args.m_program = "./producerproc"; 
  thread2_args.m_program = "./consumerproc";

  pthread_create(&prodThread, NULL, &spawn, &thread1_args); //fork and spawn the producer program 
  pthread_create(&conThread, NULL, &spawn, &thread2_args);  //fork and spawn the consumer program

  printf("the main program spawned new programs \n");

  //need to call thread join otherwise main program will continue and exit 
  pthread_join(prodThread, NULL);
  pthread_join(conThread, NULL);
  
  //here just in case
  wait (&child_status);
  if (WIFEXITED (child_status))
    printf ("the child process exited normally, with exit code %d\n", WEXITSTATUS(child_status));
  else
    printf("the child process exited abnormally\n");
  return 0;
  

}
