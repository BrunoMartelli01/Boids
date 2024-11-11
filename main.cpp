#include <iostream>

#include "Flock.hpp"
#include "Flock_Parallel.hpp"

int main() {
    srand(10);
    int nboids = 500;
    sq::Flock flock(0.8, nboids);


    double time_sequential = flock.test(1000);
    parallel::Flock flock_parallel(0.8, nboids);

    srand(10);
    double time_parallel = flock_parallel.test(1000);
    double speedup = time_sequential / time_parallel;
    std::cout <<"speedup: "<< speedup << std::endl;
    return 0;
}

// int main() {
//     srand(10);
//     int nboids = 500;
//     //sq::Flock flock(0.8, nboids);
//     //flock.run();
//
//
//     parallel::Flock flock_parallel(0.8);
//     flock_parallel.run();
//
//     return 0;
// }
