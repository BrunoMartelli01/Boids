
#include <SFML/Graphics.hpp>
const sf::VideoMode desktopTemp = sf::VideoMode::getDesktopMode();
const int WIDTH =desktopTemp.width*0.8;
const int HEIGHT = desktopTemp.height*0.8;
const float maxspeed= 6;
const float minspeed = 3;

#define BOID_SIZE 7.f
    struct Boid
    {
        Boid(const sf::CircleShape& body, const sf::Vector2f& velocity = sf::Vector2f((rand()%(2*(int)maxspeed))-maxspeed, (rand()%(2*(int)maxspeed))-maxspeed) ):
            body(body), velocity(velocity) {}

        sf::CircleShape body;
        sf::Vector2f velocity;
        int scoutGroup;


    };

    class Flock
    {
    public:
        Flock();

        void run();

    private:
        void render();
        void pollEvents();
        void update();
        void updateTest();
        void checkBoundaries(Boid& boid);
        void createBoids();
        // Rules
        sf::Vector2f alignment(const Boid& boid);
        sf::Vector2f cohesion(const Boid& boid);
        sf::Vector2f separation(const Boid& boid);


        sf::RenderWindow m_window;
        std::vector<Boid> all_boids;
        int visualRange = 75;
        float turnfactor = 0.2;
        float centeringFactor = 0.005;
        float matchingFactor = 0.05;
        float protectedRange = 20;

        float avoidFactor = 0.05;
        float margin = 0.2;
        float nBoids = 500;
    };