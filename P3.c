#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <stdlib.h>



int main(int argc, char *argv[]){

  sem_unlink("/sem1");
  sem_t *sem1;
  sem1 = sem_open("/sem1",O_CREAT,0666,0);
  if (sem1 == SEM_FAILED){
    perror("Ya esta creado el semaforo\n");
    return -21;
  }  

  sem_unlink("/semP3");
  sem_t *semP3;
  semP3 = sem_open("/semP3",O_CREAT,0666,1);
  if (semP3 == SEM_FAILED){
    perror("Ya esta creado el semaforo\n");
    return -22;
  }

  sem_unlink("/semP4");
  sem_t *semP4;
  semP4 = sem_open("/semP4",O_CREAT,0666,0);
  if (semP4 == SEM_FAILED){
    perror("Ya esta creado el semaforo\n");
    return -23;
  }
  
  printf("Esperando a P1\n");
  unlink("/tmp/myfifo");
  if((mkfifo("/tmp/myfifo",0666))<0){
  
    perror("Error en mkfifo\n");
    return(-24);
  }
  int fd = open("/tmp/myfifo", O_RDONLY);
  if(fd <  0){

    perror("Error en open\n");
    return(-25);

  }

  int N;
  if((read(fd,&N,sizeof(int)))<0){

    perror("Error en read de N\n");
    return(-26);

  }
  close(fd);
  //creacion de memoria compartida:
  shm_unlink("/MEMP3");
  const char NOMBRE[]= "/MEMP3";
  const int SIZE = (2 * (N+2)) * sizeof(int);
  int fd1 = shm_open(NOMBRE, O_CREAT | O_RDWR, 0666);
  if (fd1 < 0) {
    perror("Error en shm_open");
    return(-27);
  }

  if(ftruncate(fd1, SIZE) < 0){
    perror("Error en ftruncate()\n");
    return (-28);
  }

  void *ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
  if (ptr == MAP_FAILED) {
    perror("Error MAP_FAILED");
    return (-29);
  }

  sem_wait(sem1);
  
  //aca se altenra la lectura de memoria compartida
  int k = 0;
  for(int i = 0; i < N; i++){
    sem_wait(semP3);
    int digito_fibo;
    if (memcpy(&digito_fibo, (char *)ptr + k*sizeof(int), sizeof(int)) == NULL){
      perror("Error Memcpy");
      return (-30);
    }
   
    printf("%d\n", digito_fibo);
    k += 2;
    sem_post(semP4);
  }

  sem_wait(semP3);
  int testigo_p3 = -3;
  //se manda por memoria compartida
  int fd2 = open("/tmp/myfifo", O_WRONLY | O_CREAT);

  if((fd2 < 0)){
    perror("Error en open\n");
    return(-31);
  }

  if((write(fd2,&testigo_p3,sizeof(int)))<0){

    perror("Error en write de N\n");
    return(-32);
  }

  sem_post(semP4);

  close(fd2);                    
  munmap(ptr,SIZE);
  close(fd1);
  unlink("/tmp/myfifo");
  shm_unlink("/MEMP3");
  sem_close(sem1);
  sem_close(semP3);
  sem_close(semP4);   
  printf("P3 termina\n");                        
  
}

