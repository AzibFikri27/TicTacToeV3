#pragma once

#include "Shape.hpp"
#include <string>

class Player {
private:
    // Attributes
    Shape* shape;
    std::string name;
    int score;

public:
    // Constructors and destructor
    Player(Shape* playerShape, const std::string& name);
    ~Player();

    // Accessors and mutators
    Shape* getShape() const;
    const std::string& getName() const;
    int getScore() const;
    void incrementScore();
};