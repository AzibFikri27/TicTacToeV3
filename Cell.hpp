#pragma once

#include "Shape.hpp"
#include <winbgim.h>

class Cell {
private:
    // Attributes
    int x, y, size;
    Shape* shape;

public:
    // Constructors and destructor
    Cell();
    ~Cell();

    // Mutators
    void set(int x, int y, int size);
    void setShape(Shape* newShape);

    // Accessor
    Shape* getShape() const;

    // Rendering Method
    void draw();

    // Gameplay Methods
    bool isClicked(int clickX, int clickY);
    bool hasShape() const;
};