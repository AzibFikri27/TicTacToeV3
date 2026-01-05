#pragma once

#include "Shape.hpp"
#include <winbgim.h>

class Cell {
private:
    int x, y, size;
    Shape* shape;

public:
    Cell();
    ~Cell();

    void set(int x, int y, int size);
    void setShape(Shape* newShape);

    Shape* getShape() const;

    void draw();

    bool isClicked(int clickX, int clickY);
    bool hasShape() const;
};