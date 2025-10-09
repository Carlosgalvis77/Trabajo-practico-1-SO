#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *args[]){
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
  if((read(fd,N,sizeof(int)))<0){

    perror("Error en write de N\n");
    return(-4);

  }close(fd);
  //creacion de memoria compartida:
  const char NOMBRE = "/MEMP3";
  const int SIZE = (N^2)*8;
  int fd;
  void *ptr;

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

  ptr = mmap(NULL, SIZE, PROT_WRITE, MAP_SHARED, fd, 0);
  if (ptr == MAP_FAILED){
      perror("Error MAP_FAILED");
      return (-3);
  }
                                 
  
}
