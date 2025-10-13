#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>


int main(int argc, char *argv[]){
  sem_unlink("/sem2");
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
  
  sem_wait(sem2);

  const char NOMBRE []= "/MEMP3";
  const int SIZE = (2 * (N+2)) * sizeof(int);
  int fd1 = shm_open(NOMBRE, O_RDONLY, 0666);
  if (fd1 < 0) {
    perror("Error en shm_open"); 
    return(-1);
  }

  void *ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
  if (ptr == MAP_FAILED) {
    perror("Error MAP_FAILED");
    return (-3);
  }
  sem_wait(sem2);

  int h = 1;
  for(int i = 0; i < N; i++){
    int digito_potencia;
    if (memcpy(&digito_potencia, (char *)ptr + h*sizeof(int), sizeof(digito_potencia)) == NULL){
      perror("Error Memcpy");
      return (-9);
    }
    //semaforo
    printf("%d\n", digito_potencia);
    h += 2;
    int n_testigo_p4 = atoi(argv[1]) - 1;
    if(i == n_testigo_p4){
      int testigo_p4 = -3;
      //se manda por memoria compartida
      int fd2 = open("/tmp/myfifo", O_WRONLY | O_CREAT);

      if((fd2 < 0)){
        perror("Error en open\n");
        return(-4);
      }
      if((write(fd2,&testigo_p4,sizeof(int)))<0){

        perror("Error en write de N\n");
        return(-4);

      }
      close(fd2);
                    
    }
    munmap(ptr,SIZE);
    close(fd1);     
  }                 
}