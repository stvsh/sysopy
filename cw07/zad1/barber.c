#include "utilities.h"

Barbershop  *bs;
int         shm_id;
int         sem_id;

void init(int);
void remove_ipcs();
void sig_handle();

int
main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("Usage: ./barber <chairs_count>\n"); exit(1);
  }

  int chairs_count;
  
  sscanf(argv[1], "%d", &chairs_count);

  init(chairs_count);

  give_semaphore(sem_id);

  while (1)
  {
    take_semaphore(sem_id);

    switch(bs->barber_state)
    {
      case FREE:
        if (queue_empty(bs))
        {
          printf("%ld: barber fell asleep\n", get_time());
          bs->barber_state = SLEEPING;
        } else {
          printf("%ld: client %d invited\n", get_time(), bs->clients_queue[bs->head]);
          bs->current_client = bs->clients_queue[bs->head];
          bs->barber_state = READY;
        }
        break;

      case AWAKE:
        printf("%ld: barber woke up!\n", get_time());
        bs->barber_state = READY;
        break;

      case BUSY:
        printf("%ld: started cutting client %d\n", get_time(), bs->current_client);
        printf("%ld: finished cutting client %d\n", get_time(), bs->current_client);
        bs->current_client = 0;

        bs->barber_state = READY;
        break;

      default:
        break;
    }
    give_semaphore(sem_id);
  }

}

void
init(int chairs_count)
{
  atexit(&remove_ipcs);
  signal(SIGINT, sig_handle);
  signal(SIGTERM, sig_handle);

  key_t barber_key;

  if ((barber_key = ftok(BARBER_PATH, BARBER_ID)) == (key_t) -1)
  {
    perror("IPC error: ftok"); exit(1);
  }

  if ((shm_id = shmget(barber_key, sizeof(Barbershop), 0666 | IPC_CREAT)) == -1)
  {
    perror("IPC error: shmget"); exit(1);
  }

  if ((bs = shmat(shm_id, NULL, 0)) == (void *) -1)
  {
    perror("IPC error: shmat"); exit(1);
  }

  if ((sem_id = semget(barber_key, 1, 0666 | IPC_CREAT)) == -1)
  {
    perror("IPC error: semget"); exit(1);
  }

  if (semctl(sem_id, 0, SETVAL, 0) == -1)
  {
    perror("IPC error: semctl (SETVAL)"); exit(1);
  }

  bs->barber_state = SLEEPING;
  bs->head = -1;
  bs->tail = -1;
  bs->chairs_count = chairs_count;
  bs->current_client = 0;
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

void
sig_handle()
{
  remove_ipcs();
  exit(0);
}