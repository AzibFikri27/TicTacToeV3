#pragma once

#include "Grid.hpp"
#include "Player.hpp"
#include "XShape.hpp"
#include "OShape.hpp"
#include <string>

enum class GameState {
    START_PAGE,
    PLAYING,
    GAME_OVER
};

class Game {
private:
    Grid grid;
    Player player1;
    Player player2;
    Player* currentPlayer;
    int page;
    int winningLine[4];
    GameState currentState;
    std::string outcomeMessage;

public:
    Game();
    ~Game();

    void run();

private:
    void switchPlayer();
    bool checkWin();
    bool checkDraw();
    void resetGame();

    void drawScoreboard();
    void drawWinningLine();
    void drawStartPage();
    void drawGameOver();
    void drawBackground();
};