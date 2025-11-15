#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>

#define MAX_LINE 80
#define MAX_SIZE 5   // store only last 5 commands

typedef struct {
    char items[MAX_SIZE][MAX_LINE]; // fixed-size slots
    int front;  // index of oldest
    int rear;   // index of next free slot
    int count;  // number of items
    int absolute_count; // for numbering the history
} Queue;

void initializeQueue(Queue* q) {
    q->front = 0;
    q->rear = 0;
    q->count = 0;
    q->absolute_count = 0;
}

int isEmpty(Queue* q) {
    return q->count == 0;
}

int isFull(Queue* q) {
    return q->count == MAX_SIZE;
}

void enqueue(Queue* q, const char* value) {
    if (isFull(q)) {
        // overwrite oldest
        q->front = (q->front + 1) % MAX_SIZE;
        q->count--;
    }
    strncpy(q->items[q->rear], value, MAX_LINE - 1);
    q->items[q->rear][MAX_LINE - 1] = '\0'; // ensure null-terminated
    q->rear = (q->rear + 1) % MAX_SIZE;
    q->count++;
    q->absolute_count++;
}

void printQueue(Queue* q) {
    if (isEmpty(q)) {
        printf("No commands in history.\n");
        return;
    }
    for (int i = q->count-1; i>=0 ; i--) {
        int index = (q->front + i) % MAX_SIZE;
        int num = q->absolute_count + 1 - (q->count - i);
        printf("%d %s\n", num, q->items[index]);
    }
}

int main(void) {
    int should_run = 1;
    Queue q;
    initializeQueue(&q);

    while (should_run) {
        printf("osh> ");
        fflush(stdout);

        char input[MAX_LINE] = "\0";
        fgets(input, sizeof(input), stdin);
        if ((strlen(input) > 0) && (input[strlen (input) - 1] == '\n')) input[strlen (input) - 1] = '\0';

        if (!strcmp(input, "exit")) // if input is exit bc strcmp returns 0 if equal
        {
            break; //same as setting should_run to 0
        } else if (!strcmp(input, "history")){ // should show history
            printQueue(&q);
        } else if (!strcmp(input, "!!")){ // should execute last command
            if (isEmpty(&q)) {
                printf("No commands in history.\n");
                continue;
    		}

    		//Find most recent command and copy it into input buffer
    		int last_index = (q.rear - 1 + MAX_SIZE) % MAX_SIZE;
    		strncpy(input, q.items[last_index], MAX_LINE - 1);
    		input[MAX_LINE - 1] = '\0'; //Null terminated

            //should execute input
            //put input into history
            enqueue(&q, input);

            //check whether we should wait
            int should_wait = 1;
            if (strlen(input) >= 1 && (input[strlen(input) - 1] == '&')) {
		        should_wait = 0;
		        input[strlen(input) - 1] = '\0';
            }

            //execute the command
            pid_t pid = fork();
            if (pid == 0) //if child process
            {
		        execlp("bash", "bash", "-c", input, (char *) NULL);
            } else if (should_wait) {
		        wait(NULL);
            }
        } else { //should execute a input
            //put input into history
            enqueue(&q, input);

            //check whether we should wait
            int should_wait = 1;
            if (strlen(input) >= 1 && (input[strlen(input) - 1] == '&')) {
                should_wait = 0;
                input[strlen(input) - 1] = '\0';
            }

            //execute the command
            pid_t pid = fork();
            if (pid == 0) //if child process
            {
                execlp("bash", "bash", "-c", input, (char *) NULL);
            } else if (should_wait) {
                wait(NULL);
            }
        }
    }
    return 0;
}
