# Day 16: Ticket Translation

Quite a long one this one!

Decided to have a bit more fun with multithreading for the first and second part (validation and rule processing).

The threading strategy is to fire a bunch of thread to process as many tickets as possible. each thread processes a ticket, and we assign each thread a new ticket by reading and incrementing and atomic variable representing the next ticket index on every iteration.

The rule validation is relatively easy, since we are just setting a value to false when a ticket ins invalid, so there are no write conflicts. On the rule processesing part though we are reading and deleting elements from the same structure from multiple threads, so we need a bit more synchronization.

For that I use a shared_mutex. Share mutexes have the advantadge that they can be locked by multiple threads at the same time if desired. This allow as to have the threads soft-lock the mutex when they read, since having multiple read operations on the same data is not a problem here, but allow for a single thread to hard-lock the mutex when a write is needed. This way we can maximize paralelismn while executing the main logic and make sure we are not modifying the data from different threats at the same time.

Finally there is a bit more experimentation on using streams to process formated data. This makes reading the different types of data simpler from our raw text input. We only need to inbue the stream with the right locale and read it like we would do with any other input stream. This helps keep the logic cleaner since we don't need extra checks to verify that what we are reading is valid.