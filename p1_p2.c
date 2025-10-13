#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <math.h>

int main(int argc, char *argv[]){
    sem_t *sem1 = sem_open("/sem1", 0); 
    if (sem1 == SEM_FAILED) {
        perror("sem_open P3 en P1");
        return -1;
    }
    sem_t *sem2 = sem_open("/sem2", 0);
    if (sem2 == SEM_FAILED) {
        perror("sem_open P4 en P1");
        return -2;
    }
    
    sem_unlink("/semP");
    sem_t *semP = sem_open("/semP", O_CREAT, 0666, 1);
    if (semP == SEM_FAILED) {
        perror("sem_open P1");
        return -2;
    }

    sem_unlink("/semH");
    sem_t *semH = sem_open("/semH", O_CREAT, 0666, 0);
    if (semH == SEM_FAILED) {
        perror("sem_open P2");
        return -2;
    }
    printf("p1 trabajando\n");
    //Mkfifo para mandar el N a P3 para crear la memoria compartida
    unlink("/tmp/myfifo");
    if((mkfifo("/tmp/myfifo",0666))<0){
    
        perror("Error en mkfifo\n");
        return(-4);
    }
    int fd = open("/tmp/myfifo", O_WRONLY | O_CREAT);
    if(fd <  0){

        perror("Error en open\n");
        return(-4);

    }
    
    int N = atoi(argv[1]);
    if((write(fd,&N,sizeof(int)))<0){

        perror("Error en write de N\n");
        return(-4);

    }
    printf("se envio N a P3 y P4, despertando a p3 y p4\n");
    sem_post(sem1);
    sem_post(sem2);

    close(fd);
    int v1, v2;
    if (sem_getvalue(sem1, &v1) == -1 && sem_getvalue(sem2, &v2) == -1) {
        perror("P3 o P4 no se han ejecutado");
        return -1; 
    }

    if (v1 == 0 && v2 ==0){

        pid_t P2=fork();

        if(P2>0){

            int a = atoi(argv[2]);
            int b = atoi(argv[3]);

            // Aca se crea el bufer y se agrega b
            const char NOMBRE [] = "/MEMP3";
            const int SIZE = (2 * (N+2)) * sizeof(int);
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
            int j = 0;
            for(int i = 0;i < atoi(argv[1]); i++){

                //PUNTO CRITICO
                sem_wait(semP);
                b = a + b;
                a = b;
        
                if((memcpy(((char *)ptr + j*sizeof(int)),&b,sizeof(int))) == NULL){
                    perror("Error Memcpy");
                    return (-5);
                }
                j += 2;
                if(i == atoi(argv[1]) - 1){
                    int testigo_p1 = -1;
                    if((memcpy(((char *)ptr + (j+2)*sizeof(int)),&testigo_p1,sizeof(int))) == NULL){
                        perror("Error Memcpy");
                        return (-6);
                    }
                    sem_post(sem1);
                }
                sem_post(semH);
                //FIN DE PUNTO CRITICO
            munmap(ptr,SIZE);
            close(fd);
            }    
        }else if(P2==0){
            const char NOMBRE [] = "/MEMP3";
            const int SIZE = (2 * (N+2)) * sizeof(int);
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
            int l = 1;
            int z = atoi(argv[4]);
            for(int i= 0; i < atoi(argv[1]);i++){
                //PUNTO CRITICO
                sem_wait(semH);
                int potencia_de_dos = pow(2,z);
                z += 1;
                //aca se agrega el dato al buffer 2
                if((memcpy(((char *)ptr + l*sizeof(int)),&potencia_de_dos,sizeof(int))) == NULL){
                    perror("Error Memcpy");
                    return (-5);
                }
                l += 2;
                if(i == atoi(argv[1]) - 1){
                    int testigo_p2 = -2;
                    if((memcpy(((char *)ptr + (l+2)*sizeof(int)),&testigo_p2,sizeof(int))) == NULL){
                        perror("Error Memcpy");
                        return (-6);
                    }
                    sem_post(sem1);
                }
                sem_post(semP);
                //FIN DE PUNTO CRITICO
            }
            munmap(ptr,SIZE);
            close(fd);
        }else{
            perror("Fallo al crear P1\n");
        }
    }                               

}
