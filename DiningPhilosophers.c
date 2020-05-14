#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define eating 0
#define thinking 1
void *thinkingstate(void *param); //Function threads go to when first created
void pickupforks (int philID); //Must pickup both forks to start eating
void returnforks (int philID); //Return forks when done eating
int state[5]; //State of each philosopher (eating/thinking)
long i;
pthread_attr_t attr;
pthread_mutex_t mutex[5];
pthread_cond_t cond_var[5];

int main ()
{
    time_t t;
    srand((unsigned) time(&t));
    pthread_t philosophers[5];
    pthread_attr_init(&attr);
    
    for (i = 0; i < 5; i++)
        pthread_mutex_init(&mutex[i], NULL); //Mutex lock for each fork
    
    for(i = 0; i < 5; i++)
        pthread_cond_init(&cond_var[i], NULL); //Conditional variable for each fork
    
    for (i = 0; i < 5; i++)
        pthread_create(&philosophers[i], NULL, thinkingstate, (void *)i);
    
    for (i = 0; i < 5; i++)
        pthread_join(philosophers[i], NULL);
    
    return 0;
}

void *thinkingstate (void *param)
{
    int philID;
    philID = (int)param;
    state[philID] = thinking;
    printf("Philosopher %d is created.\n", philID); //Philosophers initially begin thinking
    
    sleep(rand()%4);     //To simulate thinking
    pickupforks(philID); //Begin trying to eat here
    returnforks(philID); //Begin this function when they are done eating
    
    return(NULL);
}

void pickupforks (int philID)
{
    printf("Philosopher %d is thinking about fork number %d. (Right fork)\n", philID, philID);
    pthread_mutex_lock(&mutex[philID]); //Lock right fork
    
    while (state[philID] == eating) //Wait on fork if adjacent philosopher is eating
    {
        pthread_cond_wait(&mutex[philID], &cond_var[philID]);
    }
    printf("Philosopher %d has the right fork.\n", philID);

    printf("Philosopher %d is thinking about fork number %d. (Left fork)\n", philID, (philID+1)%5);
    pthread_mutex_lock(&mutex[(philID+1)%5]); //Lock left fork
    
    while (state[(philID+1)%5] == eating) //Wait on fork if adjacent philosopher is eating
    {
        pthread_cond_wait(&mutex[(philID+1)%5], &cond_var[(philID+1)%5]);
    }
    printf("Philosopher %d has the left fork.\n", philID);
    
    state[philID] = eating; //Once both forks are aquired, the philosopher can eat
    printf("Philosopher %d is now eating.\n", philID);
    sleep(rand() % 4); //To simulate eating
}

void returnforks (int philID)
{
    printf("Philosopher %d is done eating.\n", philID);
    state[philID] = thinking;
    pthread_cond_signal(&cond_var[philID]);
    pthread_mutex_unlock(&mutex[philID]);  //Release right fork
    pthread_cond_signal(&cond_var[(philID+1)%5]);
    pthread_mutex_unlock(&mutex[(philID+1)%5]); //Release left fork
    sleep(rand() % 4); //To simulate thinking
}
