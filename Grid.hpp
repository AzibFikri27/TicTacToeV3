#pragma once

#include "Cell.hpp"

class Grid {
private:
    Cell cells[3][3];

public:
    Grid();
    ~Grid();

    void draw();

    Cell* getCell(int clickX, int clickY);

    Cell (*getCells())[3];
    const Cell (*getCells() const)[3];
    void setCells(const Cell& cell, int row, int col);
};