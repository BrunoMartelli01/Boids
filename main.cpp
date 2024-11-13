#include <iostream>

#include "Flock.hpp"
#include "Flock_Parallel.hpp"

int main() {
    int num_threads[] ={1,2,4,8,16,32,256,1024};
    int num_threads_size = 8;
    int nboids []={10, 100, 250, 500};
    int nboids_size = 4;
for(int i = 0; i < nboids_size; i++) {
    srand(10);
    sq::Flock flock(0.8, nboids[i]);
    double time_sequential = flock.test(1000);

    for(int k =0 ; k < num_threads_size; k++) {

        srand(10);
        parallel::Flock flock_parallel(0.8, nboids[i]);
        flock_parallel.numthreads = num_threads[k];
        double time_parallel = flock_parallel.test(1000);


        srand(10);
        parallel::Flock flock_parallel_fast(0.8, nboids[i]);
        flock_parallel_fast.numthreads = num_threads[k];
        double time_parallel_fast = flock_parallel_fast.testFast(1000);

        std::cout << "number of threads: " << num_threads[k] <<  " nBoids: "<<nboids[i]<<std::endl;
        std::cout << "sequential time: " << time_sequential << std::endl;
        std::cout << "parallel time: " << time_parallel << std::endl;
        std::cout << "parallel fast time: " << time_parallel_fast << std::endl;


        double speedup_to_parallel = time_sequential / time_parallel;
        std::cout <<"speedup sequential -> parallel: "<< speedup_to_parallel << std::endl;

        double speedup_to_parallel_fast = time_parallel / time_parallel_fast;
        std::cout <<"speedup parallel -> fast: "<< speedup_to_parallel_fast << std::endl;

        double speedup_to_sequential_fast = time_sequential / time_parallel_fast;
        std::cout <<"speedup sequential -> fast: "<< speedup_to_sequential_fast << std::endl<<std::endl;
    }
    std::cout<<"----------------------------------------------------------------------------------------------------------"<<std::endl;
}
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
