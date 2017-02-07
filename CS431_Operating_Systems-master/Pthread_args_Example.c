/*
Example of how to pass arguments to pthreads in C
*/

#include <pthread.h>
#include <stdio.h>

//Define struct to pass to new threads --similar to object private variables
struct char_print_parms
{
  char character;  // character to print
  int count;    //number of times to print the character
};



//thread that will accept an input argument --write it just like a function/method
void* char_print (void* parameters)
{
  //typecasting paramters argument --> In effect overlaying the struct on Data
  struct char_print_parms* p = (struct char_print_parms*) parameters; //VERY IMPORTANT PART
  int i;

  for (i=0; i < p->count; ++i)
    fputc(p->character, stderr);
  return NULL;
}

int main()
{
  pthread_t thread1_id;   //will store ID for thread 1
  pthread_t thread2_id;   //will store ID for thread 2

  //Note we will pass the pointer to these as in the location within RAM DATA
  struct char_print_parms thread1_args;  //argument for thread 1
  struct char_print_parms thread2_args;  //argument for thread 2

  //set values of thread1_args
  thread1_args.character = 'x';
  thread1_args.count = 20000;

  //create thread1
  pthread_create (&thread1_id, NULL, &char_print, &thread1_args);


  //set values of thread2_args
  thread2_args.character = 'o';
  thread2_args.count = 10000;

  //create thread2
  pthread_create (&thread2_id, NULL, &char_print, &thread2_args);

  pthread_join(thread1_id, NULL);
  pthread_join(thread2_id, NULL);

  return 0;
}

