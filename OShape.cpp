#include "OShape.hpp"
#include <winbgim.h>

#define COLOR_O COLOR(3, 169, 244)

void OShape::draw(int x, int y, int size) {
    setcolor(COLOR_O);
    setlinestyle(SOLID_LINE, 0, 9);
    int padding = 30;
    ellipse(x + size / 2, y + size / 2, 0, 360, size / 2 - padding, size / 2 - padding);
}