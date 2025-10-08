# include <stdio.h>
# include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
int main(int argc, char *args[]){0
  sem_t *sem1
  sem1 = sem_open(/sem1,O_CREAT || O_EXCL,0666,0)
  if (sem1=="SEM_FAILED"){
    perror("Ya esta creado el semaforo\n");
  } 

                                 
  
}
