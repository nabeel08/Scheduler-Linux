## Concepts, Tips and Tricks 
List down all the tips, tricks and concepts while implementing this project.

#### When the mutex becomes available, does the first thread that called pthread_mutex_lock() get the lock?
By the POSIX standard. It may be random, or it may be in FIFO or LIFO or any other order, according to the choices made by the implementation.

### What if the Scheduling policy is set to FIFO?
The first thread to call pthread_mutex_lock the first to get the lock when it's available and so on for subsequent threads.Its called FIFO mutex.

### Big problem: pthreads -> thread starvation caused by quick re-locking
[Link](https://stackoverflow.com/questions/12685112/pthreads-thread-starvation-caused-by-quick-re-locking)


