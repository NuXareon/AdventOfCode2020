# Day 15: Rambunctious Recitation

The second part of the problem implies that we have to do a lot of iterations, so the goals here is to reduce the complexity of the calculation as much as possible.

The solutions here uses an std::unordered_map to track the last two positions for any value (the key of the map). 
Since the average case for finding an element on an std::unordered_map is constant, it means that our average computation cost will only scale with the number of turns (O(N)). This specially relevant, since the size of this map can grow quite a bit (for my input it reached size=3611520).

For comparsion, the computation time it took to solve this problem using a std::map (with search complexity of O(log N)) was of ~16.8s.
Using std::unordered_map the time got reduced to ~4.1s. That is a ~4x increase on speed (which would only get higher the bigger the input).