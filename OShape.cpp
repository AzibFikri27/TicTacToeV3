#include "OShape.hpp"
#include <winbgim.h>

#define COLOR_O COLOR(0, 0, 255) // Bright Blue for 8-bit style

void OShape::draw(int x, int y, int size) {
    setcolor(COLOR_O);
    setlinestyle(SOLID_LINE, 0, 9);
    int padding = 30;
    // Draw circle (ellipse)
    ellipse(x + size / 2, y + size / 2, 0, 360, size / 2 - padding, size / 2 - padding);
}