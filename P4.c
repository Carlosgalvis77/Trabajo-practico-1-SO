#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>


int main(int argc, char *argv[]){
  sem_unlink(sem2);
  sem_t *sem2;
  sem2 = sem_open("/sem2",O_CREAT,0666,0);
  if (sem2== SEM_FAILED ){
    perror("Ya esta creado el semaforo\n");
  } 
  sem_wait(sem2);

  int fd = open("/tmp/myfifo", O_WRONLY);
  if((fd < 0)){

    perror("Error en open\n");
    return(-4);

  }
  int N;
  if((read(fd,&N,sizeof(int)))<0){

    perror("Error en write de N\n");
    return(-4);

  }close(fd);
  
  const char NOMBRE []= "/MEMP3";
  int fd = shm_open(NOMBRE, O_RDONLY, 0666);
  if (fd < 0) {
    perror("Error en shm_open"); 
    return(-1);
  }

  void *ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (ptr == MAP_FAILED) {
    perror("Error MAP_FAILED");
    return (-3);
  }
  int arref[N];
  memcpy(arref,(char *)ptr+(N * sizeof(int)), sizeof(arref));
  munmap(ptr,SIZE);
  close(fd);

}