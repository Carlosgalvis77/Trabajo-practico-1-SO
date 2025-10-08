#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *args[]){
  sem_t *sem1 = sem_open("/sem1", 0); 
    if (sem1 == SEM_FAILED) {
        perror("sem_open");
        return -1;
    }
  sem_t *sem2 = sem_open("/sem2", 0);
    if (sem2 == SEM_FAILED) {
        perror("sem_open");
        return -2;
    }
  if((int sem_getvalue(sem1)<=-1) && (int sem_getvalue(sem2)<=-1 )){
    pid_t P2=fork();
    if(P2>0){
    
    }else if(P2==0){
    
    }else{
      perror("Fallo al crear P1\n");
    }
                                 
  
}
