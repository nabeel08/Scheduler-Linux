# Scheduler Task
Write a program scheduler in C under Linux environment to simulate the operations of three Processor Scheduling Simulation. 

The programs for this scheduler should include the following features.
1) There is one Ready-Queue that contains a list of tasks to be serviced by three CPUs: CPU-1, CPU-2, and CPU-3. 
The Ready-Queue has a capacity for m tasks, and is initially empty.

2) All CPUs are waiting for the arrival of the tasks in Ready-Queue. The arrival of a task should interrupt any of the waiting CPUs, which will grab the task, and execute it.

3) Your scheduler includes a list of tasks, stored in file task_file. A task in the file is represented by
task# cpu_burst
The task# is a positive integer, and the cpu_burst is another positive integer (in second). Note that, each task may have different cpu_burst. Create yourself a task_file that contains 100 tasks with random cpu_burst (1 to 50).
