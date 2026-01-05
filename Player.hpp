#pragma once

#include "Shape.hpp"
#include <string>

class Player {
private:
    Shape* shape;
    std::string name;
    int score;

public:
    Player(Shape* playerShape, const std::string& name);
    ~Player();

    Shape* getShape() const;
    const std::string& getName() const;
    int getScore() const;
    void incrementScore();
};