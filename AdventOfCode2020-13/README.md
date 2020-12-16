# Day 13: Shuttle Search

This one was hard. It might not look like it when looking at the code, but finding a good solution requiring a bit of investigation of how to calculate multiples, modulus and congruences.

In the end, the solution turned out to be the Chinese remainder theorem, which allow us to find a number which is congruent to another number modulus something else. 
That is:

X ≡ a (mod i)
X ≡ b (mod j)
X ≡ c (mod k)
...

For a, b, c and i, j, k known; we can find an X that satisfy these conditions.