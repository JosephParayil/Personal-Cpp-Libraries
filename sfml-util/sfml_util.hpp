//SFML 3 1/27/2026

#pragma once
#include <SFML/Graphics.hpp>


void drawLine(
    sf::RenderWindow& window,
    sf::Vector2f a,
    sf::Vector2f b,
    float thickness = 1.0f,
    sf::Color color = sf::Color::White
) 
{
    sf::Vector2f direction = b - a;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    sf::RectangleShape line(sf::Vector2f(length, thickness));
    line.setFillColor(color);
    line.setOrigin(sf::Vector2f(0.f, thickness / 2.f));
    line.setPosition(a);

    float angle = std::atan2(direction.y, direction.x);
    line.setRotation(sf::radians(angle));

    window.draw(line);
}

float distanceToLineSegment(sf::Vector2f point, sf::Vector2f a, sf::Vector2f b) {
    sf::Vector2f ab = b - a;
    sf::Vector2f ap = point - a;
    
    float abLengthSq = ab.x * ab.x + ab.y * ab.y;
    
    // Handle degenerate case where a == b
    if (abLengthSq == 0.f) {
        sf::Vector2f diff = point - a;
        return std::sqrt(diff.x * diff.x + diff.y * diff.y);
    }
    
    // Project point onto the line, clamped to [0, 1] to stay on the segment
    float t = std::max(0.f, std::min(1.f, (ap.x * ab.x + ap.y * ab.y) / abLengthSq));
    
    // Find the closest point on the segment
    sf::Vector2f closest = a + t * ab;
    
    // Return distance from point to closest point
    sf::Vector2f diff = point - closest;
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

std::ostream& operator<<(std::ostream& os, const sf::Vector2f& v)
{
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}


float magnitude(const sf::Vector2f& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

float dot(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return a.x * b.x + a.y * b.y;
}


sf::Vector2f perpendicular(sf::Vector2f a) {
    return sf::Vector2f(a.y, -a.x);
}