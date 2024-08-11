#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sched.h>
#include <syscall.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdbool.h>

pthread_cond_t* condition;
pthread_mutex_t* mutex;

#define OKTOWRITE "/oktowrite"
#define MESSAGE "/message"
#define MUTEX "/lock"
struct dtc_status_t {
  pthread_cond_t condition;
  pthread_mutex_t mutex;
};

struct dtc_status_t* dtc_status;
int fill_buffer(char *bufptr, int size);

int main(int argc, char *argv[])
{
    int des_cond, des_msg, des_mutex;
    int mode = S_IRWXU | S_IRWXG;

    /* set mutex shared between processes */
    pthread_mutexattr_t mutexAttr;
    pthread_mutexattr_init(&mutexAttr);
    pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);

    /* set condition shared between processes */
    pthread_condattr_t condAttr;
    pthread_condattr_init(&condAttr);
    pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_SHARED);

    des_mutex = shm_open(MUTEX, O_RDWR, mode);

    if (des_mutex < 0) {
        perror("failure on shm_open on des_mutex");
        exit(1);
    }

    dtc_status = (dtc_status_t*) mmap(NULL, sizeof(dtc_status_t),PROT_READ | PROT_WRITE, MAP_SHARED, des_mutex, 0);

    if (mutex == MAP_FAILED ) {
        perror("Error on mmap on mutex\n");
        exit(1);
    }

    des_cond = shm_open(OKTOWRITE, O_RDWR, mode);

    if (des_cond < 0) {
        perror("failure on shm_open on des_cond");
        exit(1);
    }

    condition = (pthread_cond_t*) mmap(NULL, sizeof(pthread_cond_t), PROT_READ | PROT_WRITE, MAP_SHARED, des_cond, 0);

    if (condition == MAP_FAILED ) {
        perror("Error on mmap on condition\n");
        exit(1);
    }

    int fd = shm_open(MESSAGE, O_RDWR, 0644);
    if (fd < 0) {
        perror("failure on shm_open on fd");
        exit(1);
    }

    if(ftruncate(fd, sizeof(int)) == -1) {
        perror("Error on ftruncate to sizeof ftruncate fd\n");
        exit(-1);
    }

    int *shint;
    shint = (int *) mmap(NULL, sizeof(int), PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);

    if (shint == MAP_FAILED ) {
        perror("Error on mmap on shint\n");
        exit(1);
    }


    // set ot 0
    *shint = 0;


    int count=0;
    while(count++<10)
    {
        pthread_mutex_lock(&dtc_status->mutex);
        *shint = 1;
        pthread_mutex_unlock(&dtc_status->mutex);

        pthread_cond_signal(&dtc_status->condition);
        printf("signal sent to consumer, %d\n",count);

        sleep(3);
    }

    // pthread_condattr_destroy(&condAttr);
    // pthread_mutexattr_destroy(&mutexAttr);
    // pthread_mutex_destroy(mutex);
    // pthread_cond_destroy(condition);

    shm_unlink(OKTOWRITE);
    shm_unlink(MESSAGE);
    shm_unlink(MUTEX);

    return 0;

}
