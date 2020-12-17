# Day 11: Seating System

Decided to try to multithread this one. It wasn't really necessary since the map is not that big that it requires multithreading to be fast enough to find a solution, but it has a nice structure to be multithreaded and felt like it would be a fun exercise.

Basically, every thread is responsible to check a few rows. Once all threads have finished we compare the result with our previous map and check if they match. If they do we resume the main thread and print the solution, otherwise we let the threads make another iterations to new map.

To achieve this I used a couple of nice c++20 new library features: std::latch and std::barrier. Here is how we do it:

We begin the program be reading the input and storing the map in a std::vector<string>. We then calculate how many rows each thread is going to take care of (in this case I wanted 8 threads). We then proceed to spawn all the threads.

After this we will put the main thread to sleep using our std::latch. A latch is a mechanism that will sleep a thread until its internal counter reaches zero. In this case the main thread will be blocked at the latch until we find the correct answer and decrement the latch to 0 (more on this later).

Our threads logic is pretty simple: process a line and store it on the new map, rinse and repeat for all the lines each thread needs to care about.  Since we are storing the new map in a separate variable (we need to compare the previous and the new map), we don't have to worry bout other threads modifying the map, so we don't need a read lock/mutex for reading the data. Also, since all the threads write on different rows we also don't need to have a write lock, since there will not be any overlap.

You may notice that each thread is put on a semi infinite loop, since we only stop when the latch counter reaches zero. Here comes the secod part of our synch: the std::barrier. A barrier will block a thread and decrement it's internal counter. Once the counter reaches zero we release all the threads this barrier was holding and reset the counter. So it does a pretty good job on keeping our thread syncronized each iteration of the algorithm. 

But there is another nice thing we can do with an std::barrier - We can invoke a callback everytime the callback counter reaches zero, just before releasing all the threads. This means that we check for our end condition inside this callback and decremenent our std::latch in case the result is correct, which will stop the processing inside the threads and resume the main thread, which will print the solution and end the program.