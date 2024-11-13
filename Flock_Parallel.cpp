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
			updateFast();
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
		return time;
	}
double Flock::testFast(int number_of_iteration) {
		m_window.close();
		createBoids();
		double time = 0.0;
		for (int i = 0; i < number_of_iteration; i++) {
			double start =  omp_get_wtime();
			updateFast();
			double end =  omp_get_wtime();
			time +=  end - start;
		}
		time /= number_of_iteration;
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
		sf::CircleShape shape = sf::CircleShape(BOID_SIZE,  3);
		utility::centerOrigin(shape);
		shape.setFillColor(sf::Color::Red);

		all_boids.shapes.push_back(shape);

		all_boids.positions.push_back(rand()%width);  //x
		all_boids.positions.push_back(rand()%height); //y


		all_boids.velocities.push_back((rand()%(2*(int)maxspeed))-maxspeed); //x
		all_boids.velocities.push_back((rand()%(2*(int)maxspeed))-maxspeed); //y


	}


		}


	void Flock::update() {
		// Update boids
#pragma omp parallel for num_threads(Flock::numthreads)
		for (int i  = 0; i < nBoids; i++)//posso usare nboids perché ho tolto la possibilità di inserirli a mano
		{
			// Add the rules to the velocity of the boid
			float px = all_boids.positions[i*2];
			float py = all_boids.positions[i*2+1];
			float vx = all_boids.velocities[i*2];
			float vy = all_boids.velocities[i*2+1];
			std::array<float,2 > c = cohesion(px, py, i);
			std::array<float,2 > a = alignment(px, py,vx, vy, i);
			std::array<float,2 > s = separation(px,py , i);
			all_boids.velocities[i*2] += c[0] + a[0] + s[0];
			all_boids.velocities[i*2+1] += c[1] + a[1] + s[1];


			float spd =utility::length(all_boids.velocities[i*2],all_boids.velocities[i*2+1]);
			if (spd>maxspeed) {
				all_boids.velocities[i*2] = all_boids.velocities[i*2]*maxspeed/spd;

				all_boids.velocities[i*2+1] = all_boids.velocities[i*2+1]*maxspeed/spd;
			}
			if(spd<minspeed) {
				all_boids.velocities[i*2] = all_boids.velocities[i*2]*minspeed/spd;
				all_boids.velocities[i*2+1] = all_boids.velocities[i*2+1]*minspeed/spd;
			}


			checkBoundaries( all_boids,  i);

			all_boids.positions[i*2]= px + all_boids.velocities[i*2];
			all_boids.positions[i*2+1]= py + all_boids.velocities[i*2+1];

		}
	}

void Flock::updateFast() {
		// Update boids
#pragma omp parallel for num_threads(Flock::numthreads)
		for (int i  = 0; i < nBoids; i++)//posso usare nboids perché ho tolto la possibilità di inserirli a mano
		{
			// Add the rules to the velocity of the boid
			float px = all_boids.positions[i*2];
			float py = all_boids.positions[i*2+1];
			float vx = all_boids.velocities[i*2];
			float vy = all_boids.velocities[i*2+1];

			float temp_vx = 0;
			float temp_vy = 0;
			float temp_px = 0;
			float temp_py = 0;
			float temp_vx_s = 0;
			float temp_vy_s = 0;

			int neighbors = 0;

			int neighbors_s = 0;
			for(int k =0 ; k<nBoids; k++) {
				if(i != k) {
					float distance = utility::distance(px, py, all_boids.positions[k*2], all_boids.positions[k*2+1]);
					if (distance < visualRange)
					{

						temp_vx += all_boids.velocities[k*2];
						temp_vy += all_boids.velocities[k*2+1];
						temp_px += all_boids.positions[k*2];
						temp_py += all_boids.positions[k*2+1];
						neighbors++;

						if(distance < protectedRange) {
							temp_vx_s +=  px - all_boids.positions[k*2];
							temp_vy_s +=  py - all_boids.positions[k*2+1];
							neighbors_s++;
						}

					}




				}
			}


			if (neighbors != 0)
			{
				temp_vx /= neighbors;
				temp_vy /= neighbors;
				temp_vx = (temp_vx - vx )*matchingFactor;
				temp_vy = (temp_vy - vy )*matchingFactor;
				temp_px /= neighbors;
				temp_py /= neighbors;
				temp_px = (temp_px- px)*centeringFactor;
				temp_py = (temp_py- py)*centeringFactor;
			}
			if (neighbors_s != 0)
			{
				temp_vx_s /= neighbors_s;
				temp_vy_s /= neighbors_s;
				temp_vx_s *= avoidFactor;
				temp_vy_s *= avoidFactor;
			}

			all_boids.velocities[i*2] += temp_vx + temp_px + temp_vx_s;
			all_boids.velocities[i*2+1] += temp_vy + temp_py + temp_vy_s;


			float spd =utility::length(all_boids.velocities[i*2],all_boids.velocities[i*2+1]);
			if (spd>maxspeed) {
				all_boids.velocities[i*2] = all_boids.velocities[i*2]*maxspeed/spd;

				all_boids.velocities[i*2+1] = all_boids.velocities[i*2+1]*maxspeed/spd;
			}
			if(spd<minspeed) {
				all_boids.velocities[i*2] = all_boids.velocities[i*2]*minspeed/spd;
				all_boids.velocities[i*2+1] = all_boids.velocities[i*2+1]*minspeed/spd;
			}


			checkBoundaries( all_boids,  i);

			all_boids.positions[i*2]= px + all_boids.velocities[i*2];
			all_boids.positions[i*2+1]= py + all_boids.velocities[i*2+1];

		}
	}
// void Flock::updateTest()
// 	{
//
// 		std::vector<sf::Vector2<float>> v;
//
// 		int i = 0;	// Update boids
// 		for (auto& boid : all_boids)
// 		{
//
// 			// Add the rules to the velocity of the boid
// 			v.push_back(boid.velocity + cohesion(boid) + alignment(boid) + separation(boid));
//
// 			float spd =utility::length(v[i]);
// 			if (spd>maxspeed) {
// 				v[i] = v[i]*maxspeed/spd;
// 			}
// 			if(spd<minspeed) {
// 				v[i] = v[i]*minspeed/spd;
// 			}
//
// 			i++;
//
// 		}
// 		i =0;
// 		for(auto&boid : all_boids){
// 			boid.velocity = v[i];
// 			checkBoundaries(boid);
// 			sf::Vector2f p = boid.body.getPosition();
// 			boid.body.setPosition(p +boid.velocity);
// 			i++;
// 		}
// 	}

	void Flock::render()
	{
		m_window.clear();

		// Draw boids
		for (int i = 0; i < nBoids; i++)
		{
			all_boids.shapes[i].setPosition(all_boids.positions[i*2], all_boids.positions[i*2+1]);
			m_window.draw(all_boids.shapes[i]);
		}


		m_window.display();

	}

	void Flock::checkBoundaries( Boid_SoF &all_boids, int index_boid)
	{


		if (all_boids.positions[index_boid*2+1] > height*(1-margin))
			all_boids.velocities[index_boid*2+1] = all_boids.velocities[index_boid*2+1] - turnfactor;
		if(all_boids.positions[index_boid*2+1] < height*margin)
			all_boids.velocities[index_boid*2+1] = all_boids.velocities[index_boid*2+1] + turnfactor;
		if(all_boids.positions[index_boid*2] > width*(1-margin))
			all_boids.velocities[index_boid*2] = all_boids.velocities[index_boid*2] - turnfactor;
		if(all_boids.positions[index_boid*2] < width*margin)
			all_boids.velocities[index_boid*2] = all_boids.velocities[index_boid*2] + turnfactor;

	}

	// Rule 1: alignment
	std::array<float,2> Flock::alignment(const float px, const float py, const float vx, const float vy   , int index_boid)
	{
		float temp_vx = 0;
		float temp_vy = 0;
		int neighbors = 0;

		for (int i =0 ; i<nBoids; i++)
		{
			float distance = utility::distance(px, py, all_boids.positions[i*2], all_boids.positions[i*2+1]);
			if (distance < visualRange && i != index_boid)
			{
				temp_vx += all_boids.velocities[i*2];
				temp_vy += all_boids.velocities[i*2+1];
				neighbors++;
			}
		}

		if (neighbors == 0)
		{
			return {0.f, 0.f} ;
		}

		// Direction vector to nearby boids
		temp_vx /= neighbors;
		temp_vy /= neighbors;
		temp_vx = (temp_vx - vx )*matchingFactor;
		temp_vy = (temp_vy - vy )*matchingFactor;
		return {temp_vx, temp_vy} ;
	}


	std::array<float,2>  Flock::cohesion(float px, float py  , int index_boid)
		{
			float temp_px = 0;
			float temp_py = 0;
			int neighbors = 0;

			for (int i =0 ; i<nBoids; i++)
			{
				float distance = utility::distance(px, py, all_boids.positions[i*2], all_boids.positions[i*2+1]);
				if(distance < visualRange && i != index_boid)
				{
					temp_px += all_boids.positions[i*2];
					temp_py += all_boids.positions[i*2+1];
					neighbors++;
				}
			}

			if (neighbors == 0)
			{
				return {0.f, 0.f};
			}

			// Direction vector towards the center of mass
			temp_px /= neighbors;
			temp_py /= neighbors;

			temp_px = (temp_px- px)*centeringFactor;
			temp_py = (temp_py- py)*centeringFactor;


			return {temp_px, temp_py};
		}



	// Rule 3: separation
	std::array<float,2>  Flock::separation(float px, float py, int index_boid)
	{
		float temp_vx = 0;
		float temp_vy = 0;
		int neighbors = 0;

		for (int i =0 ; i<nBoids; i++)
		{
			float distance = utility::distance(px, py, all_boids.positions[i*2], all_boids.positions[i*2+1]);
			if (distance <protectedRange && i != index_boid)
			{

					temp_vx +=  px - all_boids.positions[i*2];
					temp_vy +=  py - all_boids.positions[i*2+1];

					neighbors++;
				}
			}


		if (neighbors == 0)
		{
			return {0.f, 0.f};
		}
		temp_vx *= avoidFactor;
		temp_vy *= avoidFactor;
		return {temp_vx, temp_vy};
	}


