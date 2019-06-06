#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Data Structure for task 
typedef struct task_info
{
  int task_no;
  int cpu_burst;
  char *arrival_time;
}task_t;

// Data structure for queue
typedef struct ready_queue
{
	task_t *task;   // array to store queue elements(queue elements = task)
	int maxsize;	// maximum capacity of the queue
	int front;		// front points to front element in the queue (if any)
	int rear;		// rear points to last element in the queue
	int size;		// current capacity of the queue
}queue;

// Utility function to initialize queue
queue* newQueue(int size)
{
	queue *pt = NULL;
	pt = (queue*)malloc(sizeof(queue));
	pt->task = (task_t*)malloc(size * sizeof(task_t));
	pt->maxsize = size;
	pt->front = 0;
	pt->rear = -1;
	pt->size = 0;

	return pt;
}

//function to return the size of the queue
int size(queue *pt){
  	return pt->size;
}

//function to check if the queue is empty or not
int isEmpty(queue *pt)
{
	return !size(pt);
}

void insert_task(queue *pt, task_t task){

// get arrival time
    time_t s;
    struct tm* current_time;
    time(&s); // To set the time object
    current_time = localtime(&s); // To get time information
    
    pt->task[pt->rear].task_no = task.task_no;
    pt->task[pt->rear].cpu_burst = task.cpu_burst;
    pt->task[pt->rear].arrival_time=(char *)malloc(10*sizeof(char));
    sprintf(pt->task[pt->rear].arrival_time, "%d:%d:%d",current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
    printf("\nInserted task -> task no: %d arrival time: %s",pt->task[pt->rear].task_no,pt->task[pt->rear].arrival_time);
}

// Utility function to add an element x in the queue
void enqueue(queue *pt, task_t task)
{
	if (size(pt) == pt->maxsize)
	{
		printf("OverFlow\nProgram Terminated\n");
		exit(EXIT_FAILURE);
	}
	pt->rear = (pt->rear + 1) % pt->maxsize;	// circular queue
    insert_task(pt,task);
	pt->size++;

	printf("\nenqueue: front = %d, rear = %d", pt->front, pt->rear);
}

// Utility function to remove element from the queue
task_t dequeue(queue *pt)
{
	if (isEmpty(pt)) // front == rear
	{
		printf("UnderFlow\nProgram Terminated\n");
		exit(EXIT_FAILURE);
	}
    task_t obj;
    obj.task_no = pt->task[pt->front].task_no;
    obj.cpu_burst = pt->task[pt->front].cpu_burst;
    obj.arrival_time = pt->task[pt->front].arrival_time;

	pt->front = (pt->front + 1) % pt->maxsize;	// circular queue
	pt->size--;

    printf("\ndequeue: front = %d, rear = %d", pt->front, pt->rear);

    return obj;
}

// main function
int main()
{   
    // ready queuue of size 5
	queue *pt = newQueue(5);
    task_t obj = {1,5};
    task_t obj2;
    enqueue(pt, obj);
    obj.arrival_time=(char *)malloc(sizeof(char));
    obj2=dequeue(pt);

    printf("\ntask no : %d , cpu burst : %d , arrival tme : %s",obj2.task_no,obj2.cpu_burst,obj2.arrival_time);
	printf("\nsize = %d", size(pt));

	if (isEmpty(pt))
		printf("\nQueue is empty\n");
	else
		printf("\nQueue is not empty");

	return 0;
}
