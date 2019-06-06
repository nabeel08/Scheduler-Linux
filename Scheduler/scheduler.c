#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>


#define CPUs 1


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


// Declaring Global Varibales

char *task_file; // store input file name
queue *pt; // ptr to ready queue
long int in_pos=0; // input file current position
int rv=-1; // check for end of tasks

pthread_mutex_t q_lock = PTHREAD_MUTEX_INITIALIZER; // needed to access queue
pthread_cond_t can_produce = PTHREAD_COND_INITIALIZER; // signaled when items are removed
pthread_cond_t can_consume= PTHREAD_COND_INITIALIZER; // signaled when items are added

//Utility function to initialize queue
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

//function to check if the queue is full or not
int isFull(queue *pt)
{
   	return (size(pt) == pt->maxsize)?1:0;

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

// read two task at a time from file
int file_read(int data[]){

    FILE *in;
    in = fopen(task_file, "r");
    if(in == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fseek(in,in_pos,SEEK_SET);
    if(fscanf(in,"%d %d\n%d %d",&data[0],&data[2],&data[1],&data[3])==EOF){
        printf("\nEnd of file reached!!!!");
        fclose(in);
        return 0;
    }

    in_pos=ftell(in);
    fclose(in);
    return 1;
}

// task function to put tasks read from file into ready queue
void* task(){

    task_t data={0,0};
    int data_f[4];

    while(1){
        
        pthread_mutex_lock(&q_lock); // access critical section
        
        if(isFull(pt)){
            // wait until two elements are consumed
            printf("\nTask thread waiting on Queue Full!!!");
            pthread_cond_wait(&can_produce, &q_lock);
        }
        
        // wait again if less then 2 spots in ready queue
        if((size(pt)+2) > pt->maxsize){

            printf("\nTask thread waiting on Two Spots!!!");
            pthread_cond_wait(&can_produce, &q_lock);
        }

        rv=file_read(data_f);
        // check for eof if rv == 0
        if(rv==0){
            pthread_cond_signal(&can_consume);
            pthread_mutex_unlock(&q_lock); // mutex unlock here - need to confirm
            printf("\nNo more tasks left!!!!!");
            break;
            //goto end_task;
        }        
        printf("\ntaskid: %d taskid2: %d cpuburst1: %d cpuburst2: %d",data_f[0],data_f[1],data_f[2],data_f[3]);

        // insert in ready queue -- Limitation two task at a time insert 
        for(int i=0;i<2;i++){
        
            data.task_no=data_f[i];
            data.cpu_burst=data_f[i+2];
            enqueue(pt,data);
        }
        
        pthread_cond_signal(&can_consume);
        pthread_mutex_unlock(&q_lock);
        pthread_yield(); // added due to thread non-fairness policy

    }
    //end_task:
    printf("\nBYE BYE - Task Thread");
    // write simulation log of reading all tasks
}

// cpu executes executes task - 3 cpus
void *cpu(void *id){

    int thread_id = *(int *)id;
    printf("\nI am cpu thread %d ",thread_id);
    task_t get_task;
    
    while(1){
        
        pthread_mutex_lock(&q_lock);
        if(isEmpty(pt)){ // check queue is empty
            // wait for tasks to be appended to the buffer
            if(rv!=0){
                printf("\nCPU Thread Waiting !!!");
                pthread_cond_wait(&can_consume, &q_lock);
            }
            else{
                 pthread_mutex_unlock(&q_lock);
                 break;
            }
        }
        //check if all tasks are completed
        if(rv==0){
            if(isEmpty(pt)){
                pthread_mutex_unlock(&q_lock);
                break;
            }
        }         
        // grab the task and sleep for cpu burst time
        get_task=dequeue(pt);
        printf("\nSleeping for %d secs",get_task.cpu_burst);
        sleep(get_task.cpu_burst);
        printf("\nCPU%d: Executed Completely Task %d",thread_id,get_task.task_no);
        
        // signal the fact that new items may be produced
        pthread_cond_signal(&can_produce);
        pthread_mutex_unlock(&q_lock);
        pthread_yield(); 

    }
    printf("\nBYE BYE - CPU Thread %d",thread_id);    
}

// main function
int main(int argc, char *argv[])
{   
    int q_size;   
     /* read command line argument
      size and task file name */
    if(argc != 3){
        printf("\nPlease provide required arguments");
        fprintf(stderr,"\n Usage: ./scheduler.out task_file 4\n");
        exit(EXIT_FAILURE);
    }else
    {
        // limitation : size should be even and not more then 10
        q_size = atoi(argv[2]);
        if(q_size<=0 || q_size%2!=0 || q_size>10){
            fprintf(stderr,"\nReady Queue size should be in between 2 and 10\n");
            exit(EXIT_FAILURE);
        }
        printf("\nQueue Size : %d",q_size);
        task_file = malloc(strlen(argv[1])+1);
        strcpy(task_file,argv[1]);
        printf("\nTask File Name : %s",task_file);
    }

    pt = newQueue(q_size);
    
    /* thread varibales */
    pthread_t t_thread;
    pthread_t cpu_t[CPUs];

    int task_tid[CPUs]; // need bcs we are passing address in pthread_create
    // initialize task thread
    if((pthread_create(&t_thread, NULL, task, NULL)!=0)){
        perror("pthread_create() error");
        exit(EXIT_FAILURE);
    }
    
    // initialize cpu threads
    for(int th_id=0;th_id<CPUs;th_id++){
        task_tid[th_id]=th_id+1;
        if((pthread_create(&cpu_t[th_id], NULL, cpu,&task_tid[th_id])!=0)){
        perror("pthread_create() error");
        exit(EXIT_FAILURE);
        }
    }   

    // wait for joining thread
    pthread_join(t_thread, NULL); // will wait forever    
    for(int i = 0; i < CPUs; i++)
    {
        pthread_join(cpu_t[i], NULL);
    }
    
    // cleaning up resources
    free(pt);
    free(task_file);
	return 0;
}
