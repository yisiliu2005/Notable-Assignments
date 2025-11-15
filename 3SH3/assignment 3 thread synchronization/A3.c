// compiling: gcc A3.c -o A3
// running: ./A3
// please terminate the program with a keyboard interrupt when you're done

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N 5 //number of students
pthread_mutex_t mutex;
sem_t waiting_students;
pthread_mutex_t office;
sem_t help_time_sem;
int helping_time;

/* Description of our representation:
 *
 * TA:
 * The TA is either sleeping or helping a student.
 * Providing help to a student is represented by both threads
 * calling sleep() for a random number of seconds.
 * Sleeping is represented by waiting for students.
 *
 * Students:
 * Students are either programming, waiting, or getting helped.
 * Getting helped by the TA is represented by both threads
 * calling sleep() for a random number of seconds.
 * Waiting is represented by calling wait() on the mutex office
 * and calling post() on the semaphore waiting_students
 * Programming is represented by calling sleep() for a random
 * number of seconds.
 *
 *
 * In this structure, students are sitting in the chairs outside the
 * TA's office if they're a waiting student outside TA's office
 * (i.e. wait(office) and post(waiting_students))
 *
 * Entering the TA's room to be helped is making it past the line
 * wait(office) in the code (because the TA signaled it).
 *
 */

void* TA(void* arg){
    int ta_sleeping = 1; // to check if the ta was sleeping (It is initialized with 1 because at first, nobody was waiting in the hallway.)
    do {
        sem_wait(&waiting_students); //TA is sleeping. No students are waiting in the hallway OR TA takes one student from the waiting line, so there is one less person on the waiting chairs.

        if (ta_sleeping){
            printf("TA: A student wakes the sleeping TA up. \n");
            ta_sleeping = 0; //TA is now awake
        }else{
            printf("TA: Taking next student\n"); //There are other students waiting in the hallway
        }
        pthread_mutex_unlock(&office); // invite exactly one waiting student

        // Giving help:
        sem_wait(&help_time_sem); //TA waits for student to decide how long they need help for
        printf("TA: Giving help for %i seconds\n", helping_time);
        sleep(helping_time);

        //to check if that was the last student in the queue
        int val;
        sem_getvalue(&waiting_students, &val);
        if (val == 0){
            ta_sleeping = 1; //TA starts sleeping. No waiting people in the hallway. Chairs are empty.
        }
    } while (1);
    return NULL;
}

void* student(void* arg) {  // the student cycles through programming, waiting, getting help, and going back to programming
    do {
        // Programming:
        // Generate a random number between 1 and 10
        int sleeping_time = (rand() % 10) + 1;
        printf("Programming...\n");
        sleep(sleeping_time);

        // Waiting:
        printf("Gonna go ask for help\n");
        pthread_mutex_lock(&mutex); //only one thread checks the value of the number of waiting students at a time so that only 3 students at most wait outside the TA's office
        int num = 0;
        sem_getvalue(&waiting_students, &num);
        if (num >= 3) {
            pthread_mutex_unlock(&mutex);
            printf("Nevermind, I'll come back later\n");
            continue; // go back to programming
        } // if less than 3 students waiting, wait.
        sem_post(&waiting_students); //let TA know students are waiting
        pthread_mutex_unlock(&mutex); //modifying sem value within the mutex
        printf("I'm outside the TA's office\n");
        pthread_mutex_lock(&office); //wait outside the office, only one student gets in at a time

        // Getting help:
        helping_time = (rand() % 10) + 1;
        sem_post(&help_time_sem); //student decides help time and TA accesses global variable after random number has been generated
        printf("Getting help for %i seconds\n", helping_time);
        sleep(helping_time);

    } while (1);
    return NULL;
}


int main() {
    // Seeding random number generator
    srand(time(NULL));

    //initializing synchronization tools
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        fprintf(stderr, "Mutex mutex initialization failed\n");
        return 1;
    }

    if (sem_init(&waiting_students, 0, 0) != 0) { //
        fprintf(stderr, "Error in initializing full semaphore \n");
        return 1;
    }

    if (pthread_mutex_init(&office, NULL) != 0) {
        fprintf(stderr, "Mutex office initialization failed\n");
        return 1;
    }

    if (sem_init(&help_time_sem, 0, 0) != 0) {
        fprintf(stderr, "Error in initializing helping time semaphore\n");
        return 1;
    }

    //creating threads
    pthread_t students[N];
    pthread_t theTA;

    for (int i = 0; i < N; i++) {
        if (pthread_create(&students[i], NULL, student, NULL) != 0) {
            fprintf(stderr, "Student thread creation failed\n");
        }
    }

    if (pthread_create(&theTA, NULL, TA, NULL) != 0) {
        fprintf(stderr, "TA thread creation failed\n");
    }

    //call pthread_join
    for (int i = 0; i < N; i++) pthread_join(students[i], NULL);
    pthread_join(theTA, NULL);

    return 0;
}
 
