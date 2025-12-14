#include "XShape.hpp"

// Define modern colors
#define COLOR_X COLOR(255, 87, 34) // Deep Orange

void XShape::draw(int x, int y, int size) {
    setcolor(COLOR_X);
    setlinestyle(SOLID_LINE, 0, 9);
    int padding = 30;
    line(x + padding, y + padding, x + size - padding, y + size - padding);
    line(x + size - padding, y + padding, x + padding, y + size - padding);
}