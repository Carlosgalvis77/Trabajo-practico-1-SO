#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *args[]){
  sem_t *sem1
  sem1 = sem_open("/sem1",O_CREAT,0666,0)
  if (sem1=="SEM_FAILED"){
    perror("Ya esta creado el semaforo\n");
  } 
  sem_wait(sem1);
    
                                 
  
}
