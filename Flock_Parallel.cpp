//
// Created by Bruno on 08/11/2024.
//
#include "Flock_Parallel.hpp"
#include "Utility.hpp"
#include <iostream>
using namespace parallel;

	Flock::Flock(float percent_of_screen,int numBoids):
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
		std::cout << "Mean time for update with parallelization : " << time << std::endl;
		return time;
	}


	void Flock::pollEvents()
	{
		sf::Event event;

		while (m_window.pollEvent(event))
		{
			/*if (event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Left)
			{
				sf::CircleShape body = sf::CircleShape(BOID_SIZE,  3);
				utility::centerOrigin(body);

				body.setPosition(m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window)));
				body.setFillColor(sf::Color::Red);

				all_boids.bodies.push_back(body);
				sf::Vector2f velocity = sf::Vector2f((rand()%(2*(int)maxspeed))-maxspeed, (rand()%(2*(int)maxspeed))-maxspeed)
				all_boids.velocities.push_back(velocity);
			}*/

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
		all_boids.bodies.push_back(body);
		sf::Vector2f velocity = sf::Vector2f((rand()%(2*(int)maxspeed))-maxspeed, (rand()%(2*(int)maxspeed))-maxspeed)
		all_boids.velocities.push_back(velocity);
	}


		}


	void Flock::update()
	{
		// Update boids
#pragma omp parallel for
		for (int i  = 0; i < nBoids; i++)//posso usare nboids perché ho tolto la possibilità di inserirli a mano
		{


			// Add the rules to the velocity of the boid
			all_boids.velocities[i] += cohesion(boid , i) + alignment(boid , i) + separation(boid , i);



			float spd =utility::length(all_boids.velocities[i]);
			if (spd>maxspeed) {
				all_boids.velocities[i] = all_boids.velocities[i]*maxspeed/spd;
			}
			if(spd<minspeed) {
				all_boids.velocities[i] = all_boids.velocities[i]*minspeed/spd;
			}
			sf::Vector2f p = all_boids.bodies[i].getPosition();

			checkBoundaries( all_boids.bodies[i],  all_boids.velocities[i] );

			all_boids.bodies[i].setPosition(p + all_boids.velocities[i]);

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

	void Flock::checkBoundaries( sf::CircleShape& body , sf::Vector2f& velocity)
	{


		if (body.getPosition().y > height*(1-margin))
			velocity.y = velocity.y - turnfactor;
		if(body.getPosition().y < height*margin)
			velocity.y = velocity.y + turnfactor;
		if(body.getPosition().x > width*(1-margin))
			velocity.x = velocity.x - turnfactor;
		if(body.getPosition().x < width*margin)
			velocity.x = velocity.x + turnfactor;

	}

	// Rule 1: alignment
	sf::Vector2f Flock::alignment(const sf::CircleShape& body , const sf::Vector2f& velocity , int pos_boid)
	{
		sf::Vector2f v = sf::Vector2f(0.f, 0.f);
		int neighbors = 0;

		for (int i =0 ; i<all_boids.bodies.size(); i++)
		{
			if (utility::distance(all_boids.bodies[i].getPosition(), body.getPosition())< visualRange && i != pos_boid)
			{
				v += all_boids.velocities[i];
				neighbors++;
			}
		}

		if (neighbors == 0)
		{
			return sf::Vector2f(0.f, 0.f);
		}

		// Direction vector to nearby boids
		v /= static_cast<float>(neighbors);
		v = (v-velocity )*matchingFactor;

		return v;
	}


	sf::Vector2f Flock::cohesion(const sf::CircleShape& body , int pos_boid)
		{
			sf::Vector2f p = sf::Vector2f(0.f, 0.f);
			sf::Vector2f l = sf::Vector2f(0.f, 0.f);
			int neighbors = 0;

			for (int i =0 ; i<all_boids.bodies.size(); i++)
			{
				if (utility::distance(all_boids.bodies[i].getPosition(), body.getPosition())< visualRange && i != pos_boid )
				{
					p += all_boids.bodies[i].getPosition();
					neighbors++;
				}
			}

			if (neighbors == 0)
			{
				return sf::Vector2f(0.f, 0.f);
			}

			// Direction vector towards the center of mass
			p /= static_cast<float>(neighbors);
			l = (p- body.getPosition())*centeringFactor;


			return l;
		}



	// Rule 3: separation
	sf::Vector2f Flock::separation(const sf::CircleShape& body , int pos_boid)
	{
		sf::Vector2f v = sf::Vector2f(0.f, 0.f);
		int neighbors = 0;

		for (int i =0 ; i<all_boids.bodies.size(); i++)
		{
			if (utility::distance(body.getPosition(), all_boids.bodies[i].getPosition())<protectedRange && i != pos_boid)
			{

					v +=  body.getPosition() - all_boids.bodies[i].getPosition();
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


