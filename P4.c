
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int argc, char *args[]){
  sem_t *sem2
  sem2 = sem_open("/sem2",O_CREAT,0666,0)
  if (sem2== SEM_FAILED ){
    perror("Ya esta creado el semaforo\n");
  } 
  sem_wait(sem2);
                                 
  
}
