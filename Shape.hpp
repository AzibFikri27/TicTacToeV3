#pragma once

class Shape {
public:
    // Constructors and destructor
    virtual ~Shape() {}

    // Abstract Drawing Method
    virtual void draw(int x, int y, int size) = 0;
};