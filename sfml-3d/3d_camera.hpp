#pragma once
/*
Code for 3D Camera CF movement
*/

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <vector>

#include "math4.hpp"

const float PI = 3.14159265359;

struct Camera {
    float FPS;

    mat4 cf;
    float yaw, pitch;

    
    bool mouseLocked;
    bool allowMouseLocking;
    bool crosshairEnabled;
    float sensitivity;
    sf::Vector2i setRightClickPos;

    float SPEED_NORMAL, SPEED_FAST, SPEED_SLOW;

    float FOV;
    float INITIAL_FOV;
    float INITIAL_SENSITIVITY;

    sf::RenderWindow& window;
    sf::Vector2i windowCenter;

    Camera(sf::RenderWindow& window, float fps = 60.0f,
           float sensitivity = 0.001f, float SPEED_NORMAL = 5.f,
           float SPEED_FAST = 100.f, float SPEED_SLOW = 2.f, float FOV = 500)
        : window(window),
          FPS(fps),
          sensitivity(sensitivity),
          SPEED_NORMAL(SPEED_NORMAL),
          SPEED_FAST(SPEED_FAST),
          SPEED_SLOW(SPEED_SLOW),
          FOV(FOV) {
        window.setFramerateLimit(FPS);

        mouseLocked = false;
        allowMouseLocking = true;
        crosshairEnabled = true;
        setRightClickPos = sf::Mouse::getPosition(window);
        // CURSOR_DEFAULT =
        // sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow).value();
        // CURSOR_CROSSHAIR =
        // sf::Cursor::createFromSystem(sf::Cursor::Type::Cross).value();

        cf = mat4::translation(0, 0, -200);
        yaw = pitch = 0.0f;

        INITIAL_FOV = FOV;
        INITIAL_SENSITIVITY = sensitivity;

        unsigned int WIDTH = window.getSize().x;
        unsigned int HEIGHT = window.getSize().y;

        windowCenter = sf::Vector2i(WIDTH / 2.0f, HEIGHT / 2.0f);
    }

    void handleEvent(const std::optional<sf::Event>& event) {
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->scancode == sf::Keyboard::Scan::Escape) {
                if (allowMouseLocking) mouseLocked = !mouseLocked;
                else mouseLocked = false;
                
                // window.setMouseCursorVisible(!mouseLocked);

            } else if (keyPressed->scancode == sf::Keyboard::Scan::R) {
                FOV = INITIAL_FOV;
                sensitivity = INITIAL_SENSITIVITY;
            } else if (keyPressed->scancode == sf::Keyboard::Scan::H) {
                crosshairEnabled = !crosshairEnabled;
            }
        } else if (const auto* mouseWheelScrolled =
                       event->getIf<sf::Event::MouseWheelScrolled>()) {
            // event.mouseWheelScroll.delta gives the scroll amount
            // Positive delta means scrolling up/left, negative means down/right
            float delta = mouseWheelScrolled->delta;

            if (delta > 0) {
                FOV *= 1.2;
                sensitivity /= 1.2;
            } else {
                FOV /= 1.2;
                sensitivity *= 1.2;
            }

        } else if (const auto* mouseButtonPressed =
                       event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButtonPressed->button == sf::Mouse::Button::Right) {
                setRightClickPos = sf::Mouse::getPosition(window);
            }
        }
    }

    void update() {
        float camera_speed = SPEED_NORMAL;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl))
            camera_speed = SPEED_FAST;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift))
            camera_speed = SPEED_SLOW;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W))
            cf = cf * mat4::translation(0, 0, camera_speed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S))
            cf = cf * mat4::translation(0, 0, -camera_speed);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A))
            cf = cf * mat4::translation(-camera_speed, 0, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D))
            cf = cf * mat4::translation(camera_speed, 0, 0);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::E))
            cf = cf * mat4::translation(0, camera_speed, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Q))
            cf = cf * mat4::translation(0, -camera_speed, 0);

        /*
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) cf = cf
        *mat4::rotation_y(-1/FPS); if
        (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) cf = cf
        *mat4::rotation_y(1/FPS);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) cf = cf
        *mat4::rotation_x(-1/FPS); if
        (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) cf = cf
        *mat4::rotation_x(1/FPS);
        */

        bool rightDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
        if (mouseLocked || rightDown) {
            window.setMouseCursorVisible(false);
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2i delta;

            if (mouseLocked) {
                delta = mousePos - windowCenter;
            } else {  // rightDown = true
                delta = mousePos - setRightClickPos;
            }

            yaw += delta.x * sensitivity;
            pitch += delta.y * sensitivity;
            pitch = std::clamp(pitch, -PI / 2, PI / 2);
            if (mouseLocked)
                sf::Mouse::setPosition(windowCenter, window);
            else
                sf::Mouse::setPosition(setRightClickPos, window);
        } else {
            window.setMouseCursorVisible(true);
        }

        cf = cf.nullify_rotation() * mat4::rotation_y(yaw) *
             mat4::rotation_x(pitch);

        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) cf = cf
        // *mat4::rotation_z(-1/FPS); if
        // (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) cf = cf
        // *mat4::rotation_z(1/FPS);
    }

    void drawCrosshairIfNeeded(sf::RenderWindow& window) {
        bool rightDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
        if (!crosshairEnabled || !(mouseLocked || rightDown)) return;

        float size = 20.f;
        float thickness = 2.f;

        sf::Vector2i mousePos = windowCenter;
        if (rightDown) mousePos = setRightClickPos;

        // Horizontal bar
        sf::RectangleShape hBar({size, thickness});
        hBar.setPosition(
            {mousePos.x - size / 2.f, mousePos.y - thickness / 2.f});
        hBar.setFillColor(sf::Color::White);

        // Vertical bar
        sf::RectangleShape vBar({thickness, size});
        vBar.setPosition(
            {mousePos.x - thickness / 2.f, mousePos.y - size / 2.f});
        vBar.setFillColor(sf::Color::White);

        window.draw(hBar);
        window.draw(vBar);
    }
};
