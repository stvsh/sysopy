#include "utilities.h"

Barbershop  *bs;
ClientState client_state;
int         shm_id;
int         sem_id;

void        init();
void        run_clients(int);
void        take_seat();

int
main(int argc, char *argv[])
{
  if (argc != 3) {
    printf("Usage: ./clients <clients_count> <haircuts_count>"); exit(1);
  }

  int clients_count;
  int haircuts_count;

  sscanf(argv[1], "%d", &clients_count);
  sscanf(argv[2], "%d", &haircuts_count);

  init();

  for (int i = 0; i < clients_count; ++i)
  {
    if (!(fork()))
    {
      run_clients(haircuts_count);
      exit(0);
    }
  }

  while (wait(NULL) > 0) { };
}

void
init()
{
  key_t barber_key;

  if ((barber_key = ftok(BARBER_PATH, BARBER_ID)) == (key_t) -1)
  {
    perror("IPC error: ftok"); exit(1);
  }

  if ((shm_id = shmget(barber_key, sizeof(Barbershop), 0)) == -1)
  {
    perror("IPC error: shmget"); exit(1);
  }

  if ((bs = shmat(shm_id, NULL, 0)) == (void *) -1)
  {
    perror("IPC error: shmat"); exit(1);
  }

  if ((sem_id = semget(barber_key, 0, 0)) == -1)
  {
    perror("IPC error: semget"); exit(1);
  }
}

void
run_clients(int haircuts_count)
{
  int   haircuts = 0;
  pid_t pid = getpid();

  while (haircuts < haircuts_count)
  {
    client_state = NEW;
    
    lock_semaphore(sem_id);

    if (bs->barber_state == SLEEPING)
    {
      printf("%ld: barber woke up by client %d\n", get_time(), pid);
      bs->barber_state = AWAKE;
      take_seat();
      bs->barber_state = BUSY;
    }
    else if (queue_full(bs))
    {
      printf("%ld: client %d could not eneter the queue\n", get_time(), pid);
      unlock_semaphore(sem_id);
      return;
    }
    else
    {
      enqueue(bs, pid);
      printf("%ld: client %d enetered the queue\n", get_time(), pid);
    }

    unlock_semaphore(sem_id);

    while (client_state != INVITED)
    {
      lock_semaphore(sem_id);
      if (bs->current_client == pid)
      {
        client_state = INVITED;
        take_seat();
        bs->barber_state = BUSY;
      }
      unlock_semaphore(sem_id);
    }

    while (client_state != DONE)
    {
      lock_semaphore(sem_id);
      if (bs->current_client != pid)
      {
        client_state = DONE;
        printf("%ld: client %i shaved\n", get_time(), pid);
        bs->barber_state = FREE;
        ++haircuts;
      }
      unlock_semaphore(sem_id);
    }
  }
  printf("%ld: client %d left barbershop after %d cuts done\n", get_time(), pid, haircuts);
}

void
take_seat()
{
  pid_t pid = getpid();

  if (client_state == INVITED)
  {
    dequeue(bs);
  }
  else if (client_state == NEW)
  {
    while (1)
    {
      unlock_semaphore(sem_id);
      lock_semaphore(sem_id);
      if (bs->barber_state == READY)
      {
        break;
      }
    }
    client_state = INVITED;
  }

  bs->current_client = pid;  
  printf("%ld: client %d took a seat\n", get_time(), pid);
}

void
remove_ipcs()
{
  if (sem_id != 0)
  {
    semctl(sem_id, 0, IPC_RMID);
  }

  if (shm_id != 0)
  {
    shmdt(bs);
    shmctl(shm_id, IPC_RMID, NULL);
  }
}