#include "TableHomography.hpp"
#include "debug/Debug.hpp"
#include "math/Vec2.hpp"
#include <SFML/Graphics.hpp>
#include <Eigen/Dense>
#include <cmath>
#include <iostream>



// Compute full perspective homography using Eigen
static Eigen::Matrix3f computeHomographyEigen(const std::array<Vec2, 4>& src, const std::array<Vec2, 4>& dst) {
    Eigen::Matrix<float, 8, 8> A;
    Eigen::Matrix<float, 8, 1> b;

    for (int i = 0; i < 4; ++i) {
        float x = src[i].x;
        float y = src[i].y;
        float X = dst[i].x;
        float Y = dst[i].y;

        A(2 * i, 0) = x;
        A(2 * i, 1) = y;
        A(2 * i, 2) = 1.0f;
        A(2 * i, 3) = 0.0f;
        A(2 * i, 4) = 0.0f;
        A(2 * i, 5) = 0.0f;
        A(2 * i, 6) = -x * X;
        A(2 * i, 7) = -y * X;
        b(2 * i, 0) = X;

        A(2 * i + 1, 0) = 0.0f;
        A(2 * i + 1, 1) = 0.0f;
        A(2 * i + 1, 2) = 0.0f;
        A(2 * i + 1, 3) = x;
        A(2 * i + 1, 4) = y;
        A(2 * i + 1, 5) = 1.0f;
        A(2 * i + 1, 6) = -x * Y;
        A(2 * i + 1, 7) = -y * Y;
        b(2 * i + 1, 0) = Y;
    }

    Eigen::Matrix<float, 8, 1> h = A.colPivHouseholderQr().solve(b);

    Eigen::Matrix3f H;
    H << h(0), h(1), h(2),
        h(3), h(4), h(5),
        h(6), h(7), 1.0f;

    return H;
}

void TableHomography::calibrate(const std::array<Vec2, 4>& src, const std::array<Vec2, 4>& dst) {
    srcPoints = src;
    dstPoints = dst;

    Debug::debugPrint("=== TableHomography::calibrate ===");
    for (int i = 0; i < 4; ++i) {
        Debug::debugPrint("src[" + std::to_string(i) + "]", src[i]);
        Debug::debugPrint("dst[" + std::to_string(i) + "]", dst[i]);
    }

    // Compute Eigen homography
    H_eigen = computeHomographyEigen(src, dst);
    H_inv_eigen = H_eigen.inverse();

    // Convert to sf::Transform for drawing
    sf::Transform H_sf(
        H_eigen(0, 0), H_eigen(0, 1), H_eigen(0, 2),
        H_eigen(1, 0), H_eigen(1, 1), H_eigen(1, 2),
        H_eigen(2, 0), H_eigen(2, 1), H_eigen(2, 2)
    );

    H = H_sf;
    H_inv = H_sf.getInverse();

    Debug::debugPrint("Computed Homography (Eigen):", "--------------------------------");
    std::cout << H_eigen << std::endl;

    float det = H_eigen.determinant();
    Debug::debugPrint("Homography determinant", std::to_string(det));

    if (std::isnan(det) || std::abs(det) < 1e-6f)
        Debug::debugPrint("Warning", "Degenerate or invalid homography matrix!");

    calibrated = true;
    Debug::debugPrint("Calibration complete", "True perspective homography initialized");
}

Vec2 TableHomography::imageToWorld(const Vec2& p) const {
    Eigen::Vector3f pi(p.x, p.y, 1.0f);
    Eigen::Vector3f pw = H_eigen * pi;
    return Vec2(pw(0) / pw(2), pw(1) / pw(2));
}

Vec2 TableHomography::worldToImage(const Vec2& p) const {
    Eigen::Vector3f pw(p.x, p.y, 1.0f);
    Eigen::Vector3f pi = H_inv_eigen * pw;
    return Vec2(pi(0) / pi(2), pi(1) / pi(2));
}
Vec2 TableHomography::worldToImage(const Vec3& p) const {
    Eigen::Vector3f pw(p.x, p.z, 1.0f);
    Eigen::Vector3f pi = H_inv_eigen * pw;
    float yOffset = p.y * m_pixelsPerMeterY; //test with new Y setting
    //return Vec2(pi(0) / pi(2), pi(1) / pi(2) - yOffset);
    return Vec2(pi(0) / pi(2), pi(1) / pi(2) - yOffset);
}

void TableHomography::drawDebugGrid(sf::RenderWindow& window, int divX, int divY) const{
    if (!calibrated) {
        if (printDebug) {
            Debug::debugPrint("TableHomography", "not calibrated");
        }
        return;
    }

    sf::VertexArray grid(sf::Lines);

    float tableXRange = 1.525f; // Table width (meters)
    float tableZRange = 2.74f;  // Table length (meters)
    if (printDebug) {
        Debug::debugPrint("Table size (X,Z)", Vec2(tableXRange, tableZRange));
    }
    bool drewSomething = false;

    for (int i = 0; i <= divX; i++) {
        float t = i / float(divX);
        Vec2 start{ t * tableXRange, 0.0f };
        Vec2 end{ t * tableXRange, tableZRange };
        Vec2 startScreen = worldToImage(start);
        Vec2 endScreen = worldToImage(end);

        if ((i == 0 || i == divX) && printDebug) {
            Debug::debugPrint("Vertical Line Start", startScreen);
            Debug::debugPrint("Vertical Line End", endScreen);
        }

        if (std::isfinite(startScreen.x) && std::isfinite(startScreen.y) &&
            std::isfinite(endScreen.x) && std::isfinite(endScreen.y)) {

            grid.append(sf::Vertex({ startScreen.x, startScreen.y }, sf::Color(0, 255, 0, 120)));
            grid.append(sf::Vertex({ endScreen.x, endScreen.y }, sf::Color(0, 255, 0, 120)));
            drewSomething = true;
        }
    }

    for (int j = 0; j <= divY; j++) {
        float t = j / float(divY);
        Vec2 start{ 0.0f, t * tableZRange };
        Vec2 end{ tableXRange, t * tableZRange };

        Vec2 startScreen = worldToImage(start);
        Vec2 endScreen = worldToImage(end);

        if ((j == 0 || j == divY) && printDebug) {
            Debug::debugPrint("Horizontal Line Start", startScreen);
            Debug::debugPrint("Horizontal Line End", endScreen);
        }

        if (std::isfinite(startScreen.x) && std::isfinite(startScreen.y) &&
            std::isfinite(endScreen.x) && std::isfinite(endScreen.y)) {

            grid.append(sf::Vertex({ startScreen.x, startScreen.y }, sf::Color(0, 255, 0, 120)));
            grid.append(sf::Vertex({ endScreen.x, endScreen.y }, sf::Color(0, 255, 0, 120)));
            drewSomething = true;
        }
    }

    if (drewSomething)
        window.draw(grid);
    printDebug = false;
}
