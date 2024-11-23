
#include <iostream>
#include <filesystem>
#include "Flock.hpp"
#include "Flock_Parallel.hpp"
#include <fstream>
int main() {
    int num_threads[] ={1,2,4,8,16,32,64,128,256, 512,1024};
    int num_threads_size = sizeof(num_threads)/sizeof(num_threads[0]);
    int nboids []={10, 100, 250 ,500};
    int nboids_size = sizeof(nboids)/sizeof(nboids[0]);
    int number_of_iterations = 500;

for(int i = 0; i < nboids_size; i++) {

    std::string filename = "boidsN"+std::to_string(nboids[i])+".csv";

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Errore nell'apertura del file" << std::endl;
        return 1;
    }

    file<<"number of threads,  number of boids, "
          "sequential AoS time, sequential SoA time,  sequential SoA + Fast time, "
          "parallel SoA + Fast  time, "
          "speedup sequential AoS ->  sequntial SoA, "
            "speedup sequential SoA -> sequential  SoA + Fast, "
          "speedup sequential SoA + fast  -> parallel SoA + Fast, "
          "speedup sequential AoS -> parallel SoA + Fast "<<std::endl;




    sq::Flock flock(0.8, nboids[i]);
    double time_sequential = flock.test(number_of_iterations);

    parallel::Flock flock_Soa(0.8, nboids[i]);
    double time_sequential_Soa = flock_Soa.test(number_of_iterations);

    parallel::Flock flock_fast(0.8, nboids[i]);
    double time_sequential_fast = flock_fast.testFastSequential(number_of_iterations);



    for(int k =0 ; k < num_threads_size; k++) {



        parallel::Flock flock_parallel_fast(0.8, nboids[i]);
        flock_parallel_fast.numthreads = num_threads[k];
        double time_parallel_fast = flock_parallel_fast.testFast(number_of_iterations);





        file<<num_threads[k]<<", "<<nboids[i]<<", "<<time_sequential<<", "<< time_sequential_Soa << ", "<<time_sequential_fast<<", "<<time_parallel_fast<<", ";
        std::cout << "number of threads: " << num_threads[k] << " number of boids: " << nboids[i] << std::endl;
        std::cout << "sequential time: " << time_sequential << std::endl;
        std::cout << "sequential SoA time: " << time_sequential_Soa << std::endl;
        std::cout << "sequential fast time: " << time_sequential_fast << std::endl;
        std::cout << "parallel fast time: " << time_parallel_fast << std::endl<<std::endl;

        double speedup_to_fast = time_sequential / time_sequential_Soa;
        std::cout <<"speedup sequential_AoS -> sequential_SoA: "<< speedup_to_fast << std::endl;
        file<<speedup_to_fast<<", ";


        double sequential_to_sequential_fast = time_sequential_Soa / time_sequential_fast;
        std::cout <<"speedup sequential_SoA -> sequential_SoA + fast: "<< sequential_to_sequential_fast << std::endl;
        file<<sequential_to_sequential_fast<<", ";


        double sequential_fast_to_parallel_fast = time_sequential_fast / time_parallel_fast;
        std::cout <<"speedup sequential_fast -> parallel_fast: "<< sequential_fast_to_parallel_fast << std::endl;
        file<<sequential_fast_to_parallel_fast<<", ";

        double speedup_to_sequential_fast = time_sequential / time_parallel_fast;
        std::cout <<"speedup sequential -> fast: "<< speedup_to_sequential_fast << std::endl<<std::endl<<std::endl<<std::endl;
        file << speedup_to_sequential_fast << std::endl;
    }
    file.close();

    std::cout<<"----------------------------------------------------------------------------------------------------------"<<std::endl<<std::endl<<std::endl<<std::endl;
}


    return 0;
}

