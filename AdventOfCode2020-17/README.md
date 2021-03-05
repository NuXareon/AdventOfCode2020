# Day 17: Conway Cubes

Decided to experiment a bit further with multithreading with this problem.

I implemented four solutions for this problem, one single threaded an 3 multithreaded ones, and compared their efficiency.

First implementations was the single threaded one. Not to complex once you get your head around the problem. Just iterated through all the cubes and apply the algorithm.

The first attempt to multithread this ones to just fire a thread for each plane, and have each thread update the grid directly. This has the advantadge that we don't need any synchronization between the threads, since each thread processes a different part of out grid. On the downside, creating a high number of threads generates a very big overhead.

Second attempt was to create a list of all the plane coordinates, and have a fixed number of threads process all of them. We use a mutex to protect the access to this coordinate list.

I then implemented a version using an atomic index that represente a plane coordinate. Thread use that to know which plane they need to process next.

Finally, I tried a implementation with no synch required, where each thread takes a set amount of elements to process which are defined when we creat the thread (first thread the first n elements, etc...).

Here are some stats:

> Using input2.txt (just a much bigger grid), we get this results with 6 iterations:
* Single Thread: 28703 ms
* Thread per plane: 7909 ms
* Mutex synch: 7999 ms
* Atomic synch: 7512 ms

> If we increase the number of iterations to 10:
* Thread per plane: 32822 ms
* Mutex synch: 30756 ms
* Atomic synch: 30482 ms
* No Synch: 29895

Here we can see how each method is slightly better than the previous one, which makes sense since the less amunt of synch we need between threads the less overhead we are putting on the cpu/memory. The changes are quite minor though, so they are probably not very relevant at this level (threads are probably not clashing offten enough for this to make a big difference).

> As and extra, this are the times when running the original problem:
* Single thread: 37
* Thread per plane: 408
* Mutex synch: 58
* Atomic synch: 56

In this case we can see the absurd amount of overhead caused by creating a large amount of threads, but also how any multithread overhead is making the program run slower than it's single threaded version.
