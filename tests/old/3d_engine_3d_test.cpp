#include <iostream>
#include <cassert>
#include <cmath>
#include <memory>
#include <SFML/Graphics.hpp>
#include <sfml-3d/3d_engine.hpp>
#include <sfml-3d/math4.hpp>
#include <sfml-3d/3d_camera.hpp>

// Helper function for float comparison
bool floatEqual(float a, float b, float epsilon = 0.001f) {
    return std::abs(a - b) < epsilon;
}

// Test Line3D construction and position calculation
void test_line3d_construction() {
    std::cout << "Testing Line3D construction..." << std::endl;
    
    vec4 start(0.0f, 0.0f, 0.0f, 1.0f);
    vec4 end(100.0f, 100.0f, 100.0f, 1.0f);
    Line3D line(start, end, 2.0f);
    
    // Test basic properties
    assert(floatEqual(line.a.x, 0.0f));
    assert(floatEqual(line.a.y, 0.0f));
    assert(floatEqual(line.a.z, 0.0f));
    assert(floatEqual(line.b.x, 100.0f));
    assert(floatEqual(line.b.y, 100.0f));
    assert(floatEqual(line.b.z, 100.0f));
    assert(floatEqual(line.thickness, 2.0f));
    
    // Test position (should be midpoint)
    assert(floatEqual(line.position.x, 50.0f));
    assert(floatEqual(line.position.y, 50.0f));
    assert(floatEqual(line.position.z, 50.0f));
    
    std::cout << "  ✓ Line3D construction tests passed" << std::endl;
}

// Test Line3D projection when both points are in front of camera
void test_line3d_projection_visible() {
    std::cout << "Testing Line3D projection (visible)..." << std::endl;
    
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    Camera camera(window, 60.0f, 0.001f, 500.0f);
    
    // Create a line in front of the camera (z > 0 in camera space)
    vec4 start(0.0f, 0.0f, 100.0f, 1.0f);
    vec4 end(50.0f, 50.0f, 100.0f, 1.0f);
    Line3D line(start, end, 2.0f);
    
    auto shape = line.computeShape(window, camera);
    
    // Shape should be created (both points visible)
    assert(shape != nullptr);
    
    // Should be a Line2D
    Line2D* line2d = dynamic_cast<Line2D*>(shape.get());
    assert(line2d != nullptr);
    
    window.close();
    std::cout << "  ✓ Line3D visible projection tests passed" << std::endl;
}

// Test Line3D projection when both points are behind camera
void test_line3d_projection_behind() {
    std::cout << "Testing Line3D projection (behind camera)..." << std::endl;
    
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    Camera camera(window);
    
    // Create a line behind the camera (z < 0 in camera space)
    vec4 start(0.0f, 0.0f, -300.0f);
    vec4 end(50.0f, 50.0f, -300.0f);
    Line3D line(start, end, 2.0f);
    
    auto shape = line.computeShape(window, camera);
    
    // Shape should be null (both points behind camera)
    assert(shape == nullptr);
    
    window.close();
    std::cout << "  ✓ Line3D behind camera tests passed" << std::endl;
}

// Test Line3D projection when one point is behind camera (clipping)
void test_line3d_projection_clipping() {
    std::cout << "Testing Line3D projection (clipping)..." << std::endl;
    
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    Camera camera(window, 60.0f, 0.001f, 500.0f);
    
    // Create a line with one point behind and one in front
    vec4 start(0.0f, 0.0f, -50.0f, 1.0f);
    vec4 end(0.0f, 0.0f, 100.0f, 1.0f);
    Line3D line(start, end, 2.0f);
    
    auto shape = line.computeShape(window, camera);
    
    // Shape should still be created (clipped to NEAR plane)
    assert(shape != nullptr);
    
    // Should be a Line2D
    Line2D* line2d = dynamic_cast<Line2D*>(shape.get());
    assert(line2d != nullptr);
    
    window.close();
    std::cout << "  ✓ Line3D clipping tests passed" << std::endl;
}

// Test Sphere3D construction
void test_sphere3d_construction() {
    std::cout << "Testing Sphere3D construction..." << std::endl;
    
    vec4 center(10.0f, 20.0f, 30.0f, 1.0f);
    Sphere3D sphere(center, 15.0f);
    
    // Test basic properties
    assert(floatEqual(sphere.position.x, 10.0f));
    assert(floatEqual(sphere.position.y, 20.0f));
    assert(floatEqual(sphere.position.z, 30.0f));
    assert(floatEqual(sphere.radius, 15.0f));
    
    std::cout << "  ✓ Sphere3D construction tests passed" << std::endl;
}

// Test Sphere3D projection when in front of camera
void test_sphere3d_projection_visible() {
    std::cout << "Testing Sphere3D projection (visible)..." << std::endl;
    
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    Camera camera(window, 60.0f, 0.001f, 500.0f);
    
    // Create a sphere in front of the camera
    vec4 center(0.0f, 0.0f, -100.0f, 1.0f);
    Sphere3D sphere(center, 10.0f);
    
    auto shape = sphere.computeShape(window, camera);
    
    // Shape should be created
    assert(shape != nullptr);
    
    // Should be a Circle2D
    Circle2D* circle = dynamic_cast<Circle2D*>(shape.get());
    assert(circle != nullptr);
    
    // Check projected radius calculation: FOV * radius / z
    float expected_radius = 500.0f * 10.0f / 100.0f; // 50.0f
    assert(floatEqual(circle->radius, expected_radius));
    
    window.close();
    std::cout << "  ✓ Sphere3D visible projection tests passed" << std::endl;
}

// Test Sphere3D projection when behind camera
void test_sphere3d_projection_behind() {
    std::cout << "Testing Sphere3D projection (behind camera)..." << std::endl;
    
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    Camera camera(window, 60.0f, 0.001f, 500.0f);
    
    // Create a sphere behind the camera
    vec4 center(0.0f, 0.0f, -300.0f, 1.0f);
    Sphere3D sphere(center, 10.0f);
    
    auto shape = sphere.computeShape(window, camera);
    
    // Shape should be null (behind camera)
    assert(shape == nullptr);
    
    window.close();
    std::cout << "  ✓ Sphere3D behind camera tests passed" << std::endl;
}

// Test Sphere3D projection at NEAR plane boundary
void test_sphere3d_projection_near_plane() {
    std::cout << "Testing Sphere3D projection (NEAR plane)..." << std::endl;
    
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    Camera camera(window, 60.0f, 0.001f, 500.0f);
    
    // Create a sphere at the NEAR plane
    vec4 center(0.0f, 0.0f, NEAR-200.f, 1.0f);
    Sphere3D sphere(center, 5.0f);
    
    auto shape = sphere.computeShape(window, camera);
    
    // Shape should be null (at NEAR plane boundary)
    assert(shape == nullptr);
    
    // Just beyond NEAR should be visible
    vec4 center2(0.0f, 0.0f, NEAR + 0.1f, 1.0f);
    Sphere3D sphere2(center2, 5.0f);
    auto shape2 = sphere2.computeShape(window, camera);
    assert(shape2 != nullptr);
    
    window.close();
    std::cout << "  ✓ Sphere3D NEAR plane tests passed" << std::endl;
}

// Test Sphere3D perspective scaling (closer = larger)
void test_sphere3d_perspective_scaling() {
    std::cout << "Testing Sphere3D perspective scaling..." << std::endl;
    
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    Camera camera(window, 60.0f, 0.001f, 500.0f);
    
    // Same radius sphere at different distances
    Sphere3D sphere_far(vec4(0.0f, 0.0f, 0.0f, 1.0f), 10.0f);
    Sphere3D sphere_near(vec4(0.0f, 0.0f, -100.0f, 1.0f), 10.0f);
    
    auto shape_far = sphere_far.computeShape(window, camera);
    auto shape_near = sphere_near.computeShape(window, camera);
    
    Circle2D* circle_far = dynamic_cast<Circle2D*>(shape_far.get());
    Circle2D* circle_near = dynamic_cast<Circle2D*>(shape_near.get());
    
    assert(circle_far != nullptr);
    assert(circle_near != nullptr);
    
    // Closer sphere should project larger
    assert(circle_near->radius > circle_far->radius);
    
    // Check ratio: should be 2x (distance ratio is 2:1)
    assert(floatEqual(circle_near->radius / circle_far->radius, 2.0f));
    
    window.close();
    std::cout << "  ✓ Sphere3D perspective scaling tests passed" << std::endl;
}

// Test Object3D polymorphism with Line3D and Sphere3D
void test_3d_polymorphism() {
    std::cout << "Testing 3D object polymorphism..." << std::endl;
    
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    Camera camera(window, 60.0f, 0.001f, 500.0f);
    
    // Create objects through base pointer
    std::unique_ptr<Object3D> obj1 = std::make_unique<Line3D>(
        vec4(0.0f, 0.0f, 100.0f, 1.0f),
        vec4(50.0f, 50.0f, 100.0f, 1.0f),
        2.0f
    );
    
    std::unique_ptr<Object3D> obj2 = std::make_unique<Sphere3D>(
        vec4(0.0f, 0.0f, 100.0f, 1.0f),
        10.0f
    );
    
    // Both should compute shapes successfully
    auto shape1 = obj1->computeShape(window, camera);
    auto shape2 = obj2->computeShape(window, camera);
    
    assert(shape1 != nullptr);
    assert(shape2 != nullptr);
    
    // Verify correct types
    assert(dynamic_cast<Line2D*>(shape1.get()) != nullptr);
    assert(dynamic_cast<Circle2D*>(shape2.get()) != nullptr);
    
    window.close();
    std::cout << "  ✓ 3D polymorphism tests passed" << std::endl;
}

// Test draw method
void test_3d_draw() {
    std::cout << "Testing 3D object draw method..." << std::endl;
    
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    Camera camera(window, 60.0f, 0.001f, 500.0f);
    
    Line3D line(
        vec4(0.0f, 0.0f, 100.0f, 1.0f),
        vec4(50.0f, 50.0f, 100.0f, 1.0f),
        2.0f
    );
    
    Sphere3D sphere(vec4(0.0f, 0.0f, 100.0f, 1.0f), 10.0f);
    
    // These should not crash (actual drawing won't be visible in test)
    line.draw(window, camera, sf::Color::Red);      // Added camera parameter
    sphere.draw(window, camera, sf::Color::Blue);   // Added camera parameter
    
    // Test drawing objects behind camera (should handle gracefully)
    Line3D line_behind(
        vec4(0.0f, 0.0f, -100.0f, 1.0f),
        vec4(50.0f, 50.0f, -100.0f, 1.0f),
        2.0f
    );
    line_behind.draw(window, camera, sf::Color::Green);  // Added camera parameter
    
    window.close();
    std::cout << "  ✓ 3D draw tests passed" << std::endl;
}


int main() {
    std::cout << "Running test_3d.cpp - Testing Line3D and Sphere3D..." << std::endl;
    std::cout << std::endl;
    
    try {
        test_line3d_construction();
        test_line3d_projection_visible();
        test_line3d_projection_behind();
        test_line3d_projection_clipping();
        
        test_sphere3d_construction();
        test_sphere3d_projection_visible();
        test_sphere3d_projection_behind();
        test_sphere3d_projection_near_plane();
        test_sphere3d_perspective_scaling();
        
        test_3d_polymorphism();
        test_3d_draw();
        
        std::cout << std::endl;
        std::cout << "✓ All 3D tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "✗ Test failed with unknown exception" << std::endl;
        return 1;
    }
}