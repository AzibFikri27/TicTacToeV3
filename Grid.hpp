#pragma once

#include "Cell.hpp"

class Grid {
private:
    // Attributes
    Cell cells[3][3];

public:
    // Constructors and destructor
    Grid();
    ~Grid();

    // Rendering Method
    void draw();

    // Cell Access Method
    Cell* getCell(int clickX, int clickY);

    // Accessors and mutators
    Cell (*getCells())[3];
    const Cell (*getCells() const)[3];
    void setCells(const Cell& cell, int row, int col);
};