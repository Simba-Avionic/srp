#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syscall.h>
#include <unistd.h>

pthread_cond_t* condition;
pthread_mutex_t* mutex;

#define OKTOWRITE "/oktowrite"
#define MESSAGE "/message"
#define MUTEX "/lock"

struct shared_use_st {
  bool conditionSatisfied;
};
struct dtc_status_t {
  pthread_cond_t condition;
  pthread_mutex_t mutex;
};

struct shared_use_st* shared_stuff;
struct dtc_status_t* dtc_status;
std::co
int main() {
  int des_cond, des_msg, des_mutex;
  int mode = S_IRWXU | S_IRWXG;

  // Unlink first to clean
  shm_unlink(MUTEX);
  shm_unlink(OKTOWRITE);
  shm_unlink(MESSAGE);

  pthread_mutexattr_t mutexAttr;
  pthread_mutexattr_init(&mutexAttr);
  pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);

  pthread_condattr_t condAttr;
  pthread_condattr_init(&condAttr);
  pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_SHARED);

  des_mutex = shm_open(MUTEX, O_CREAT | O_RDWR, mode);

  if (des_mutex < 0) {
    perror("failure on shm_open on des_mutex");
    exit(1);
  }

  if (ftruncate(des_mutex, sizeof(dtc_status_t)) == -1) {
    perror("Error on ftruncate to sizeof pthread_cond_t\n");
    exit(-1);
  }

  dtc_status =
      (dtc_status_t*)mmap(NULL, sizeof(dtc_status),
                             PROT_READ | PROT_WRITE, MAP_SHARED, des_mutex, 0);

  if (dtc_status == MAP_FAILED) {
    perror("Error on mmap on mutex\n");
    exit(1);
  }

  des_cond = shm_open(OKTOWRITE, O_CREAT | O_RDWR, mode);

  if (des_cond < 0) {
    perror("failure on shm_open on des_cond");
    exit(1);
  }

  if (ftruncate(des_cond, sizeof(pthread_cond_t)) == -1) {
    perror("Error on ftruncate to sizeof pthread_cond_t\n");
    exit(-1);
  }

  condition =
      (pthread_cond_t*)mmap(NULL, sizeof(pthread_cond_t),
                            PROT_READ | PROT_WRITE, MAP_SHARED, des_cond, 0);

  if (condition == MAP_FAILED) {
    perror("Error on mmap on condition\n");
    exit(1);
  }

  int fd = shm_open(MESSAGE, O_CREAT | O_RDWR, 0644);
  if (fd < 0) {
    perror("failure on shm_open on fd");
    exit(1);
  }

  if (ftruncate(fd, 16) == -1) {
    perror("Error on ftruncate to sizeof ftruncate fd\n");
    exit(-1);
  }
  int* shint;
  shint =
      (int*)mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);

  if (shint == MAP_FAILED) {
    perror("Error on mmap on shint\n");
    exit(1);
  }

  *shint = 0;

  if (pthread_mutex_init(&dtc_status->mutex, &mutexAttr) != 0) {
    printf("Error initi mutex");
    exit(111);
  }
  if (pthread_cond_init(&dtc_status->condition, &condAttr) != 0) {
    printf("Error initi cond");
    exit(111);
  }

  while (1) {
    printf("Receiver waits on for signal from hello1.c \n");

    pthread_mutex_lock(&dtc_status->mutex);
    while (*shint == 0) pthread_cond_wait(&dtc_status->condition, &dtc_status->mutex);
    printf("Waiting");
    sleep(1);
    pthread_mutex_unlock(&dtc_status->mutex);

    printf("Signal received, wake up!!!!!!!!\n");
    *shint = 0;
    // reset
    //  pthread_mutex_lock(mutex);
    //  shared_stuff->conditionSatisfied=0;
    //  pthread_mutex_unlock(mutex);
  }

  return 0;
}