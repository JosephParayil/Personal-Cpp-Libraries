#pragma once


#include <array>
#include <utility>
#include <optional>

#include <SFML/Graphics.hpp>
#include <sfml-util/sfml_util.hpp>

struct Shape2D {
    virtual ~Shape2D() = default;

    virtual void draw(sf::RenderWindow& window, sf::Color color) = 0;

    virtual bool computeCollisionWithPoint(sf::Vector2f point) = 0;
};

struct Line2D : Shape2D {
    sf::Vector2f a, b;
    float thickness;

    Line2D(sf::Vector2f start, sf::Vector2f end, float t = 1.0f) 
        : a(start), b(end), thickness(t) {}

    void draw(sf::RenderWindow& window, sf::Color color = sf::Color::White) override {
        drawLine(window, a, b, thickness, color);
    }

    bool computeCollisionWithPoint(sf::Vector2f point) override {
        return distanceToLineSegment(point, a, b) <= (thickness + 20.0f);
    }
};

struct Circle2D : Shape2D {
    sf::Vector2f center;
    float radius;

    Circle2D(sf::Vector2f c, float r) : center(c), radius(r) {}

    void draw(sf::RenderWindow& window, sf::Color color = sf::Color::White) override {
        sf::CircleShape sfCircle(radius);
        sfCircle.setOrigin({radius, radius});
        sfCircle.setPosition(center);
        sfCircle.setFillColor(color);
        window.draw(sfCircle);
    }

    bool computeCollisionWithPoint(sf::Vector2f point) override {
        sf::Vector2f diff = point - center;
        float distanceSq = diff.x * diff.x + diff.y * diff.y;
        return distanceSq <= radius * radius;
    }
};