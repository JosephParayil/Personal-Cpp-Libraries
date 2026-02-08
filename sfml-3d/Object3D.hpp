#pragma once

#include <array>
#include <utility>
#include <optional>
#include <limits>

#include <SFML/Graphics.hpp>
#include "3d_camera.hpp"
#include "math4.hpp"
#include <sfml-util/sfml_util.hpp>



struct Object3D {
    static sf::Vector2f convert_3d_to_2d(vec4 point_3d, const Camera& camera) {
        return {camera.FOV * point_3d.x / point_3d.z, camera.FOV * point_3d.y / point_3d.z};
    }

    //Distance represents distance to the camera; used for depth sorting
    float distance;
    bool distance_updated = false;
    
    virtual float calculateDistance(const Camera& camera) = 0;
    float getDistance(const Camera& camera) {
        if (!distance_updated) {
            distance = calculateDistance(camera);
        }
        return distance;
    }


    virtual ~Object3D() = default;

    virtual std::unique_ptr<Shape2D> computeShape(sf::RenderWindow& window, const Camera& camera) = 0;

    void draw(sf::RenderWindow& window, const Camera& camera, sf::Color color = sf::Color::White) {
        if (auto shape = computeShape(window, camera)) {
            shape->draw(window, color);
        }
    }
};


struct Line3D : Object3D {
    vec4 a, b;
    float thickness;

    Line3D() = default;
    Line3D(vec4 start, vec4 end, float t = 1.0f) : a(start), b(end), thickness(t) {}

    float calculateDistance(const Camera& camera) override {
        const int LINE_RESOLUTION = 3;

        float min_dist = std::numeric_limits<float>::max();
        for (float i=0.0f; i <= 1; i += 1.0f/LINE_RESOLUTION) {
            vec4 intermediate = ((b-a)*i)+a;
            float new_dist = (camera.cf.get_position()-intermediate).magnitude();
            if (new_dist < min_dist) {
                min_dist = new_dist;
            }
        }
        return min_dist;
    }

    std::unique_ptr<Shape2D> computeShape(sf::RenderWindow& window, const Camera& camera) override {
        mat4 camera_inverse = camera.cf.inverse_rigid();
        vec4 a_t = camera_inverse* a;
        vec4 b_t = camera_inverse * b;

        if (a_t.z <= 0 && b_t.z <= 0)
            return nullptr;

        if (a_t.z <= 0) {
            float t = (NEAR-a_t.z) / (b_t.z - a_t.z);

            a_t.x = a_t.x + t * (b_t.x - a_t.x);
            a_t.y = a_t.y + t * (b_t.y - a_t.y);
            a_t.z = NEAR;
        }

        if (b_t.z <= 0) {
            float t = (NEAR-a_t.z) / (b_t.z - a_t.z);

            b_t.x = a_t.x + t * (b_t.x - a_t.x);
            b_t.y = a_t.y + t * (b_t.y - a_t.y);
            b_t.z = NEAR;
        }


        sf::Vector2f a_ = normalize_point(window, convert_3d_to_2d(a_t, camera));
        sf::Vector2f b_ = normalize_point(window, convert_3d_to_2d(b_t, camera));

        return std::make_unique<Line2D>(a_, b_, thickness);
    }
};

struct Sphere3D : Object3D {
    vec4 position;
    float radius;

    Sphere3D() = default;
    Sphere3D(vec4 pos, float r) : position(pos), radius(r) {}

    float calculateDistance(const Camera& camera) override {
        float center_dist = (camera.cf.get_position()-position).magnitude();
        return center_dist-radius;
    }

    std::unique_ptr<Shape2D> computeShape(sf::RenderWindow& window, const Camera& camera) override {
        mat4 camera_inverse = camera.cf.inverse_rigid();
        vec4 view_pos = camera_inverse * position;

        if (view_pos.z <= NEAR) 
            return nullptr;

        float projected_radius = camera.FOV * radius / view_pos.z;
        
        sf::Vector2f screen_pos = normalize_point(window, convert_3d_to_2d(view_pos, camera));

        return std::make_unique<Circle2D>(screen_pos, projected_radius);
    }
};