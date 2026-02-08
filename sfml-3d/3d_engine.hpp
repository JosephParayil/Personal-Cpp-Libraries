#pragma once
/*
Code for drawing stuff in 3D
*/

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <sfml-util/sfml_util.hpp>
#include <utility>
#include <vector>

#include "3d_camera.hpp"
#include "math4.hpp"

static sf::Vector2f normalize_point(sf::RenderWindow& window,
                                    sf::Vector2f raw) {
    float width = static_cast<float>(window.getSize().x);
    float height = static_cast<float>(window.getSize().y);
    return {raw.x + width / 2.0f, height / 2.0f - raw.y};
}

float NEAR = 0.01f;

#include "Object3D.hpp"
#include "Shape2D.hpp"

// Helpers to make compatible with an older version of this library
// NOTE: Not 100% compatible. Before it was sufficient only to pass a mat4. Now
// you must pass the whole Camera object

void drawSphere(sf::RenderWindow& window, vec4 a, float radius, Camera& camera,
                sf::Color color = sf::Color::White) {
    Sphere3D sphere(a, radius);
    sphere.draw(window, camera, color);
}

void draw3DLine(sf::RenderWindow& window, vec4 a, vec4 b, Camera& camera,
                float thickness = 1.0f) {
    Line3D line(a, b, thickness);
    line.draw(window, camera);
}

// Drawables class: contains a bunch of Object3Ds

struct Object3D_Collection {
    std::vector<std::pair<int, Object3D*>> c;

    auto& operator[](std::size_t index) { return c[index]; }

    void resetDistances() {
        for (auto& pair : c) {
            pair.second->distance_updated = false;
        }
    }

    void depthSort(Camera& camera) {
        resetDistances();
        std::sort(c.begin(), c.end(), [&camera](const auto& a, const auto& b) {
            return a.second->getDistance(camera) >
                   b.second->getDistance(camera);
        });
    }
};

// CUBE (basically a collection of 12 Line3D objects)
std::array<vec4, 8> cubeVertices(vec4 center, float edge) {
    float h = edge / 2.f;

    return {
        vec4{center.x - h, center.y - h, center.z - h, 1},  // 0
        vec4{center.x + h, center.y - h, center.z - h, 1},  // 1
        vec4{center.x + h, center.y + h, center.z - h, 1},  // 2
        vec4{center.x - h, center.y + h, center.z - h, 1},  // 3
        vec4{center.x - h, center.y - h, center.z + h, 1},  // 4
        vec4{center.x + h, center.y - h, center.z + h, 1},  // 5
        vec4{center.x + h, center.y + h, center.z + h, 1},  // 6
        vec4{center.x - h, center.y + h, center.z + h, 1}   // 7
    };
}

const int cubeEdges[12][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0},  // bottom face
    {4, 5}, {5, 6}, {6, 7}, {7, 4},  // top face
    {0, 4}, {1, 5}, {2, 6}, {3, 7}   // vertical edges
};
