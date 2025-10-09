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
  //Mkfifo para mandar el N a P3 para crear la memoria compartida
  unlink("/tmp/myfifo");
  if((mkfifo("/tmp/myfifo",0666))<0){
    
    perror("Error en mkfifo\n");
    return(-4);
  }
  int fd = open("/tmp/myfifo", O_WRONLY | O_CREAT)
  if(fd <  0){

    perror("Error en open\n");
    return(-4);

  }

  if((write(fd,args[1],sizeof(int)))<0){

    perror("Error en write de N\n");
    return(-4);

  }close(fd);

  }
  if((int sem_getvalue(sem1)<=-1) && (int sem_getvalue(sem2)<=-1 )){
    pid_t P2=fork();
    if(P2>0){

      int a = args[2];
      int b = args[3];

      // Aca se crea el bufer y se agrega b
        const char NOMBRE = "/MEMP3";
        int fd = shm_open(NOMBRE, O_RDWR, 0666);
        if (fd < 0) {
          perror("Error en shm_open"); 
          return(-1);
        }

        void *ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (ptr == MAP_FAILED) {
            perror("Error MAP_FAILED");
            return (-3);
        }
      for(i=0,i<args[1],i++){
        b = a + b;
        a = b;
        
        if(memcpy(ptr,b,sizeof(int));
      }
      munmap(ptr,SIZE);
        
    }else if(P2==0){

      c = args[4];
      for(i= arg[4],i<args[1],i++){
        int potencia_de_dos = 2**i;
        //aca se agrega el dato al buffer 2
      }

    }else{
      perror("Fallo al crear P1\n");
    }
                                  

}
