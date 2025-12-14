#include "Player.hpp"

Player::Player(Shape* playerShape, const std::string& name) : shape(playerShape), name(name), score(0) {}

Player::~Player() {
    delete shape;
}

Shape* Player::getShape() const {
    return shape;
}

const std::string& Player::getName() const {
    return name;
}

int Player::getScore() const {
    return score;
}

void Player::incrementScore() {
    score++;
}