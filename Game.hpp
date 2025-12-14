#pragma once

#include "Grid.hpp"
#include "Player.hpp"
#include "XShape.hpp"
#include "OShape.hpp"

enum class GameState {
    START_PAGE,
    PLAYING,
    GAME_OVER
};

class Game {
private:
    // Attributes
    Grid grid;
    Player player1;
    Player player2;
    Player* currentPlayer;
    int page;
    int winningLine[4];
    GameState currentState;
    std::string outcomeMessage;

public:
    // Constructors and destructor
    Game();
    ~Game();

    // Main Game Loop
    void run();

private:
    // Game Logic Methods
    void switchPlayer();
    bool checkWin();
    bool checkDraw();
    void resetGame();

    // Rendering Methods
    void drawScoreboard();
    void drawWinningLine();
    void drawStartPage();
    void drawGameOver();
    void drawBackground();
};