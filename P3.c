#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <stdlib.h>



int main(int argc, char *argv[]){

  sem_unlink(sem1);
  sem_t *sem1;
  sem1 = sem_open("/sem1",O_CREAT,0666,0);
  if (sem1=="SEM_FAILED"){
    perror("Ya esta creado el semaforo\n");
  } 
  sem_wait(sem1);
  //aca aun esta bloqueado P3 toca ver en que momento se desbloquea
  
  
  
  
  //buscar manera para que el tamaño sea el parametro N*2 convertido en bytes
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
  //creacion de memoria compartida:
  const char NOMBRE[]= "/MEMP3";
  const int SIZE = (N^2)*8;
  shm_unlink(NOMBRE);
  int fd = shm_open(NOMBRE, O_CREAT | O_RDONLY, 0666);
  if (fd < 0) {
    perror("Error en shm_open");
    return(-1);
  }

  if(ftruncate(fd, SIZE) < 0){
    perror("Error en ftruncate()\n");
    return (-2);
  }

  void *ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (ptr == MAP_FAILED) {
    perror("Error MAP_FAILED");
    return (-3);
  }
  int arref[N];
  memcpy(arref,ptr,sizeof(arref));
  munmap(ptr,SIZE);
  close(fd);
                                 
  
}
