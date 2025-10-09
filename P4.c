
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int argc, char *args[]){
  sem_t *sem2;
  sem2 = sem_open("/sem2",O_CREAT,0666,0);
  if (sem2== SEM_FAILED ){
    perror("Ya esta creado el semaforo\n");
  } 
  sem_wait(sem2);
  
  const char NOMBRE = "/MEMP3";
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

}