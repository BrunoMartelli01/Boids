#include "Flock.hpp"
// Created by bruno on 25/10/2024.
//

#include "Utility.hpp"
#include <iostream>


using namespace sq;

	Flock::Flock(float percent_of_screen,int numBoids) :
		nBoids(numBoids),
		width(desktopTemp.width*percent_of_screen),
		height(desktopTemp.height*percent_of_screen),
		m_window(sf::VideoMode(width,height),
				"Boids",
				sf::Style::Close,
				sf::ContextSettings(0, 0, 8))
		{
			m_window.setVerticalSyncEnabled(true);
		}



	void Flock::run()
	{
		createBoids();
		while (m_window.isOpen())
		{
			pollEvents();
			//updateTest();
			update();
			render();
		}
	}
	double Flock::test(int number_of_iteration) {
		m_window.close();
		createBoids();
		double time = 0.0;
		for (int i = 0; i < number_of_iteration; i++) {
			double start =  omp_get_wtime();
			update();
			double end =  omp_get_wtime();
			time +=  end - start;
		}
		time /= number_of_iteration;
		std::cout << "Mean time for update with no parallelization : " << time << std::endl;
		return time;
	}


	void Flock::pollEvents()
	{
		sf::Event event;

		while (m_window.pollEvent(event))
		{
			if (event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Left)
			{
				sf::CircleShape body = sf::CircleShape(BOID_SIZE,  3);
				utility::centerOrigin(body);

				body.setPosition(m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window)));
				body.setFillColor(sf::Color::Red);
				Boid b = Boid(body);
				all_boids.push_back(b);
			}

				if (event.type == sf::Event::Closed|| (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
				{
					m_window.close();
				}
			}

	}
void Flock::createBoids()
	{

				for (int i = 0; i < nBoids; i++){
				sf::CircleShape body = sf::CircleShape(BOID_SIZE,  3);
				utility::centerOrigin(body);
				body.setPosition(rand()%width, rand()%height);
				body.setFillColor(sf::Color::Red);
				all_boids.push_back(Boid(body));
			}


		}


	void Flock::update()
	{
//#pragma omp parallel for
		// Update boids
		for (auto& boid : all_boids)
		{


			// Add the rules to the velocity of the boid
			boid.velocity += cohesion(boid) + alignment(boid) + separation(boid);



			float spd =utility::length(boid.velocity);
			if (spd>maxspeed) {
				boid.velocity = boid.velocity*maxspeed/spd;
			}
			if(spd<minspeed) {
				boid.velocity = boid.velocity*minspeed/spd;
			}
			sf::Vector2f p = boid.body.getPosition();

			checkBoundaries(boid);

			boid.body.setPosition(p +boid.velocity);

		}
	}
void Flock::updateTest()
	{

		std::vector<sf::Vector2<float>> v;

		int i = 0;	// Update boids
		for (auto& boid : all_boids)
		{

			// Add the rules to the velocity of the boid
			v.push_back(boid.velocity + cohesion(boid) + alignment(boid) + separation(boid));

			float spd =utility::length(v[i]);
			if (spd>maxspeed) {
				v[i] = v[i]*maxspeed/spd;
			}
			if(spd<minspeed) {
				v[i] = v[i]*minspeed/spd;
			}

			i++;

		}
		i =0;
		for(auto&boid : all_boids){
			boid.velocity = v[i];
			checkBoundaries(boid);
			sf::Vector2f p = boid.body.getPosition();
			boid.body.setPosition(p +boid.velocity);
			i++;
		}
	}

	void Flock::render()
	{
		m_window.clear();

		// Draw boids
		for (const auto& boid : all_boids)
		{
			m_window.draw(boid.body);
		}

		m_window.display();

	}

	void Flock::checkBoundaries(Boid& boid)
	{
		// Reset position of a boid if it's outside the window
		if (boid.body.getPosition().y > height*(1-margin))
			boid.velocity.y = boid.velocity.y - turnfactor;
		if(boid.body.getPosition().y < height*margin)
			boid.velocity.y = boid.velocity.y + turnfactor;
		if(boid.body.getPosition().x > width*(1-margin))
			boid.velocity.x = boid.velocity.x - turnfactor;
		if(boid.body.getPosition().x < width*margin)
			boid.velocity.x = boid.velocity.x + turnfactor;

	}

	// Rule 1: alignment
	sf::Vector2f Flock::alignment(const Boid& boid)
	{
		sf::Vector2f v = sf::Vector2f(0.f, 0.f);
		int neighbors = 0;

		for (const auto& otherBoid : all_boids)
		{
			// if(&boid == &targetBoid)
			// 	std::cout<<"boid: " << boid.body.getPosition().x<< "target: "<<targetBoid.body.getPosition().x<< std::endl;
			if (utility::distance(otherBoid.body.getPosition(), boid.body.getPosition())< visualRange && &otherBoid != &boid)
			{
				v += otherBoid.velocity;
				neighbors++;
			}
		}

		if (neighbors == 0)
		{
			return sf::Vector2f(0.f, 0.f);
		}

		// Direction vector to nearby boids
		v /= static_cast<float>(neighbors);
		v = (v-boid.velocity )*matchingFactor;

		return v;
	}


	sf::Vector2f Flock::cohesion(const Boid& boid)
		{
			sf::Vector2f p = sf::Vector2f(0.f, 0.f);
			sf::Vector2f l = sf::Vector2f(0.f, 0.f);
			int neighbors = 0;

			for (const auto& otherBoid : all_boids)
			{
				if (utility::distance(otherBoid.body.getPosition(), boid.body.getPosition())< visualRange && &otherBoid != &boid )
				{
					p += otherBoid.body.getPosition();
					neighbors++;
				}
			}

			if (neighbors == 0)
			{
				return sf::Vector2f(0.f, 0.f);
			}

			// Direction vector towards the center of mass
			p /= static_cast<float>(neighbors);
			l = (p- boid.body.getPosition())*centeringFactor;


			return l;
		}



	// Rule 3: separation
	sf::Vector2f Flock::separation(const Boid& boid)
	{
		sf::Vector2f v = sf::Vector2f(0.f, 0.f);
		int neighbors = 0;

		for (const auto& otherBoid : all_boids)
		{
			if (utility::distance(boid.body.getPosition(), otherBoid.body.getPosition())<protectedRange && &otherBoid != &boid)
			{

					v +=  boid.body.getPosition() - otherBoid.body.getPosition();
					neighbors++;
				}
			}


		if (neighbors == 0)
		{
			return sf::Vector2f(0.f, 0.f);
		}
		v *= avoidFactor;

		return v;
	}


