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

    if ((sem_open("/sem1",  0) == SEM_FAILED) ||  (sem_open("/sem2",  0) == SEM_FAILED)) {
        perror("P3 o P4 no se han ejecutado todavía\n");
        return 0;
    }else{
        sem_t *sem1 = sem_open("/sem1",  0);
        sem_t *sem2 = sem_open("/sem2",  0);

        sem_unlink("/semP");
        sem_t *semP = sem_open("/semP", O_CREAT, 0666, 1);
        if (semP == SEM_FAILED) {
            perror("sem_open PP");
            return -3;
        }

        sem_t *semP1 = sem_open("/semP1", 0); 
        if (semP1 == SEM_FAILED) {
            perror("sem_open P1");
            return -1;
        }  
        sem_unlink("/semH");
        sem_t *semH = sem_open("/semH", O_CREAT, 0666, 0);
        if (semH == SEM_FAILED) {
            perror("sem_open PH");
            return -4;
        }
        //Mkfifo para mandar el N a P3 para crear la memoria compartida
        int fdp3 = open("/tmp/myfifo", O_WRONLY);
        if(fdp3 <  0){

            perror("Error en open 22\n");
            return(-5);

        }
        int N = atoi(argv[1]);
        if((write(fdp3,&N,sizeof(int)))<0){

            perror("Error en write de N\n");
            return(-6);

        }
        close(fdp3);

        int fdp4 = open("/tmp/myfifo1", O_WRONLY);
        if(fdp4 <  0){

            perror("Error en open\n");
            return(-7);

        }
        if((write(fdp4,&N,sizeof(int)))<0){

            perror("Error en write de N\n");
            return(-8);

        }
        close(fdp4);
    
        int v1, v2;
        sem_getvalue(sem1, &v1);
        sem_getvalue(sem2, &v2);
        if (sem_getvalue(sem1, &v1) < 0) {
            perror("sem_getvalue(sem1)");
            return -2;
        }
        if (sem_getvalue(sem2, &v2) < 0) {
            perror("sem_getvalue(sem2)");
            return -3;
        }

        if(v1 == 0 && v2 ==0){
            //paro a p1 
            sem_wait(semP1);
            //creo a p2
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
                    return(-10);
                }

                void *ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fdp, 0);
                if (ptr == MAP_FAILED) {
                    perror("Error MAP_FAILED");
                    return (-11);
                }
                int j = 0;
                for(int i = 0;i < N; i++){

                    //PUNTO CRITICO
                    sem_wait(semP);
                    int next = a + b;
                    a = b;
                    b = next;   
            
                    if((memcpy(((char *)ptr + j*sizeof(int)),&b,sizeof(int))) == NULL){
                        perror("Error Memcpy");
                        return (-12);
                    }
                    j += 2;
                    sem_post(semH);
                    //FIN DE PUNTO CRITICO
                }
                int testigo_p1 = -1;
                if((memcpy(((char *)ptr + (j+2)*sizeof(int)),&testigo_p1,sizeof(int))) == NULL){
                    perror("Error Memcpy");
                    return (-6);
                }
                munmap(ptr,SIZE);
                close(fdp); 
                
                int fd2 = open("/tmp/myfifo", O_RDONLY);

                if((fd2 < 0)){
                    perror("Error en open 1\n");
                    return(-13);
                }
                int testigo_p3;
                if((read(fd2,&testigo_p3,sizeof(int)))<0){
                    
                    perror("Error en read de testigo3\n");
                    return(-14);
                }else if(testigo_p3 == -3){
                    sem_post(semH);  
                    sem_wait(semP);  
                    write(STDOUT_FILENO, "P1 termina\n", 11);
                    sem_wait(semP);
                    close(fd2);
                    unlink("/tmp/myfifo"); 
                    sem_close(sem2);
                    sem_close(sem1);
                    sem_close(semP1);
                    sem_close(semP);
                    sem_close(semH); 

                }
                
            }else if(P2==0){
                const char NOMBRE [] = "/MEMP3";
                const int SIZE = (2 * (N+2)) * sizeof(int);
                int fdh = shm_open(NOMBRE, O_RDWR, 0666);
                if (fdh < 0) {
                    perror("Error en shm_open"); 
                    return(-15);
                }

                void *ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fdh, 0);
                if (ptr == MAP_FAILED) {
                    perror("Error MAP_FAILED");
                    return (-16);
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
                        return (-17);
                    }
                    l += 2;
                    sem_post(semP);
                    //FIN DE PUNTO CRITICO
                }
                int testigo_p2 = -2;
                if((memcpy(((char *)ptr + (l+2)*sizeof(int)),&testigo_p2,sizeof(int))) == NULL){
                    perror("Error Memcpy");
                    return (-18);
                }
                //Activo a p3
                sem_post(sem1);
                //activo a p4
                sem_post(sem2);
                
                munmap(ptr,SIZE);
                close(fdh);

                int fdhmy = open("/tmp/myfifo1", O_RDONLY);

                if((fdhmy < 0)){
                    perror("Error en open 2\n");
                    return(-19);
                }
                int testigo_p4;
                if((read(fdhmy,&testigo_p4,sizeof(int)))<0){

                    perror("Error en read de testigo4\n");
                    return(-20);
                }else if (testigo_p4  == -3){

                    sem_wait(semH);
                    write(STDOUT_FILENO, "P2 termina\n", 11);
                    sem_post(semP);

                    close(fdhmy);
                    unlink("/tmp/myfifo1"); 

                    sem_close(sem2);
                    sem_close(sem1);
                    sem_close(semP1);
                    sem_close(semH);
                    sem_post(semP);
                    sem_close(semP);

                } 
                
                
            }else{
                perror("Fallo al crear P1\n");
            }
            sem_unlink("/sem1");
            sem_unlink("/sem2");
            sem_unlink("/semP");
            sem_unlink("/semP1");
            sem_unlink("/semH");      
        }
    }                   
}
