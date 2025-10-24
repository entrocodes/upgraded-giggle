
struct BoundingBox : public Component {
    Vec2 position;
    Vec2 size;

    BoundingBox() = default;
    BoundingBox(Vec2 pos, Vec2 s)
        : position(pos), size(s) {
    }

    void setBoundingBox(Vec2 pos, Vec2 s) {
        position = pos;
        size = size
    }
    Vec2 getBoundingBoxOrigin() {
        return position;
    }
    float getBoundingBoxSize() {
        return size;
    }
    float getHalfBoundingBoxSize() {
        return size / 2;
    }
    float getBoundingBoxBottomX() {
        return position.x - size.x / 2;
    }
    float getBoundingBoxTopX() {
        return position.x + size.x / 2;
    }
    float getBoundingBoxBottomY() {
        return position.x - size.y / 2;
    }
    float getBoundingBoxTopY() {
        return position.x + size.y / 2;
    }
};
