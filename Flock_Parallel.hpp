
#include <SFML/Graphics.hpp>
#include <omp.h>
namespace parallel {
    const sf::VideoMode desktopTemp = sf::VideoMode::getDesktopMode();
    const float maxspeed= 6;
    const float minspeed = 3;

#define BOID_SIZE 7.f
    struct Boid_SoF
    {
        std::vector<sf::CircleShape> bodies;
        std::vector<sf::Vector2f> velocities;

    };


    class Flock
    {
    public:

        Flock(float percent_of_screen, int numBoids = 500);
        void run();
        double test(int number_of_iteration);

    private:
        int width;
        int height;
        void render();
        void pollEvents();
        void update();
        void updateTest();
        void checkBoundaries(sf::CircleShape& body , sf::Vector2f& velocity);
        void createBoids();
        // Rules
        sf::Vector2f alignment(const sf::CircleShape& body , const sf::Vector2f& velocity , int pos_boid);
        sf::Vector2f cohesion(const sf::CircleShape& body , int pos_boid);
        sf::Vector2f separation(const sf::CircleShape& body , int pos_boid);


        sf::RenderWindow m_window;
        Boid_SoF all_boids;
        int visualRange = 75;
        float turnfactor = 0.2;
        float centeringFactor = 0.0005;
        float matchingFactor = 0.05;
        float protectedRange = 20;

        float avoidFactor = 0.05;
        float margin = 0.2;
        int nBoids;
    };
}