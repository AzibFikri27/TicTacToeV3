#pragma once

#include "Shape.hpp"
#include <winbgim.h>

class XShape : public Shape {
public:
    void draw(int x, int y, int size) override;
};