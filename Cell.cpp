#include "Cell.hpp"

// Constructor: Initializes a Cell object with default values.
// Sets position (x, y) and size to 0, and the shape to nullptr.
Cell::Cell() : x(0), y(0), size(0), shape(nullptr) {}

// Destructor: Cleans up memory by deleting the shape associated with the cell.
Cell::~Cell() {
    delete shape;
}

// set: Sets the position and size of the cell.
//  x: The x-coordinate of the cell.
//  y: The y-coordinate of the cell.
// size: The size of the cell.
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