#pragma once

#include "Grid.hpp"
#include "Player.hpp"
#include "XShape.hpp"
#include "OShape.hpp"
#include <string>
#include <ctime>

enum class GameState {
    START_PAGE,
    PLAYING,
    GAME_OVER,
    EXIT
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
    clock_t gameOverTimer;
    void* bgImage; // Buffer for background image
    void* soundOnImage; // Buffer for sound on image
    void* soundOffImage; // Buffer for sound off image
    bool soundEnabled;

public:
    Game();
    ~Game();

    void run();

private:
    void switchPlayer();
    bool checkWin();
    bool checkDraw();
    void resetGame();
    void resetScores();

    void drawScoreboard();
    void animateWinningLine();
    void drawStartPage();
    void drawGameOver();
    void drawBackground();

    void showGameOverAnimation(const std::string& message);
    void drawPopup(const std::string& message);

    bool isMouseOver(int mx, int my, int x1, int y1, int x2, int y2);
    void drawSoundButton();
    void playSound(int freq, int duration);
};