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
    
    //Mkfifo para mandar el N a P3 para crear la memoria compartida
    int fdp3 = open("/tmp/myfifo", O_WRONLY);
    if(fdp3 <  0){

        perror("Error en open\n");
        return(-4);

    }
    int N = atoi(argv[1]);
    if((write(fdp3,&N,sizeof(int)))<0){

        perror("Error en write de N\n");
        return(-4);

    }
    close(fdp3);

    int fdp4 = open("/tmp/myfifo1", O_WRONLY);
    if(fdp4 <  0){

        perror("Error en open\n");
        return(-4);

    }
    if((write(fdp4,&N,sizeof(int)))<0){

        perror("Error en write de N\n");
        return(-4);

    }
    close(fdp4);

    int v1, v2;
    if (sem_getvalue(sem1, &v1) == -1 || sem_getvalue(sem2, &v2) == -1) {
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
            int fdp = shm_open(NOMBRE, O_RDWR, 0666);
            if (fdp < 0) {
                perror("Error en shm_open"); 
                return(-1);
            }

            void *ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fdp, 0);
            if (ptr == MAP_FAILED) {
                perror("Error MAP_FAILED");
                return (-3);
            }
            int j = 0;
            for(int i = 0;i < N; i++){

                //PUNTO CRITICO
                sem_wait(semP);
                int next = a + b;
                a = b;
                b = next;   

                if((memcpy(((char *)ptr + (j)*sizeof(int)),&b,sizeof(int))) == NULL){
                    perror("Error Memcpy");
                    return (-5);
                }
                j += 2;
                sem_post(semH);
                //FIN DE PUNTO CRITICO
            }
            sem_wait(semP);
            int testigo_p1 = -1;
            if((memcpy(((char *)ptr + (j+2)*sizeof(int)),&testigo_p1,sizeof(int))) == NULL){
                perror("Error Memcpy");
                return (-6);
            }
            sem_post(sem1);
            sem_post(semH);

            munmap(ptr,SIZE);
            close(fdp);    
        }else if(P2==0){
            const char NOMBRE [] = "/MEMP3";
            const int SIZE = (2 * (N+2)) * sizeof(int);
            int fdh = shm_open(NOMBRE, O_RDWR, 0666);
            if (fdh < 0) {
                perror("Error en shm_open"); 
                return(-1);
            }

            void *ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fdh, 0);
            if (ptr == MAP_FAILED) {
                perror("Error MAP_FAILED");
                return (-3);
            }
            int l = 1;
            int z = atoi(argv[4]);
            for(int i= 0; i < N;i++){
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
                sem_post(semP);
                //FIN DE PUNTO CRITICO
            }
            sem_wait(semH);
            int testigo_p2 = -2;
            if((memcpy(((char *)ptr + (l+2)*sizeof(int)),&testigo_p2,sizeof(int))) == NULL){
                perror("Error Memcpy");
                return (-6);
            }
            sem_post(sem2);
            sem_post(semP);
            
            munmap(ptr,SIZE);
            close(fdh);
        }else{
            perror("Fallo al crear P1\n");
        }
    }                               

}
