#pragma once
/*
Code for drawing stuff in 3D
*/

#include <array>
#include <utility>
#include <optional>

#include <SFML/Graphics.hpp>
#include "math4.hpp"
#include <sfml-util/sfml_util.hpp>



sf::Vector2f normalize_point(sf::Vector2f raw, int WIDTH, int HEIGHT) {
    return {raw.x+WIDTH/2, HEIGHT/2-raw.y};
}

float FOV = 500;

sf::Vector2f convert_3d_to_2d(vec4 point_3d) {
    return {FOV*point_3d.x/point_3d.z, FOV*point_3d.y/point_3d.z};
}


std::array<vec4, 8> cubeVertices(
    vec4 center,
    float edge
) {
    float h = edge / 2.f;

    return {
        vec4{center.x - h, center.y - h, center.z - h, 1}, // 0
        vec4{center.x + h, center.y - h, center.z - h, 1}, // 1
        vec4{center.x + h, center.y + h, center.z - h, 1}, // 2
        vec4{center.x - h, center.y + h, center.z - h, 1}, // 3
        vec4{center.x - h, center.y - h, center.z + h, 1}, // 4
        vec4{center.x + h, center.y - h, center.z + h, 1}, // 5
        vec4{center.x + h, center.y + h, center.z + h, 1}, // 6
        vec4{center.x - h, center.y + h, center.z + h, 1}  // 7
    };
}


const int cubeEdges[12][2] = { 
    {0,1},{1,2},{2,3},{3,0}, // bottom face
    {4,5},{5,6},{6,7},{7,4}, // top face
    {0,4},{1,5},{2,6},{3,7}  // vertical edges
};


const static float NEAR = 0.01f;


std::optional<std::pair<sf::Vector2f, float>> computeSphere(sf::RenderWindow& window, vec4 a, float radius, mat4& camera) {
    mat4 camera_inverse = camera.inverse_rigid();
    a = camera_inverse * a;

    if (a.z <= NEAR) return std::nullopt;

    int WIDTH = window.getSize().x;
    int HEIGHT = window.getSize().y;

    float radius_ = FOV*radius/a.z;
    sf::Vector2f a_ = normalize_point(convert_3d_to_2d(a), WIDTH, HEIGHT);

    return std::make_pair(a_, radius_);
}

void drawCircle(sf::RenderWindow& window, sf::Vector2f a, float radius, sf::Color color = sf::Color::White) {
    sf::CircleShape sfCircle(radius);
    sfCircle.setOrigin(sf::Vector2f(radius, radius));
    sfCircle.setPosition(sf::Vector2f(a.x, a.y));
    sfCircle.setFillColor(color);
    window.draw(sfCircle);
}

void drawSphere(sf::RenderWindow& window, vec4 a, float radius, mat4& camera, sf::Color color = sf::Color::White) {
    if (auto circle = computeSphere(window, a, radius, camera)) {
        auto [a_, radius_] = *circle;
        drawCircle(window, a_, radius_, color);
    }
}


std::optional<std::pair<sf::Vector2f, sf::Vector2f>> computeLine(sf::RenderWindow& window, vec4 a, vec4 b, mat4& camera) {
    mat4 camera_inverse = camera.inverse_rigid();
    a = camera_inverse* a;
    b = camera_inverse * b;

    if (a.z <= 0 && b.z <= 0)
        return std::nullopt;

    if (a.z <= 0) {
        float t = (NEAR-a.z) / (b.z - a.z);

        a.x = a.x + t * (b.x - a.x);
        a.y = a.y + t * (b.y - a.y);
        a.z = NEAR;
    }

    if (b.z <= 0) {
        float t = (NEAR-a.z) / (b.z - a.z);

        b.x = a.x + t * (b.x - a.x);
        b.y = a.y + t * (b.y - a.y);
        b.z = NEAR;
    }

    int WIDTH = window.getSize().x;
    int HEIGHT = window.getSize().y;

    sf::Vector2f a_ = normalize_point(convert_3d_to_2d(a), WIDTH, HEIGHT);
    sf::Vector2f b_ = normalize_point(convert_3d_to_2d(b), WIDTH, HEIGHT);

    return std::make_pair(a_, b_);
}

void draw3DLine(sf::RenderWindow& window, vec4 a, vec4 b, mat4& camera, float thickness = 1.0f) {
    if (auto line = computeLine(window, a, b, camera)) {
        auto [a_, b_] = *line;
        drawLine(window, a_, b_, thickness);
    }
}

