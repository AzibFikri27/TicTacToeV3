#pragma once

#include "Shape.hpp"
#include <winbgim.h>

class OShape : public Shape {
public:
    void draw(int x, int y, int size) override;
};