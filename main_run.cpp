#include <stdlib.h>

#include "Flock_Parallel.hpp"
//
// Created by bruno on 13/11/2024.
//
int main() {
    srand(10);
    int nboids = 500;
    //sq::Flock flock(0.8, nboids);
    //flock.run();


    parallel::Flock flock_parallel(0.8);
    flock_parallel.run();

    return 0;
}
