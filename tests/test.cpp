/*
Depth Sorting Test - Random 3D Objects
Demonstrates depth sorting with spheres and lines

Controls:
- WASD: Move camera horizontally
- Q/E: Move camera up/down
- Mouse/Arrow keys: Look around
- T: Toggle depth sorting on/off
- G: Generate new random objects
- C: Toggle colors vs white
- Space: Pause/unpause animation
- ESC: Toggle mouse lock

SFML 3.0
*/

#include <iostream>
#include <cassert>
#include <cmath>
#include <memory>
#include <vector>
#include <random>
#include <SFML/Graphics.hpp>
#include <sfml-3d/3d_engine.hpp>
#include <sfml-3d/math4.hpp>
#include <sfml-3d/3d_camera.hpp>

// Structure to hold object info
struct ObjectInfo {
    Object3D* obj;
    sf::Color color;
    int id;
};

// Global object storage (for cleanup)
std::vector<ObjectInfo> allObjects;

void cleanupObjects() {
    for (auto& info : allObjects) {
        delete info.obj;
    }
    allObjects.clear();
}

void generateRandomObjects(Object3D_Collection& collection, int numObjects = 50) {
    // Clean up existing objects
    cleanupObjects();
    collection.c.clear();

    // Random number generators
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::uniform_real_distribution<float> pos_dist(-200.0f, 200.0f);
    std::uniform_real_distribution<float> radius_dist(3.0f, 25.0f);
    std::uniform_real_distribution<float> thickness_dist(1.5f, 6.0f);
    std::uniform_int_distribution<int> color_dist(80, 255);
    std::uniform_int_distribution<int> type_dist(0, 1); // 0 = sphere, 1 = line

    for (int i = 0; i < numObjects; i++) {
        int type = type_dist(gen);
        
        // Generate random color
        sf::Color color(
            color_dist(gen),
            color_dist(gen),
            color_dist(gen)
        );

        Object3D* obj = nullptr;

        if (type == 0) {
            // Create sphere
            vec4 pos(pos_dist(gen), pos_dist(gen), pos_dist(gen));
            float radius = radius_dist(gen);
            obj = new Sphere3D(pos, radius);
        } else {
            // Create line
            vec4 start(pos_dist(gen), pos_dist(gen), pos_dist(gen));
            vec4 end(pos_dist(gen), pos_dist(gen), pos_dist(gen));
            float thickness = thickness_dist(gen);
            obj = new Line3D(start, end, thickness);
        }

        // Store object info
        ObjectInfo info;
        info.obj = obj;
        info.color = color;
        info.id = i;
        allObjects.push_back(info);

        // Add to collection
        collection.c.push_back({i, obj});
    }

    std::cout << "Generated " << numObjects << " random objects" << std::endl;
}

// Add some animated objects for extra visual interest
void updateAnimatedObjects(Object3D_Collection& collection, float time) {
    // Optional: Add some simple animation to make depth sorting more obvious
    // For now, keeping objects static for clearer demonstration
}

void drawHUD(sf::RenderWindow& window, bool depthSortEnabled, bool useColors, 
             int numObjects, const Camera& camera) {
    
    // Draw semi-transparent background panel
    sf::RectangleShape background({300.0f, 100.0f});
    background.setPosition({10.0f, 10.0f});
    background.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(background);

    // Draw status indicators using colored rectangles
    float yPos = 25.0f;
    float xPos = 20.0f;
    
    // Depth Sort indicator
    sf::RectangleShape depthSortBar({150.0f, 20.0f});
    depthSortBar.setPosition({xPos, yPos});
    depthSortBar.setFillColor(depthSortEnabled ? sf::Color::Green : sf::Color::Red);
    window.draw(depthSortBar);
    
    // Colors indicator
    yPos += 30.0f;
    sf::RectangleShape colorsBar({150.0f, 20.0f});
    colorsBar.setPosition({xPos, yPos});
    colorsBar.setFillColor(useColors ? sf::Color::Yellow : sf::Color(100, 100, 100));
    window.draw(colorsBar);
    
    // Object count indicator (represented by filled bars)
    yPos += 30.0f;
    float barWidth = (numObjects / 100.0f) * 150.0f;  // Scale to 100 max objects
    sf::RectangleShape countBar({barWidth, 20.0f});
    countBar.setPosition({xPos, yPos});
    countBar.setFillColor(sf::Color::Cyan);
    window.draw(countBar);

    // Draw status indicator dot in corner
    sf::CircleShape indicator(8.0f);
    indicator.setPosition({window.getSize().x - 30.0f, 20.0f});
    indicator.setFillColor(depthSortEnabled ? sf::Color::Green : sf::Color::Red);
    window.draw(indicator);
}

int main() {
    const int HEIGHT = 1000;
    const int WIDTH = 1600;
    
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Depth Sorting Test - 3D Engine");

    // Create camera
    Camera camera(window, 60.0f, 0.001f, 5.0f, 100.0f, 2.0f);
    
    // Position camera for good initial view
    camera.cf = mat4::translation(0, 0, -400);

    // Create object collection
    Object3D_Collection collection;

    // Generate initial random objects
    const int NUM_OBJECTS = 60;
    generateRandomObjects(collection, NUM_OBJECTS);

    // State variables
    bool depthSortEnabled = true;
    bool useColors = true;
    bool paused = false;
    float animationTime = 0.0f;

    std::cout << "\n=== Depth Sorting Test ===" << std::endl;
    std::cout << "Press T to toggle depth sorting and see the difference!" << std::endl;
    std::cout << "Move around with WASD/QE to see sorting from different angles" << std::endl;
    std::cout << "\nWhen sorting is OFF, you'll see visual artifacts:" << std::endl;
    std::cout << "- Far objects appearing in front of near ones" << std::endl;
    std::cout << "- Incorrect layering and overlaps\n" << std::endl;

    size_t frame = 0;
    sf::Clock clock;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        
        // Event handling
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scan::T) {
                    depthSortEnabled = !depthSortEnabled;
                    std::cout << "Depth sorting: " << (depthSortEnabled ? "ON" : "OFF") << std::endl;
                }
                else if (keyPressed->scancode == sf::Keyboard::Scan::G) {
                    std::cout << "Generating new random objects..." << std::endl;
                    generateRandomObjects(collection, NUM_OBJECTS);
                }
                else if (keyPressed->scancode == sf::Keyboard::Scan::C) {
                    useColors = !useColors;
                    std::cout << "Colors: " << (useColors ? "ON" : "OFF") << std::endl;
                }
                else if (keyPressed->scancode == sf::Keyboard::Scan::Space) {
                    paused = !paused;
                    std::cout << "Animation: " << (paused ? "PAUSED" : "RUNNING") << std::endl;
                }
            }
            
            camera.handleEvent(event);
        }

        // Update
        camera.update();
        
        if (!paused) {
            animationTime += deltaTime;
            updateAnimatedObjects(collection, animationTime);
        }

        // Render
        window.clear(sf::Color(20, 20, 30)); // Dark blue background

        // DEPTH SORT if enabled
        if (depthSortEnabled) {
            collection.depthSort(camera);
        }

        // Draw all objects
        for (size_t i = 0; i < collection.c.size(); i++) {
            auto& pair = collection.c[i];
            Object3D* obj = pair.second;
            int id = pair.first;
            
            // Find corresponding color
            sf::Color color = sf::Color::White;
            if (useColors && id < allObjects.size()) {
                color = allObjects[id].color;
            }
            
            obj->draw(window, camera, color);
        }

        /*
        // Draw reference axes at origin
        vec4 origin(0, 0, 0);
        draw3DLine(window, origin, vec4(50, 0, 0), camera, 3.0f);  // X axis - red drawn separately
        draw3DLine(window, origin, vec4(0, 50, 0), camera, 3.0f);  // Y axis - green drawn separately
        draw3DLine(window, origin, vec4(0, 0, 50), camera, 3.0f);  // Z axis - blue drawn separately
        

        // Draw a central reference sphere
        drawSphere(window, origin, 5.0f, camera, sf::Color(255, 255, 0, 150));
        */
        // Draw HUD
        drawHUD(window, depthSortEnabled, useColors, NUM_OBJECTS, camera);

        // Draw crosshair if needed
        camera.drawCrosshairIfNeeded(window);

        window.display();

        frame++;
    }

    // Cleanup
    cleanupObjects();

    return 0;
}