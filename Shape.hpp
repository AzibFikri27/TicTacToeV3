#pragma once

class Shape {
public:
    virtual ~Shape() {}
    virtual void draw(int x, int y, int size) = 0;
};