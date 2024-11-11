#include <iostream>

#include "Flock.hpp"
#include "Flock_Parallel.hpp"

int main() {
    srand(10);
    sq::Flock flock(0.8);
    flock.run();

    double time_sequential = flock.test(1000);
    srand(10);
    parallel::Flock flock_parallel(0.8);
    flock_parallel.run();
    double time_parallel = flock_parallel.test(1000);
    double speedup = time_sequential / time_parallel;
    std::cout <<"speedup: "<< speedup << std::endl;
    return 0;
}
