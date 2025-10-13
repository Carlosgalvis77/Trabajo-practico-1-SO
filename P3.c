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
  } 

  int fd = open("/tmp/myfifo", O_RDONLY);

  if((fd < 0)){

    perror("Error en open\n");
    return(-4);
  }

  int N;
  if((read(fd,&N,sizeof(int)))<0){

    perror("Error en write de N\n");
    return(-4);

  }
  close(fd);

  //creacion de memoria compartida:
  const char NOMBRE[]= "/MEMP3";
  const int SIZE = (2 * (N+2)) * sizeof(int);
  shm_unlink(NOMBRE);
  int fd1 = shm_open(NOMBRE, O_CREAT | O_RDWR, 0666);
  if (fd1 < 0) {
    perror("Error en shm_open");
    return(-1);
  }

  if(ftruncate(fd1, SIZE) < 0){
    perror("Error en ftruncate()\n");
    return (-2);
  }

  void *ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
  if (ptr == MAP_FAILED) {
    perror("Error MAP_FAILED");
    return (-3);
  }
  sem_wait(sem1);  
  
  //sem_wait(semH);

  //aca se altenra la lectura de memoria compartida
  int k = 0;
  for(int i = 0; i < N; i++){
    int digito_fibo;
    if (memcpy(&digito_fibo, (char *)ptr + k*sizeof(int), sizeof(digito_fibo)) == NULL){
      perror("Error Memcpy");
      return (-7);
    }
    //semaforo
    printf(digito_fibo);
    k += 2;
    if(i == atoi(argv[1]) - 1){
      int testigo_p3 = -3;
      //se manda por memoria compartida
      int fd2 = open("/tmp/myfifo", O_WRONLY | O_CREAT);

      if((fd2 < 0)){
        perror("Error en open\n");
        return(-4);
      }
      if((write(fd2,&testigo_p3,sizeof(int)))<0){

        perror("Error en write de N\n");
        return(-4);

      }
      close(fd2);
                    
    }
    munmap(ptr,SIZE);
    close(fd1);     
  }                          
  
  
}
