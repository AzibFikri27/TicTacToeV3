#include "Game.hpp"
#include <winbgim.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <windows.h> // Explicitly include windows.h for CreateThread
#include <sys/stat.h> // For checking file existence

// 8-bit Color Palette
#define GAME_BK_COLOR COLOR(0, 0, 0) // Black background (Renamed to avoid conflict)
#define COLOR_GRID COLOR(255, 255, 255) // White grid
#define COLOR_TEXT COLOR(255, 255, 0) // Yellow text
#define COLOR_BUTTON COLOR(0, 255, 0) // Green button
#define COLOR_BUTTON_TEXT COLOR(0, 0, 0) // Black text on button
#define COLOR_WIN_LINE COLOR(255, 0, 255) // Magenta winning line
#define COLOR_EXIT_BUTTON COLOR(255, 0, 0) // Red button for exit
#define COLOR_RESTART_BUTTON COLOR(0, 0, 255) // Blue button for restart
#define COLOR_POPUP_BG COLOR(20, 20, 20) // Dark grey for popup background
#define COLOR_POPUP_BORDER COLOR(255, 255, 255) // White border for popup

// Thread functions for async sound
DWORD WINAPI PlayBeepThread(LPVOID lpParam) {
    int* params = (int*)lpParam;
    Beep(params[0], params[1]);
    delete[] params;
    return 0;
}

void playBeepAsync(int freq, int duration) {
    int* params = new int[2];
    params[0] = freq;
    params[1] = duration;
    CreateThread(NULL, 0, PlayBeepThread, params, 0, NULL);
}

DWORD WINAPI PlayWinSoundThread(LPVOID lpParam) {
    Beep(523, 150); Beep(659, 150); Beep(784, 150); Beep(1046, 300);
    return 0;
}

DWORD WINAPI PlayDrawSoundThread(LPVOID lpParam) {
    Beep(440, 300); Beep(349, 300);
    return 0;
}

// Helper to check if file exists
bool fileExists(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

Game::Game() : player1(new XShape(), "Player X"), player2(new OShape(), "Player O"), currentPlayer(&player1), page(0), currentState(GameState::START_PAGE), bgImage(nullptr) {
    initwindow(900, 600, "Tic Tac Toe - 8 Bit Edition");
    setbkcolor(GAME_BK_COLOR);
    cleardevice();
    winningLine[0] = -1;
    gameOverTimer = 0;

    // Try to find the asset file
    std::string assetPath = "assets/pixel-art-night-sky-starry-space-with-shooting-stars-8-bit-pixelated-game-galaxy-seamless-background-vector.jpg";
    if (!fileExists(assetPath)) {
        // Try absolute path if relative fails (common in IDE builds)
        assetPath = "C:/Users/azibf/CLionProjects/TicTacToeV3/assets/pixel-art-night-sky-starry-space-with-shooting-stars-8-bit-pixelated-game-galaxy-seamless-background-vector.jpg";
    }

    if (fileExists(assetPath)) {
        // Draw to active page (which is 0 initially)
        readimagefile(assetPath.c_str(), 0, 0, 900, 600);

        // Allocate memory for the image
        // Note: imagesize might return -1 if size is too big for 16-bit BGI, but winbgim handles it.
        unsigned int imageSize = imagesize(0, 0, 900, 600);
        if (imageSize != -1) {
            bgImage = malloc(imageSize);
            if (bgImage) {
                getimage(0, 0, 900, 600, bgImage);
            }
        }
    } else {
        std::cerr << "Background image not found!" << std::endl;
    }

    // Clear for the start of the loop
    cleardevice();
}

Game::~Game() {
    delete player1.getShape();
    delete player2.getShape();
    if (bgImage) {
        free(bgImage);
    }
}

void Game::run() {
    while (currentState != GameState::EXIT) {
        setactivepage(page);
        cleardevice();
        drawBackground();

        if (currentState == GameState::START_PAGE) {
            drawStartPage();
        } else if (currentState == GameState::PLAYING) {
            // Draw game area border
            setcolor(COLOR_GRID);
            setlinestyle(SOLID_LINE, 0, 3);
            rectangle(35, 35, 565, 565);

            drawScoreboard();
            grid.draw();
            // No static line drawing here, animation happens on win
        } else if (currentState == GameState::GAME_OVER) {
             // Keep drawing the game state in the background
            setcolor(COLOR_GRID);
            setlinestyle(SOLID_LINE, 0, 3);
            rectangle(35, 35, 565, 565);
            drawScoreboard();
            grid.draw();

            // Draw winning line only once (it's already calculated)
            if (winningLine[0] != -1) {
                setcolor(COLOR_WIN_LINE);
                setlinestyle(SOLID_LINE, 0, 10);
                line(winningLine[0], winningLine[1], winningLine[2], winningLine[3]);
            }

            // Draw the popup over it
            drawPopup(outcomeMessage);
        }

        setvisualpage(page);
        page = 1 - page;

        if (kbhit()) {
            if (getch() == 27) break;
        }

        if (ismouseclick(WM_LBUTTONDOWN)) {
            int x, y;
            getmouseclick(WM_LBUTTONDOWN, x, y);

            // Play a simple beep sound on click
            playBeepAsync(523, 100);

            if (currentState == GameState::START_PAGE) {
                // Start Button
                if (x > 350 && x < 550 && y > 400 && y < 475) {
                    currentState = GameState::PLAYING;
                    playBeepAsync(659, 200); // Start game sound
                }
                // Exit Button
                if (x > 350 && x < 550 && y > 500 && y < 575) {
                    currentState = GameState::EXIT;
                }
            } else if (currentState == GameState::PLAYING) {
                // Restart Button
                if (x > 600 && x < 860 && y > 350 && y < 400) {
                    resetGame();
                    resetScores();
                    playBeepAsync(659, 200);
                }
                // Exit to Main Menu Button
                else if (x > 600 && x < 860 && y > 420 && y < 470) {
                    resetGame();
                    resetScores();
                    currentState = GameState::START_PAGE;
                    playBeepAsync(659, 200);
                }
                else {
                    Cell* clickedCell = grid.getCell(x, y);
                    if (clickedCell && !clickedCell->hasShape()) {
                        clickedCell->setShape(currentPlayer->getShape());

                        // Force redraw to show the move immediately
                        grid.draw();
                        setvisualpage(page);

                        if (checkWin()) {
                            currentPlayer->incrementScore();
                            outcomeMessage = currentPlayer->getName() + " Wins!";

                            // Win sound effect (Async)
                            CreateThread(NULL, 0, PlayWinSoundThread, NULL, 0, NULL);

                            // Animate the winning line
                            animateWinningLine();

                            showGameOverAnimation(outcomeMessage);
                            currentState = GameState::GAME_OVER;
                            gameOverTimer = clock();
                        } else if (checkDraw()) {
                            outcomeMessage = "It's a Draw!";

                            // Draw sound effect (Async)
                            CreateThread(NULL, 0, PlayDrawSoundThread, NULL, 0, NULL);

                            showGameOverAnimation(outcomeMessage);
                            currentState = GameState::GAME_OVER;
                            gameOverTimer = clock();
                        } else {
                            switchPlayer();
                        }
                    }
                }
            } else if (currentState == GameState::GAME_OVER) {
                // Click anywhere to continue to next round
                if ((double)(clock() - gameOverTimer) / CLOCKS_PER_SEC >= 1.0) {
                    resetGame();
                }
            }
        }
        delay(33);
    }
    closegraph();
}

void Game::drawBackground() {
    if (bgImage) {
        putimage(0, 0, bgImage, COPY_PUT);
    } else {
        // Fallback if image failed to load
        setbkcolor(GAME_BK_COLOR);
        cleardevice();
    }
}

void Game::drawStartPage() {
    setcolor(COLOR_TEXT);
    settextstyle(BOLD_FONT, HORIZ_DIR, 6); // Use BOLD_FONT for blockier look
    char title[] = "TIC-TAC-TOE";
    int textWidth = textwidth(title);
    outtextxy((900 - textWidth) / 2, 150, title);

    // Draw Start Button
    setfillstyle(SOLID_FILL, COLOR_BUTTON);
    bar(350, 400, 550, 475);
    setcolor(COLOR(255, 255, 255));
    rectangle(350, 400, 550, 475);

    setcolor(COLOR_BUTTON_TEXT);
    setbkcolor(COLOR_BUTTON); // Set background color for text to match button
    settextstyle(BOLD_FONT, HORIZ_DIR, 4);
    char startText[] = "START";
    textWidth = textwidth(startText);
    outtextxy(350 + (200 - textWidth) / 2, 415, startText);
    setbkcolor(GAME_BK_COLOR); // Reset background color

    // Draw Exit Button
    setfillstyle(SOLID_FILL, COLOR_EXIT_BUTTON);
    bar(350, 500, 550, 575);
    setcolor(COLOR(255, 255, 255));
    rectangle(350, 500, 550, 575);
    
    setcolor(COLOR(255, 255, 255)); // White text for exit
    setbkcolor(COLOR_EXIT_BUTTON);
    settextstyle(BOLD_FONT, HORIZ_DIR, 4);
    char exitText[] = "EXIT";
    textWidth = textwidth(exitText);
    outtextxy(350 + (200 - textWidth) / 2, 515, exitText);
    setbkcolor(GAME_BK_COLOR);
}

void Game::drawGameOver() {
    // Not used directly anymore, replaced by drawPopup
}

void Game::showGameOverAnimation(const std::string& message) {
    // Simple "pop-in" animation
    int centerX = 450;
    int centerY = 300;
    int maxW = 400;
    int maxH = 200;

    // Faster animation: fewer steps, larger increments
    for (int i = 1; i <= 5; ++i) {
        setactivepage(page);
        cleardevice();
        drawBackground();

        // Draw game state in background
        setcolor(COLOR_GRID);
        setlinestyle(SOLID_LINE, 0, 3);
        rectangle(35, 35, 565, 565);
        drawScoreboard();
        grid.draw();

        // Draw static winning line if it exists
        if (winningLine[0] != -1) {
            setcolor(COLOR_WIN_LINE);
            setlinestyle(SOLID_LINE, 0, 10);
            line(winningLine[0], winningLine[1], winningLine[2], winningLine[3]);
        }

        // Draw expanding popup
        int currentW = (maxW * i) / 5;
        int currentH = (maxH * i) / 5;
        int left = centerX - currentW / 2;
        int top = centerY - currentH / 2;
        int right = centerX + currentW / 2;
        int bottom = centerY + currentH / 2;

        setfillstyle(SOLID_FILL, COLOR_POPUP_BG);
        bar(left, top, right, bottom);
        setcolor(COLOR_POPUP_BORDER);
        setlinestyle(SOLID_LINE, 0, 3);
        rectangle(left, top, right, bottom);

        setvisualpage(page);
        page = 1 - page;
        delay(15); // Reduced delay for faster animation
    }
}

void Game::drawPopup(const std::string& message) {
    int centerX = 450;
    int centerY = 300;
    int w = 400;
    int h = 200;
    int left = centerX - w / 2;
    int top = centerY - h / 2;
    int right = centerX + w / 2;
    int bottom = centerY + h / 2;

    setfillstyle(SOLID_FILL, COLOR_POPUP_BG);
    bar(left, top, right, bottom);
    setcolor(COLOR_POPUP_BORDER);
    setlinestyle(SOLID_LINE, 0, 3);
    rectangle(left, top, right, bottom);

    setcolor(COLOR_TEXT);
    setbkcolor(COLOR_POPUP_BG);
    settextstyle(BOLD_FONT, HORIZ_DIR, 4);
    int textWidth = textwidth(const_cast<char*>(message.c_str()));
    outtextxy(centerX - textWidth / 2, centerY - 40, const_cast<char*>(message.c_str()));

    settextstyle(BOLD_FONT, HORIZ_DIR, 2);
    setcolor(COLOR(200, 200, 200));
    char continueMsg[] = "Click to Continue";
    textWidth = textwidth(continueMsg);

    // Blinking effect for "Click to Continue"
    // Use a static counter that increments based on frame updates, not just calls
    // But since this is called in a loop with delay(30), we can just use a counter
    static int blinkCounter = 0;
    blinkCounter++;
    if ((blinkCounter / 10) % 2 == 0) { // Blink every ~300ms (10 frames * 30ms)
        outtextxy(centerX - textWidth / 2, centerY + 40, continueMsg);
    }

    setbkcolor(GAME_BK_COLOR);
}


void Game::switchPlayer() {
    currentPlayer = (currentPlayer == &player1) ? &player2 : &player1;
}

bool Game::checkWin() {
    const Cell (*cells)[3] = grid.getCells();
    const Shape* currentPlayerShape = currentPlayer->getShape();
    int cellSize = 160;
    int startX = 60;
    int startY = 60;

    for (int i = 0; i < 3; i++) {
        if (cells[i][0].getShape() == currentPlayerShape && cells[i][1].getShape() == currentPlayerShape && cells[i][2].getShape() == currentPlayerShape) {
            winningLine[0] = startX + 20;
            winningLine[1] = startY + i * cellSize + cellSize / 2;
            winningLine[2] = startX + 3 * cellSize - 20;
            winningLine[3] = winningLine[1];
            return true;
        }
        if (cells[0][i].getShape() == currentPlayerShape && cells[1][i].getShape() == currentPlayerShape && cells[2][i].getShape() == currentPlayerShape) {
            winningLine[0] = startX + i * cellSize + cellSize / 2;
            winningLine[1] = startY + 20;
            winningLine[2] = winningLine[0];
            winningLine[3] = startY + 3 * cellSize - 20;
            return true;
        }
    }

    if (cells[0][0].getShape() == currentPlayerShape && cells[1][1].getShape() == currentPlayerShape && cells[2][2].getShape() == currentPlayerShape) {
        winningLine[0] = startX + 20;
        winningLine[1] = startY + 20;
        winningLine[2] = startX + 3 * cellSize - 20;
        winningLine[3] = startY + 3 * cellSize - 20;
        return true;
    }
    if (cells[0][2].getShape() == currentPlayerShape && cells[1][1].getShape() == currentPlayerShape && cells[2][0].getShape() == currentPlayerShape) {
        winningLine[0] = startX + 3 * cellSize - 20;
        winningLine[1] = startY + 20;
        winningLine[2] = startX + 20;
        winningLine[3] = startY + 3 * cellSize - 20;
        return true;
    }

    return false;
}

bool Game::checkDraw() {
    const Cell (*cells)[3] = grid.getCells();
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (!cells[i][j].hasShape()) {
                return false;
            }
        }
    }
    return true;
}

void Game::drawScoreboard() {
    // Draw border for scoreboard
    setcolor(COLOR_GRID);
    rectangle(580, 40, 880, 560);

    setcolor(COLOR_TEXT);
    settextstyle(BOLD_FONT, HORIZ_DIR, 4);
    char title[] = "SCORE";
    int textWidth = textwidth(title);
    outtextxy(580 + (300 - textWidth) / 2, 60, title);

    setcolor(COLOR(200, 200, 200));
    settextstyle(BOLD_FONT, HORIZ_DIR, 2);
    std::string turnMsg = "Turn: " + currentPlayer->getName();
    textWidth = textwidth(&turnMsg[0]);
    outtextxy(580 + (300 - textWidth) / 2, 120, &turnMsg[0]);

    settextstyle(BOLD_FONT, HORIZ_DIR, 3);
    std::stringstream ss1;
    ss1 << player1.getName() << ": " << player1.getScore();
    std::string p1_score = ss1.str();
    textWidth = textwidth(&p1_score[0]);
    outtextxy(580 + (300 - textWidth) / 2, 180, &p1_score[0]);

    std::stringstream ss2;
    ss2 << player2.getName() << ": " << player2.getScore();
    std::string p2_score = ss2.str();
    textWidth = textwidth(&p2_score[0]);
    outtextxy(580 + (300 - textWidth) / 2, 230, &p2_score[0]);

    // Restart Button
    setfillstyle(SOLID_FILL, COLOR_RESTART_BUTTON);
    bar(600, 350, 860, 400);
    setcolor(COLOR(255, 255, 255));
    rectangle(600, 350, 860, 400);

    setbkcolor(COLOR_RESTART_BUTTON);
    settextstyle(BOLD_FONT, HORIZ_DIR, 2);
    char restartText[] = "RESTART GAME";
    textWidth = textwidth(restartText);
    outtextxy(600 + (260 - textWidth) / 2, 365, restartText);
    setbkcolor(GAME_BK_COLOR);

    // Exit to Main Menu Button
    setfillstyle(SOLID_FILL, COLOR_EXIT_BUTTON);
    bar(600, 420, 860, 470);
    setcolor(COLOR(255, 255, 255));
    rectangle(600, 420, 860, 470);

    setbkcolor(COLOR_EXIT_BUTTON);
    settextstyle(BOLD_FONT, HORIZ_DIR, 2);
    char exitText[] = "MAIN MENU";
    textWidth = textwidth(exitText);
    outtextxy(600 + (260 - textWidth) / 2, 435, exitText);
    setbkcolor(GAME_BK_COLOR);
}

void Game::resetGame() {
    grid = Grid();
    winningLine[0] = -1;
    currentState = GameState::PLAYING;
}

void Game::resetScores() {
    player1.resetScore();
    player2.resetScore();
}

void Game::animateWinningLine() {
    if (winningLine[0] == -1) return;

    setcolor(COLOR_WIN_LINE);
    setlinestyle(SOLID_LINE, 0, 10);

    int steps = 30;
    float startX = winningLine[0];
    float startY = winningLine[1];
    float endX = winningLine[2];
    float endY = winningLine[3];
    float dx = (endX - startX) / steps;
    float dy = (endY - startY) / steps;

    for (int i = 0; i <= steps; ++i) {
        // We need to redraw the background and game state for each frame of the animation
        // because we are using double buffering (setactivepage/setvisualpage)
        setactivepage(page);
        cleardevice();
        drawBackground();

        // Draw game board
        setcolor(COLOR_GRID);
        setlinestyle(SOLID_LINE, 0, 3);
        rectangle(35, 35, 565, 565);
        drawScoreboard();
        grid.draw();

        // Draw the partial line
        setcolor(COLOR_WIN_LINE);
        setlinestyle(SOLID_LINE, 0, 10);
        line(startX, startY, startX + i * dx, startY + i * dy);

        setvisualpage(page);
        page = 1 - page;
        delay(10);
    }
}