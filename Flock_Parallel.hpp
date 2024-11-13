
#include <SFML/Graphics.hpp>
#include <omp.h>
#include <vector>
#include <array>
namespace parallel {
    const sf::VideoMode desktopTemp = sf::VideoMode::getDesktopMode();
    const float maxspeed= 6;
    const float minspeed = 3;

#define BOID_SIZE 7.f
    struct Boid_SoF
    {
        std::vector<float> positions;
        std::vector<float> velocities;
        std::vector<sf::CircleShape> shapes;

    };


    class Flock
    {
    public:

        Flock(float percent_of_screen, int numBoids = 500);
        void run();
        double test(int number_of_iteration);
        double testFast(int number_of_iteration) ;
        double testFastSequential(int number_of_iteration);

        int numthreads = 1;
        int nBoids= 500;
    private:
        int width;
        int height;
        void render();
        void pollEvents();
        void update();
        void updateTest();
        void updateFast();
        void updateFastSequential();
        void checkBoundaries(Boid_SoF  &all_boids, int index_boid);
        void createBoids();
        // Rules
        std::array<float,2>  alignment( float px,  float py,  float vx,  float vy   , int index_boid);
        std::array<float,2>  cohesion( float px, float py, int index_boid);
        std::array<float,2>  separation(float px, float py, int index_boid);


        sf::RenderWindow m_window;
        Boid_SoF all_boids;
        int visualRange = 75;
        float turnfactor = 0.2;
        float centeringFactor = 0.0005;
        float matchingFactor = 0.05;
        float protectedRange = 20;

        float avoidFactor = 0.05;
        float margin = 0.2;


    };
}