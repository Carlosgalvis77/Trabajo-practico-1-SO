#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char *argv[]){
  sem_t *sem2;
  sem2 = sem_open("/sem2",O_CREAT,0666,0);
  if (sem2== SEM_FAILED ){
    perror("Ya esta creado el semaforo\n");
    return -33;
  } 

  sem_t *semP3 = sem_open("/semP3", 0); 
    if (semP3 == SEM_FAILED) {
        perror("sem_open P3 en P4");
        return -34;
    }

  sem_t *semP4 = sem_open("/semP4", 0); 
  if (semP4 == SEM_FAILED) {
      perror("sem_open P4 en P4");
      return -35;
  }
  
  printf("Esperando a P1\n");
  unlink("/tmp/myfifo1");
  if((mkfifo("/tmp/myfifo1",0666))<0){
  
    perror("Error en mkfifo1\n");
    return(-36);
  }

  int fd = open("/tmp/myfifo1", O_RDONLY);
  if(fd <  0){

    perror("Error en open\n");
    return(-37);

  }

  int N;
  if((read(fd,&N,sizeof(int)))<0){

    perror("Error en read de N\n");
    return(-38);

  }
  close(fd);

  sem_wait(sem2);
  
  const char NOMBRE []= "/MEMP3";
  const int SIZE = (2 * (N+2)) * sizeof(int);
  int fd2 = shm_open(NOMBRE, O_RDWR, 0666);
  if (fd2 < 0) {
    perror("Error en shm_open"); 
    return(-39);
  }

  void *ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
  if (ptr == MAP_FAILED) {
    perror("Error MAP_FAILED");
    return (-40);
  }

  int h = 1;
  for(int i = 0; i < N; i++){
    sem_wait(semP4);
    int digito_potencia;
    if (memcpy(&digito_potencia, (char *)ptr + h*sizeof(int), sizeof(int)) == NULL){
      perror("Error Memcpy");
      return (-41);
    }
    //semaforo
    printf("%d\n", digito_potencia);
    h += 2; 
    sem_post(semP3); 
  }        
  
  sem_wait(semP4);
  int testigo_p4 = -3;
  //se manda por memoria compartida
  int fd3 = open("/tmp/myfifo1", O_WRONLY | O_CREAT);

  if((fd3 < 0)){
    perror("Error en open\n");
    return(-42);
  }
  if((write(fd3,&testigo_p4,sizeof(int)))<0){

    perror("Error en write de N\n");
    return(-43);

  }
  sem_post(semP3);
  close(fd3); 
  munmap(ptr,SIZE);
  close(fd2); 
  sem_close(sem2);
  sem_close(semP3);
  sem_close(semP4);
  unlink("/tmp/myfifo1");   
  printf("P4 termina\n");        
}