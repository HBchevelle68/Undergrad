/*
Use FORK() to duplicate process[fifthproc] in a new address space
then EXECVP() within child process to load program[ls] and input arguments


After you run initial program make changes as comments below suggest to
properly wait() for child to end and avoid ZOMBIE PROCESS!
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int spawn( char* program, char** arg_list)
{
  pid_t child_pid;  //PID = 16 bit value 
  
  child_pid = fork();  //create two process 
  if (child_pid != 0)  //parent process
    return child_pid;
  
  else   //child process
    {
      execvp(program, arg_list);     
      
//will only return if there is an error
      fprintf(stderr, "An error occured in execvp\n");
      abort(); 
    }
}

int main()
{
  int child_status;   

  //argument list to pass NULL terminated!
  char* arg_list[] = { "ls", "-1", "/", NULL}; //where argv[0] = "ls"

  spawn("ls", arg_list);
  
  printf("the main program spawned a new program and is Exiting\n");

  wait (&child_status);
  	if (WIFEXITED (child_status))
    		printf ("the child process exited normally, with exit code %d\n", WEXITSTATUS(child_status));
  	else
    		printf("the child process exited abnormally\n");
  return 0;
  

/*replace return 0 with

wait (&child_status);
  if (WIFEXITED (child_status))
    printf ("the child process exited normally, with exit code %d\n", WEXITSTATUS(child_status));
  else
    printf("the child process exited abnormally\n");
  return 0;

*/

}
