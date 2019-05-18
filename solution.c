/*
 *
    Solution for Dining Philosopher Problem
 * 
 */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
 
#define N 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define LEFT (pNum + N - 1) % N
#define RIGHT (pNum + 1) % N
 
int state[N];
// In order to change # of philosopher, modify the array manually.
int philosophers[N] = { 0, 1, 2, 3, 4 };

// Mutex is used such that no two philosophers may access the pickup or 
//  putdown at the same time. 
// The array is used to control the behavior of each philosopher. 
sem_t mutex;
sem_t S[N];

//  FUNCTION:   Eat noodle, if philosopher is able to.
//  Input:      philosopher ID 
//  Output:     change state of philosopher 
void eat_noodle(int pNum)
{
    if (state[pNum] == HUNGRY
        && state[LEFT] != EATING
        && state[RIGHT] != EATING) {
        // State that philosopher is eating.
        state[pNum] = EATING;
 
        sleep(1);
 
        printf("Philosopher # %d TAKING fork %d and %d\n",
                                    pNum, LEFT, pNum);
        printf("Philosopher # %d is EATING\n", pNum);
 
        // sem_post() is used to wake up semaphores.
        // That's why, it executes only while philosopher putting
        //  fork down.
        // So means, it has no effect during taking fork, because
        //  the semaphore is already woke.
        sem_post(&S[pNum]);
    }
}
 
//  FUNCTION:   Take the chopsticks, if philosopher will eat.
//  Input:      philosopher ID 
//  Output:     change state of philosopher 
void take_fork(int pNum)
{
 
    // It will wait to be signalled in order not to another 
    //  philosopher access the process.
    sem_wait(&mutex);
 
    // State that philosopher is hungry.
    state[pNum] = HUNGRY;
    printf("Philosopher # %d is HUNGRY\n", pNum);
 
    // Philosopher will eat, if neighbours are not eating.
    eat_noodle(pNum);
    sem_post(&mutex);
 
    // If philosopher is unable to eat, it will wait to be signalled.
    sem_wait(&S[pNum]);
 
    sleep(1);
}
 
//  FUNCTION:   Put the chopsticks, if philosopher is done for eating.
//  Input:      philosopher ID 
//  Output:     change state of philosopher 
void put_fork(int pNum)
{
 
    // It will wait to be signalled in order not to another 
    //  philosopher access the process.
    sem_wait(&mutex);
 
    // State that philosopher is thinking.
    state[pNum] = THINKING;
    printf("Philosopher # %d PUTTING fork %d and %d down\n",
           pNum, LEFT, pNum);
    printf("Philosopher # %d is THINKING\n", pNum);
 
    // Check if neighbors of the philosopher can eat or not.
    eat_noodle(LEFT);
    eat_noodle(RIGHT);
 
    // It will release the philosopher.
    sem_post(&mutex);
}
 
//  FUNCTION:   The life cycle of philosopher
//  Input:      philosopher ID 
//  Output:     None
//  Note:       process P[i]
//                 while true do
//                 {    THINK;
//                      PICKUP CHOPSTICKS;
//                      EAT;
//                      PUTDOWN CHOPSTICKS;
//                 }
void* philosopher(void* num)
{ 
    while (1) {
        int* i = num;
        sleep(1);
        take_fork(*i);
        sleep(1);
        put_fork(*i);
    }
}

//  FUNCTION:   main()
int main()
{
    pthread_t threads[N];
 
    // Initialize the semaphores
    sem_init(&mutex, 0, 1);
    for (int i = 0; i < N; i++)
        sem_init(&S[i], 0, 0);
 
    // Create philosopher processes
    for (int i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL,
                       philosopher, &philosophers[i]);
        printf("Philosopher # %d is THINKING\n", i);
    }
    
    // Allows philosopher to wait for the ending.
    for (int i = 0; i < N; i++)
        pthread_join(threads[i], NULL);
}