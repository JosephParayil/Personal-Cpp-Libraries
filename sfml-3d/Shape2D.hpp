#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <optional>
#include <sfml-util/sfml_util.hpp>
#include <utility>

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

    void draw(sf::RenderWindow& window,
              sf::Color color = sf::Color::White) override {
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

    void draw(sf::RenderWindow& window,
              sf::Color color = sf::Color::White) override {
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

struct Text2D : Shape2D {
    sf::Font font;
    sf::Text text;

    Text2D(sf::Vector2f pos, std::string t, sf::Font f)
        : font(f), text(font, t) {
        text.setPosition(pos);
        text.setOrigin({text.getGlobalBounds().size.x / 2,
                        text.getGlobalBounds().size.y / 2});
    }

    void draw(sf::RenderWindow& window,
              sf::Color color = sf::Color::White) override {
        text.setFillColor(color);
        text.setOutlineColor(color);
        window.draw(text);
    }

    bool computeCollisionWithPoint(sf::Vector2f point) override {
        return text.getGlobalBounds().contains(point);
    }
};
