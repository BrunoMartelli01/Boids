//
// Created by bruno on 25/10/2024.
//
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <math.h>

namespace utility
{
    inline float distance(const sf::Vector2f& v1, const sf::Vector2f& v2)
    {
        return sqrtf(((v2.x - v1.x) * (v2.x - v1.x)) + ((v2.y - v1.y) * (v2.y - v1.y)));
    }

    inline float length(const sf::Vector2f& v)
    {
        return sqrtf(v.x * v.x + v.y * v.y);
    }


    inline void centerOrigin(sf::Shape& shape)
    {
        sf::FloatRect bounds = shape.getLocalBounds();
        shape.setOrigin(std::floor(bounds.left + bounds.width / 2.f), std::floor(bounds.top + bounds.height / 2.f));
    }
}