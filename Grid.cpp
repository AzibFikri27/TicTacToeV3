#include "Grid.hpp"
#include <winbgim.h>

#define COLOR_GRID_GLASS COLOR(80, 80, 100)

Grid::Grid() {
    int startX = 60;
    int startY = 60;
    int cellSize = 160;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            cells[i][j].set(startX + j * cellSize, startY + i * cellSize, cellSize);
        }
    }
}

Grid::~Grid() {}

void Grid::draw() {
    setcolor(COLOR_GRID_GLASS);
    setlinestyle(SOLID_LINE, 0, 5);

    int startX = 60;
    int startY = 60;
    int cellSize = 160;
    int gridSize = 3 * cellSize;

    for (int i = 1; i < 3; i++) {
        line(startX, startY + i * cellSize, startX + gridSize, startY + i * cellSize);
        line(startX + i * cellSize, startY, startX + i * cellSize, startY + gridSize);
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            cells[i][j].draw();
        }
    }
}

Cell* Grid::getCell(int clickX, int clickY) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (cells[i][j].isClicked(clickX, clickY)) {
                return &cells[i][j];
            }
        }
    }
    return nullptr;
}

Cell (*Grid::getCells())[3] {
    return cells;
}

const Cell (*Grid::getCells() const)[3] {
    return cells;
}

void Grid::setCells(const Cell& cell, int row, int col) {
    if (row >= 0 && row < 3 && col >= 0 && col < 3) {
        cells[row][col] = cell;
    }
}