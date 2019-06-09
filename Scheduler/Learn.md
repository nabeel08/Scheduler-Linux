## Concepts, Tips and Tricks 
List down all the tips, tricks and concepts while implementing this project.

#### When the mutex becomes available, does the first thread that called pthread_mutex_lock() get the lock?
By the POSIX standard. It may be random, or it may be in FIFO or LIFO or any other order, according to the choices made by the implementation.

#### What if the Scheduling policy is set to FIFO?
The first thread to call pthread_mutex_lock the first to get the lock when it's available and so on for subsequent threads.Its called FIFO mutex.

#### Big problem: pthreads -> thread starvation caused by quick re-locking
[Link](https://stackoverflow.com/questions/12685112/pthreads-thread-starvation-caused-by-quick-re-locking)
We can not rely in any way on which waiting thread will get a mutex, or even that a waiting thread will be woken up before another thread that wasn't waiting for the mutex happens to request and get the lock.
In fact, the OS scheduler might even be more likely to allow an already-running thread to continue to run and obtain the lock.

**pthread_yield**
[Link](https://linux.die.net/man/3/pthread_yield)

#### Thread Scheduling
[Link](https://www.ibm.com/support/knowledgecenter/en/ssw_aix_71/com.ibm.aix.genprogc/threads_sched.htm)

#### Purpose of pthread_cond_wait() and pthread_cond_signal()
The function pthread_cond_wait will unlock the held mutex while the thread is waiting. This allows another thread to enter the critical section.
The purpose of using pthread_cond_wait is that the thread needs to wait for some condition to become true before really performing the work inside the critical section. To test the condition in the first place requires locking the mutex. But, if the condition is false, it must wait for some event to make the condition true. If it waits with the lock held, no other thread will be able to update the state for the condition to become true, since updating the condition will also require locking the same mutex.

