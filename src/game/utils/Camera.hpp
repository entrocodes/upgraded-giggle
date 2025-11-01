#pragma once
#include "../math/Vec2.hpp"
#include "../math/TableHomography.hpp"
#include <SFML/Graphics.hpp>

class Camera {
public:
    Vec2 position = Vec2(320,320);
    float zoom = 1.0f;
    TableHomography homography;

    Camera() = default;

    // Compute an SFML view from camera (still needed for window)
    sf::View getView(const Vec2& windowSize) const {
        sf::View view;
        view.setSize(windowSize.x, windowSize.y);
        view.setCenter(position.x, position.y);
        view.zoom(1.0f / zoom); // Zoom > 1 = zoom in
        return view;
    }

    // Transform a world point to screen coordinates using homography and camera
    Vec2 worldToScreen(const Vec2& worldPos, const Vec2& windowSize) const {
        //Vec2 p = homography.worldToImage(worldPos);
        //p = (p - position) * zoom + windowSize * 0.5f; // center and scale
        //return p;
        Vec2 p = (worldPos - position) * zoom + windowSize * 0.5f;
        return p;
    }

    // Optional: inverse, screen -> world
    Vec2 screenToWorld(const Vec2& screenPos, const Vec2& windowSize) const {
        //Vec2 p = (screenPos - windowSize * 0.5f) / zoom + position;
        //return homography.imageToWorld(p);
        Vec2 p = (screenPos - windowSize * 0.5f) / zoom + position;
        return p;
    }
    //Vec2 worldToScreenTest(const Vec2& worldPos, const Vec2& windowSize) const {
    //    Vec2 p = homography.worldToImage(worldPos);
    //    p = (p - position) * zoom + windowSize * 0.5f; // center and scale
    //    return p;
    //}

    //// Optional: inverse, screen -> world
    //Vec2 screenToWorldTest(const Vec2& screenPos, const Vec2& windowSize) const {
    //    Vec2 p = (screenPos - windowSize * 0.5f) / zoom + position;
    //    return homography.imageToWorld(p);
    //}
};
