#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <sys/mman.h>
#include <stdarg.h>

#define OK_CODE 0
#define ERR_CODE 1
#define POCET_ARG 5
#define OUTPUT_FILE "proj2.out"

#define MAX_SKIERS 20000
#define MAX_BUS_STOPS 10
#define MAX_BUS_CAPACITY 100
#define MAX_SKIER_WAIT 10000
#define MAX_BUS_DELAY 1000

typedef struct
{
    int stop_count;
    int counter;
    int in_bus;
    int skiing;
    int *at_stop;
    sem_t bus;
    sem_t *stop_sem;
    sem_t action_mutex;
    sem_t bus_finished;
    sem_t bus_can_leave;
    FILE *log_file;
} SharedData;

SharedData *data;

int init_resources(int Z)
{
    data = (SharedData *)mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    data->at_stop = (int *)mmap(NULL, sizeof(int) * Z, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    data->stop_sem = (sem_t *)mmap(NULL, sizeof(sem_t) * Z, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    data->stop_count = Z;
    data->counter = 0;
    data->skiing = 0;
    data->in_bus = 0;

    if (data == MAP_FAILED ||
        data->at_stop == MAP_FAILED ||
        data->stop_sem == MAP_FAILED)
    {
        fprintf(stderr, "Error creating shared mem\n");
        return ERR_CODE;
    }

    sem_init(&data->bus, 1, 0); // post first
    for (int i = 0; i < Z; i++)
    {
        sem_init(&data->stop_sem[i], 1, 0); // post first
    }
    sem_init(&data->action_mutex, 1, 1);
    sem_init(&data->bus_finished, 1, 0);  // post first
    sem_init(&data->bus_can_leave, 1, 0); // post first
    data->log_file = fopen(OUTPUT_FILE, "w");
    if (data->log_file == NULL)
    {
        fprintf(stderr, "Error opening log file for write\n");
        return ERR_CODE;
    }
    setbuf(data->log_file, NULL);
    return OK_CODE;
}
void sem_destroy_all()
{
    sem_destroy(&data->bus);
    for (int i = 0; i < data->stop_count; i++)
    {
        sem_destroy(&data->stop_sem[i]);
    }
    sem_destroy(&data->action_mutex);
    sem_destroy(&data->bus_finished);
    sem_destroy(&data->bus_can_leave);
}

void free_all()
{
    sem_destroy_all();

    fclose(data->log_file);
    munmap(data->at_stop, sizeof(int) * data->stop_count);
    munmap(data->stop_sem, sizeof(sem_t) * data->stop_count);
    munmap(data, sizeof(SharedData));
}

void log_action(const char *format, ...)
{

    sem_wait(&data->action_mutex);

    va_list args;
    va_start(args, format);

    char new_format[256];
    sprintf(new_format, "%d: %s\n", ++data->counter, format);

    vfprintf(data->log_file, new_format, args);
    fflush(data->log_file);

    va_end(args);

    sem_post(&data->action_mutex);
}

void inc_stop(int stop_id)
{
    sem_wait(&data->action_mutex);
    data->at_stop[stop_id]++;
    sem_post(&data->action_mutex);
}

void dec_stop(int stop_id)
{
    sem_wait(&data->action_mutex);
    data->at_stop[stop_id]--;
    sem_post(&data->action_mutex);
}

int get_random_from_range(int min, int max)
{
    srand(time(NULL) + getpid()); // set random seed
    int t = rand() % (max + 1 - min) + min;
    return t;
}

void skier(int id, int Z, int TL)
{
    log_action("L %d: started", id);
    usleep(get_random_from_range(0, TL)); // chalky

    int stop_id = get_random_from_range(0, Z - 1);
    int stop_no = stop_id + 1;

    log_action("L %d: arrived to %d", id, stop_no);
    inc_stop(stop_id); // blud starts waiting for da bus
    // wait till bus arrivez
    sem_wait(&data->stop_sem[stop_id]);
    // bus here
    log_action("L %d: boarding", id);
    dec_stop(stop_id);
    data->in_bus++;
    sem_post(&data->bus); // boarded

    // wait till bus reach final
    sem_wait(&data->bus_finished);
    log_action("L %d: going to ski", id);
    // he is done
    data->in_bus--;
    data->skiing++;
    sem_post(&data->bus_can_leave);

    exit(0);
}

void bus(int Z, int K, int TB, int L)
{
    log_action("BUS: started");
    for (int i = 0; i < Z; i++)
    {
        int stop_no = i + 1;
        // slow aaaaah bus
        usleep(get_random_from_range(0, TB));

        log_action("BUS: arrived to %d", stop_no);

        sem_wait(&data->action_mutex);
        int on_stop = data->at_stop[i];
        sem_post(&data->action_mutex);

        int ppl_to_board = (on_stop > K) ? K : on_stop;
        // lets not overfill the bus
        if (data->in_bus + ppl_to_board > K)
        {
            ppl_to_board = K - data->in_bus;
        }
        // let em board
        for (int bro = 0; bro < ppl_to_board; bro++)
        {
            sem_post(&data->stop_sem[i]); // tell bro to board

            sem_wait(&data->bus); // wait till broski boards
        }

        log_action("BUS: leaving %d", stop_no);
        // another stop or nah?
        if (stop_no < Z)
            continue;
        // slow bus frfr
        usleep(get_random_from_range(0, TB));
        log_action("BUS: arrived to final");
        // tell em to f off
        int ppl_to_leave = data->in_bus;

        for (int xd = 0; xd < ppl_to_leave; xd++)
        {
            sem_post(&data->bus_finished);
            sem_wait(&data->bus_can_leave);
        }
        log_action("BUS: leaving final");

        if (data->skiing != L)
        {
            // ah shii, here we go again
            i = -1;
            continue;
        }
    }

    log_action("BUS: finish");
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != POCET_ARG + 1) // +1 name
    {
        fprintf(stderr, "Error: Invalid arguments\n");
        fprintf(stderr, "Run like this: $ %s L Z K TL TB\n", argv[0]);
        return ERR_CODE;
    }
    int L = strtol(argv[1], NULL, 10);
    int Z = strtol(argv[2], NULL, 10);
    int K = strtol(argv[3], NULL, 10);
    int TL = strtol(argv[4], NULL, 10);
    int TB = strtol(argv[5], NULL, 10);

    if (
        L < 0 || L > MAX_SKIERS ||        // L<20000
        Z <= 0 || Z > MAX_BUS_STOPS ||    // 0<Z<=10
        K < 10 || K > MAX_BUS_CAPACITY || // 10<=K<=100
        TL < 0 || TL > MAX_SKIER_WAIT ||  // 0<=TL<=10000
        TB < 0 || TB > MAX_BUS_DELAY      // 0<=TB<=1000
    )
    {
        fprintf(stderr, "Error: Argument values out of range\n");
        return ERR_CODE;
    }

    if (init_resources(Z) != OK_CODE)
        return ERR_CODE;

    pid_t pid = fork(); // bus
    if (pid < 0)
    {
        fprintf(stderr, "Error: skibus fork failed\n");
        free_all();
        return ERR_CODE;
    }
    if (pid == 0)
    {
        bus(Z, K, TB, L);
    }
    // skiers
    for (int i = 0; i < L; i++)
    {
        pid = fork();
        if (pid < 0)
        {
            fprintf(stderr, "Error: %d skier fork failed\n", i + 1);
            free_all();
            return ERR_CODE;
        }
        if (pid == 0)
        {
            skier(i + 1, Z, TL);
        }
    }

    // wait till all skiers and bus are done
    for (int i = 0; i < L + 1; i++)
    {
        wait(NULL);
    }
    free_all();
    return OK_CODE;
}
