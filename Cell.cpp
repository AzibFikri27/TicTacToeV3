#include "Cell.hpp"

Cell::Cell() : x(0), y(0), size(0), shape(nullptr) {}

Cell::~Cell() {
    delete shape;
}

void Cell::set(int x, int y, int size) {
    this->x = x;
    this->y = y;
    this->size = size;
}

void Cell::setShape(Shape* newShape) {
    shape = newShape;
}
void Cell::draw() {
    if (shape) {
        shape->draw(x, y, size);
    }
}

bool Cell::isClicked(int clickX, int clickY) {
    return clickX >= x && clickX <= x + size && clickY >= y && clickY <= y + size;
}

bool Cell::hasShape() const {
    return shape != nullptr;
}

Shape* Cell::getShape() const {
    return shape;
}