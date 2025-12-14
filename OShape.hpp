#pragma once

#include "Shape.hpp"
#include <winbgim.h>

class OShape : public Shape {
public:
    // Drawing Method
    void draw(int x, int y, int size) override;
};