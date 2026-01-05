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
#define GAME_BK_COLOR COLOR(0, 0, 0) // Black background
#define COLOR_GRID COLOR(255, 255, 255) // White grid
#define COLOR_TEXT COLOR(255, 255, 0) // Yellow text
#define COLOR_BUTTON COLOR(0, 255, 0) // Green button
#define COLOR_BUTTON_HOVER COLOR(50, 255, 50) // Lighter Green
#define COLOR_BUTTON_TEXT COLOR(0, 0, 0) // Black text on button
#define COLOR_WIN_LINE COLOR(255, 0, 255) // Magenta winning line
#define COLOR_EXIT_BUTTON COLOR(255, 0, 0) // Red button for exit
#define COLOR_EXIT_BUTTON_HOVER COLOR(255, 100, 100) // Lighter Red
#define COLOR_RESTART_BUTTON COLOR(0, 0, 255) // Blue button for restart
#define COLOR_RESTART_BUTTON_HOVER COLOR(100, 100, 255) // Lighter Blue
#define COLOR_POPUP_BG COLOR(20, 20, 20) // Dark grey for popup background
#define COLOR_POPUP_BORDER COLOR(255, 255, 255) // White border for popup
#define COLOR_SOUND_BUTTON COLOR(255, 165, 0) // Orange for sound button
#define COLOR_SOUND_BUTTON_HOVER COLOR(255, 200, 100) // Lighter Orange
#define COLOR_ACTIVE_PLAYER COLOR(0, 255, 0) // Green for active player text
#define COLOR_INACTIVE_PLAYER COLOR(100, 100, 100) // Grey for inactive player text

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

Game::Game() : player1(new XShape(), "Player X"), player2(new OShape(), "Player O"), currentPlayer(&player1), page(0), currentState(GameState::START_PAGE), bgImage(nullptr), soundEnabled(true) {
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

        // Draw sound toggle button on all screens
        drawSoundButton();

        setvisualpage(page);
        page = 1 - page;

        if (kbhit()) {
            if (getch() == 27) break;
        }

        if (ismouseclick(WM_LBUTTONDOWN)) {
            int x, y;
            getmouseclick(WM_LBUTTONDOWN, x, y);

            // Check sound button click (top right corner)
            if (isMouseOver(x, y, 850, 10, 890, 50)) {
                soundEnabled = !soundEnabled;
                playSound(800, 50);
            }
            else {
                if (currentState == GameState::START_PAGE) {
                    // Start Button
                    if (isMouseOver(x, y, 350, 400, 550, 475)) {
                        currentState = GameState::PLAYING;
                        playSound(659, 200); // Start game sound
                    }
                    // Exit Button
                    if (isMouseOver(x, y, 350, 500, 550, 575)) {
                        currentState = GameState::EXIT;
                    }
                } else if (currentState == GameState::PLAYING) {
                    // Restart Button
                    if (isMouseOver(x, y, 600, 350, 860, 400)) {
                        resetGame();
                        resetScores();
                        playSound(659, 200);
                    }
                    // Exit to Main Menu Button
                    else if (isMouseOver(x, y, 600, 420, 860, 470)) {
                        resetGame();
                        resetScores();
                        currentState = GameState::START_PAGE;
                        playSound(659, 200);
                    }
                    else {
                        Cell* clickedCell = grid.getCell(x, y);
                        if (clickedCell && !clickedCell->hasShape()) {
                            clickedCell->setShape(currentPlayer->getShape());

                            // Distinct sound for each player
                            if (currentPlayer == &player1) {
                                playSound(800, 100); // Higher pitch for X
                            } else {
                                playSound(600, 100); // Lower pitch for O
                            }

                            // Force redraw to show the move immediately
                            grid.draw();
                            setvisualpage(page);

                            if (checkWin()) {
                                currentPlayer->incrementScore();
                                outcomeMessage = currentPlayer->getName() + " Wins!";

                                // Win sound effect (Async)
                                if (soundEnabled) {
                                    CreateThread(NULL, 0, PlayWinSoundThread, NULL, 0, NULL);
                                }

                                // Animate the winning line
                                animateWinningLine();

                                showGameOverAnimation(outcomeMessage);
                                currentState = GameState::GAME_OVER;
                                gameOverTimer = clock();
                            } else if (checkDraw()) {
                                outcomeMessage = "It's a Draw!";

                                // Draw sound effect (Async)
                                if (soundEnabled) {
                                    CreateThread(NULL, 0, PlayDrawSoundThread, NULL, 0, NULL);
                                }

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
    int mx = mousex();
    int my = mousey();

    setcolor(COLOR_TEXT);
    settextstyle(BOLD_FONT, HORIZ_DIR, 6);
    char title[] = "TIC-TAC-TOE";
    int textWidth = textwidth(title);
    outtextxy((900 - textWidth) / 2, 150, title);

    setlinestyle(SOLID_LINE, 0, 3);

    // Draw Start Button with Hover
    if (mx > 350 && mx < 550 && my > 400 && my < 475) {
        setfillstyle(SOLID_FILL, COLOR_BUTTON_HOVER);
    } else {
        setfillstyle(SOLID_FILL, COLOR_BUTTON);
    }
    bar(350, 400, 550, 475);
    setcolor(COLOR(255, 255, 255));
    rectangle(350, 400, 550, 475);

    setcolor(COLOR_BUTTON_TEXT);

    fillsettingstype fs;
    getfillsettings(&fs);
    setbkcolor(fs.color); // Match text bg to button color

    settextstyle(BOLD_FONT, HORIZ_DIR, 4);
    char startText[] = "START";
    textWidth = textwidth(startText);
    outtextxy(350 + (200 - textWidth) / 2, 415, startText);
    setbkcolor(GAME_BK_COLOR);

    // Draw Exit Button with Hover
    if (mx > 350 && mx < 550 && my > 500 && my < 575) {
        setfillstyle(SOLID_FILL, COLOR_EXIT_BUTTON_HOVER);
    } else {
        setfillstyle(SOLID_FILL, COLOR_EXIT_BUTTON);
    }
    bar(350, 500, 550, 575);
    setcolor(COLOR(255, 255, 255));
    rectangle(350, 500, 550, 575);
    
    setcolor(COLOR(255, 255, 255));

    getfillsettings(&fs);
    setbkcolor(fs.color);

    settextstyle(BOLD_FONT, HORIZ_DIR, 4);
    char exitText[] = "EXIT";
    textWidth = textwidth(exitText);
    outtextxy(350 + (200 - textWidth) / 2, 515, exitText);
    setbkcolor(GAME_BK_COLOR);

    // Version/Credits
    setcolor(COLOR(150, 150, 150));
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    outtextxy(10, 580, (char*)"Improvised Version v1.2");
}

void Game::drawGameOver() {
    // Not used directly anymore, replaced by drawPopup
}

void Game::showGameOverAnimation(const std::string& message) {
    int centerX = 450;
    int centerY = 300;
    int maxW = 400;
    int maxH = 200;

    for (int i = 1; i <= 5; ++i) {
        setactivepage(page);
        cleardevice();
        drawBackground();

        setcolor(COLOR_GRID);
        setlinestyle(SOLID_LINE, 0, 3);
        rectangle(35, 35, 565, 565);
        drawScoreboard();
        grid.draw();

        if (winningLine[0] != -1) {
            setcolor(COLOR_WIN_LINE);
            setlinestyle(SOLID_LINE, 0, 10);
            line(winningLine[0], winningLine[1], winningLine[2], winningLine[3]);
        }

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
        delay(15);
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

    static int blinkCounter = 0;
    blinkCounter++;
    if ((blinkCounter / 10) % 2 == 0) {
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
    int mx = mousex();
    int my = mousey();

    // Draw border for scoreboard
    setcolor(COLOR_GRID);
    setlinestyle(SOLID_LINE, 0, 3); // Ensure consistent border thickness
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

    // Player 1 Score (Highlight if active)
    if (currentPlayer == &player1) {
        setcolor(COLOR_ACTIVE_PLAYER);
    } else {
        setcolor(COLOR_INACTIVE_PLAYER);
    }
    std::stringstream ss1;
    ss1 << player1.getName() << ": " << player1.getScore();
    std::string p1_score = ss1.str();
    textWidth = textwidth(&p1_score[0]);
    outtextxy(580 + (300 - textWidth) / 2, 180, &p1_score[0]);

    // Player 2 Score (Highlight if active)
    if (currentPlayer == &player2) {
        setcolor(COLOR_ACTIVE_PLAYER);
    } else {
        setcolor(COLOR_INACTIVE_PLAYER);
    }
    std::stringstream ss2;
    ss2 << player2.getName() << ": " << player2.getScore();
    std::string p2_score = ss2.str();
    textWidth = textwidth(&p2_score[0]);
    outtextxy(580 + (300 - textWidth) / 2, 230, &p2_score[0]);

    // Restart Button with Hover
    if (mx > 600 && mx < 860 && my > 350 && my < 400) {
        setfillstyle(SOLID_FILL, COLOR_RESTART_BUTTON_HOVER);
    } else {
        setfillstyle(SOLID_FILL, COLOR_RESTART_BUTTON);
    }
    bar(600, 350, 860, 400);
    setcolor(COLOR(255, 255, 255));
    rectangle(600, 350, 860, 400);

    fillsettingstype fs;
    getfillsettings(&fs);
    setbkcolor(fs.color);

    settextstyle(BOLD_FONT, HORIZ_DIR, 2);
    char restartText[] = "RESTART GAME";
    textWidth = textwidth(restartText);
    outtextxy(600 + (260 - textWidth) / 2, 365, restartText);
    setbkcolor(GAME_BK_COLOR);

    // Exit to Main Menu Button with Hover
    if (mx > 600 && mx < 860 && my > 420 && my < 470) {
        setfillstyle(SOLID_FILL, COLOR_EXIT_BUTTON_HOVER);
    } else {
        setfillstyle(SOLID_FILL, COLOR_EXIT_BUTTON);
    }
    bar(600, 420, 860, 470);
    setcolor(COLOR(255, 255, 255));
    rectangle(600, 420, 860, 470);

    getfillsettings(&fs);
    setbkcolor(fs.color);

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
        setactivepage(page);
        cleardevice();
        drawBackground();

        setcolor(COLOR_GRID);
        setlinestyle(SOLID_LINE, 0, 3);
        rectangle(35, 35, 565, 565);
        drawScoreboard();
        grid.draw();

        setcolor(COLOR_WIN_LINE);
        setlinestyle(SOLID_LINE, 0, 10);
        line(startX, startY, startX + i * dx, startY + i * dy);

        setvisualpage(page);
        page = 1 - page;
        delay(10);
    }
}

bool Game::isMouseOver(int mx, int my, int x1, int y1, int x2, int y2) {
    return mx >= x1 && mx <= x2 && my >= y1 && my <= y2;
}

void Game::drawSoundButton() {
    int mx = mousex();
    int my = mousey();
    int x1 = 850, y1 = 10, x2 = 890, y2 = 50;

    if (mx > x1 && mx < x2 && my > y1 && my < y2) {
        setfillstyle(SOLID_FILL, COLOR_SOUND_BUTTON_HOVER);
    } else {
        setfillstyle(SOLID_FILL, COLOR_SOUND_BUTTON);
    }
    bar(x1, y1, x2, y2);

    // Explicitly set line style for the button border to prevent it from inheriting thicker lines from the game grid
    setlinestyle(SOLID_LINE, 0, 2);
    setcolor(COLOR(255, 255, 255));
    rectangle(x1, y1, x2, y2);

    // Draw Speaker Icon
    setcolor(COLOR(0, 0, 0)); // Black icon
    setfillstyle(SOLID_FILL, COLOR(0, 0, 0));

    // Speaker Body
    bar(x1 + 8, y1 + 14, x1 + 13, y1 + 26);

    // Speaker Cone
    int poly[] = {
        x1 + 13, y1 + 14,
        x1 + 13, y1 + 26,
        x1 + 24, y1 + 32,
        x1 + 24, y1 + 8
    };
    fillpoly(4, poly);

    setlinestyle(SOLID_LINE, 0, 2);
    if (soundEnabled) {
        // Sound Waves
        arc(x1 + 18, y1 + 20, 315, 45, 10);
        arc(x1 + 18, y1 + 20, 315, 45, 14);
    } else {
        // Mute X
        line(x1 + 28, y1 + 15, x1 + 35, y1 + 25);
        line(x1 + 35, y1 + 15, x1 + 28, y1 + 25);
    }

    setbkcolor(GAME_BK_COLOR);
}

void Game::playSound(int freq, int duration) {
    if (soundEnabled) {
        playBeepAsync(freq, duration);
    }
}