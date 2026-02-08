#include <iostream>
#include <cassert>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <sfml-3d/3d_engine.hpp>
#include <sfml-3d/math4.hpp>
#include <sfml-3d/3d_camera.hpp>

// Helper function for float comparison
bool floatEqual(float a, float b, float epsilon = 0.001f) {
    return std::abs(a - b) < epsilon;
}

// Test normalize_point
void test_normalize_point() {
    std::cout << "Testing normalize_point..." << std::endl;
    
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    
    // Center point (0, 0) should map to window center
    sf::Vector2f normalized = normalize_point(window, {0.0f, 0.0f});
    assert(floatEqual(normalized.x, 400.0f));
    assert(floatEqual(normalized.y, 300.0f));
    
    // Test positive x, positive y
    normalized = normalize_point(window, {100.0f, 100.0f});
    assert(floatEqual(normalized.x, 500.0f));
    assert(floatEqual(normalized.y, 200.0f));
    
    // Test negative x, negative y
    normalized = normalize_point(window, {-100.0f, -100.0f});
    assert(floatEqual(normalized.x, 300.0f));
    assert(floatEqual(normalized.y, 400.0f));
    
    window.close();
    std::cout << "  ✓ normalize_point tests passed" << std::endl;
}

// Test Line2D
void test_line2d() {
    std::cout << "Testing Line2D..." << std::endl;
    
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    
    Line2D line({0.0f, 0.0f}, {100.0f, 100.0f}, 2.0f);
    
    // Test basic properties
    assert(floatEqual(line.a.x, 0.0f));
    assert(floatEqual(line.a.y, 0.0f));
    assert(floatEqual(line.b.x, 100.0f));
    assert(floatEqual(line.b.y, 100.0f));
    assert(floatEqual(line.thickness, 2.0f));
    
    // Test collision with point on line
    assert(line.computeCollisionWithPoint({50.0f, 50.0f}));
    
    // Test collision with point near line (within thickness + 20)
    assert(line.computeCollisionWithPoint({50.0f, 60.0f}));
    
    // Test no collision with distant point
    assert(!line.computeCollisionWithPoint({200.0f, 200.0f}));
    
    window.close();
    std::cout << "  ✓ Line2D tests passed" << std::endl;
}

// Test Circle2D
void test_circle2d() {
    std::cout << "Testing Circle2D..." << std::endl;
    
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    
    Circle2D circle({100.0f, 100.0f}, 50.0f);
    
    // Test basic properties
    assert(floatEqual(circle.center.x, 100.0f));
    assert(floatEqual(circle.center.y, 100.0f));
    assert(floatEqual(circle.radius, 50.0f));
    
    // Test collision with point at center
    assert(circle.computeCollisionWithPoint({100.0f, 100.0f}));
    
    // Test collision with point on edge
    assert(circle.computeCollisionWithPoint({150.0f, 100.0f}));
    
    // Test collision with point inside
    assert(circle.computeCollisionWithPoint({110.0f, 110.0f}));
    
    // Test no collision with point outside
    assert(!circle.computeCollisionWithPoint({200.0f, 200.0f}));
    
    window.close();
    std::cout << "  ✓ Circle2D tests passed" << std::endl;
}

// Test Object3D::convert_3d_to_2d
void test_convert_3d_to_2d() {
    std::cout << "Testing Object3D::convert_3d_to_2d..." << std::endl;
    
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    Camera camera(window, 60.0f, 0.001f, 500.0f);
    
    // Point directly in front
    vec4 point1(0.0f, 0.0f, 100.0f, 1.0f);
    sf::Vector2f projected1 = Object3D::convert_3d_to_2d(point1, camera);
    assert(floatEqual(projected1.x, 0.0f));
    assert(floatEqual(projected1.y, 0.0f));
    
    // Point to the right
    vec4 point2(10.0f, 0.0f, 100.0f, 1.0f);
    sf::Vector2f projected2 = Object3D::convert_3d_to_2d(point2, camera);
    assert(floatEqual(projected2.x, 50.0f)); // 500 * 10 / 100
    assert(floatEqual(projected2.y, 0.0f));
    
    // Point above
    vec4 point3(0.0f, 10.0f, 100.0f, 1.0f);
    sf::Vector2f projected3 = Object3D::convert_3d_to_2d(point3, camera);
    assert(floatEqual(projected3.x, 0.0f));
    assert(floatEqual(projected3.y, 50.0f));
    
    // Point closer (should project larger)
    vec4 point4(10.0f, 0.0f, 50.0f, 1.0f);
    sf::Vector2f projected4 = Object3D::convert_3d_to_2d(point4, camera);
    assert(floatEqual(projected4.x, 100.0f)); // 500 * 10 / 50
    
    window.close();
    std::cout << "  ✓ convert_3d_to_2d tests passed" << std::endl;
}

// Test polymorphism
void test_polymorphism() {
    std::cout << "Testing polymorphism..." << std::endl;
    
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    
    // Test Shape2D polymorphism
    std::unique_ptr<Shape2D> shape1 = std::make_unique<Line2D>(
        sf::Vector2f{0.0f, 0.0f}, sf::Vector2f{100.0f, 100.0f}, 2.0f);
    std::unique_ptr<Shape2D> shape2 = std::make_unique<Circle2D>(
        sf::Vector2f{100.0f, 100.0f}, 50.0f);
    
    // Verify we can call virtual methods through base pointer
    assert(shape1->computeCollisionWithPoint({50.0f, 50.0f}));
    assert(shape2->computeCollisionWithPoint({100.0f, 100.0f}));
    
    window.close();
    std::cout << "  ✓ Polymorphism tests passed" << std::endl;
}

int main() {
    std::cout << "Running 3d_engine.hpp tests..." << std::endl;
    std::cout << std::endl;
    
    try {
        test_normalize_point();
        test_line2d();
        test_circle2d();
        test_convert_3d_to_2d();
        test_polymorphism();
        
        std::cout << std::endl;
        std::cout << "✓ All tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "✗ Test failed with unknown exception" << std::endl;
        return 1;
    }
}