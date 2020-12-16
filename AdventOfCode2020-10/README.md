# Day 10: Adapter Array

This one was tricky. Counting all the possible combinations (brute force) was obviously not an option, since that would take too long.

In the end, the solution turned out to be quite elegant: for each charger just add up all the possible combinations from all the charces that can get to it.

This is also called the Tribonacci Sequence. Which is basically the same idea as Fibonacci but instead of adding the two previous number we add the three previous numbers. In our case we need to do it conditionally based on the conditions of the problem.