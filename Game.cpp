#include "Game.hpp"
#include <winbgim.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

#define COLOR_BACKGROUND_DARK COLOR(20, 20, 40)
#define COLOR_GLASS_FILL "rgba(255, 255, 255, 0.1)"
#define COLOR_GLASS_BORDER COLOR(255, 255, 255)
#define COLOR_TEXT COLOR(230, 230, 250)
#define COLOR_ACCENT_1 COLOR(102, 51, 153)
#define COLOR_ACCENT_2 COLOR(255, 105, 180)

Game::Game() : player1(new XShape(), "Player X"), player2(new OShape(), "Player O"), currentPlayer(&player1), page(0), currentState(GameState::START_PAGE) {
    initwindow(900, 600, "Tic Tac Toe");
    setbkcolor(COLOR_BACKGROUND_DARK);
    cleardevice();
    winningLine[0] = -1;
}

Game::~Game() {
    delete player1.getShape();
    delete player2.getShape();
}

void Game::run() {
    while (currentState != GameState::GAME_OVER) {
        setactivepage(page);
        cleardevice();
        drawBackground();

        if (currentState == GameState::START_PAGE) {
            drawStartPage();
        } else {
            setfillstyle(SOLID_FILL, COLOR(40, 40, 60));
            bar(40, 40, 560, 560);
            setcolor(COLOR_GLASS_BORDER);
            rectangle(40, 40, 560, 560);

            drawScoreboard();
            grid.draw();
            if (winningLine[0] != -1) {
                drawWinningLine();
            }
        }

        setvisualpage(page);
        page = 1 - page;

        if (kbhit()) {
            if (getch() == 27) break;
        }

        if (ismouseclick(WM_LBUTTONDOWN)) {
            int x, y;
            getmouseclick(WM_LBUTTONDOWN, x, y);

            if (currentState == GameState::START_PAGE) {
                if (x > 350 && x < 550 && y > 400 && y < 475) {
                    currentState = GameState::PLAYING;
                }
            } else if (currentState == GameState::PLAYING) {
                Cell* clickedCell = grid.getCell(x, y);
                if (clickedCell && !clickedCell->hasShape()) {
                    clickedCell->setShape(currentPlayer->getShape());

                    if (checkWin()) {
                        currentPlayer->incrementScore();
                        outcomeMessage = currentPlayer->getName() + " Wins!";
                        drawGameOver();
                        delay(2000);
                        resetGame();
                    } else if (checkDraw()) {
                        outcomeMessage = "It's a Draw!";
                        drawGameOver();
                        delay(2000);
                        resetGame();
                    } else {
                        switchPlayer();
                    }
                }
            }
        }
        delay(30);
    }
    closegraph();
}

void Game::drawBackground() {
}

void Game::drawStartPage() {
    setcolor(COLOR_TEXT);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 6);
    char title[] = "TIC-TAC-TOE";
    int textWidth = textwidth(title);
    outtextxy((900 - textWidth) / 2, 200, title);

    setfillstyle(SOLID_FILL, COLOR(60, 60, 80));
    bar(350, 400, 550, 475);
    setcolor(COLOR_GLASS_BORDER);
    rectangle(350, 400, 550, 475);
    setcolor(COLOR_TEXT);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
    char startText[] = "START";
    textWidth = textwidth(startText);
    outtextxy(350 + (200 - textWidth) / 2, 415, startText);
}

void Game::drawGameOver() {
    setactivepage(page);
    cleardevice();
    drawBackground();
    
    setfillstyle(SOLID_FILL, COLOR(40, 40, 60));
    bar(40, 40, 560, 560);
    setcolor(COLOR_GLASS_BORDER);
    rectangle(40, 40, 560, 560);

    drawScoreboard();
    grid.draw();
    if (winningLine[0] != -1) {
        drawWinningLine();
    }

    setcolor(COLOR(50, 205, 50));
    if (outcomeMessage == "It's a Draw!") {
        setcolor(COLOR_TEXT);
    }
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
    int textWidth = textwidth(&outcomeMessage[0]);
    outtextxy(580 + (300 - textWidth) / 2, 400, &outcomeMessage[0]);
    setvisualpage(page);
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
    setfillstyle(SOLID_FILL, COLOR(40, 40, 60));
    bar(580, 40, 880, 560);
    setcolor(COLOR_GLASS_BORDER);
    rectangle(580, 40, 880, 560);

    setcolor(COLOR_TEXT);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
    char title[] = "TIC-TAC-TOE";
    int textWidth = textwidth(title);
    outtextxy(580 + (300 - textWidth) / 2, 80, title);

    setcolor(COLOR(200, 200, 220));
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2);
    std::string turnMsg = "Turn: " + currentPlayer->getName();
    textWidth = textwidth(&turnMsg[0]);
    outtextxy(580 + (300 - textWidth) / 2, 180, &turnMsg[0]);

    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 3);
    std::stringstream ss1;
    ss1 << player1.getName() << ": " << player1.getScore();
    std::string p1_score = ss1.str();
    textWidth = textwidth(&p1_score[0]);
    outtextxy(580 + (300 - textWidth) / 2, 250, &p1_score[0]);

    std::stringstream ss2;
    ss2 << player2.getName() << ": " << player2.getScore();
    std::string p2_score = ss2.str();
    textWidth = textwidth(&p2_score[0]);
    outtextxy(580 + (300 - textWidth) / 2, 300, &p2_score[0]);
}

void Game::resetGame() {
    grid = Grid();
    winningLine[0] = -1;
    currentState = GameState::PLAYING;
}

void Game::drawWinningLine() {
    if (winningLine[0] == -1) return;

    setcolor(COLOR(50, 205, 50));
    setlinestyle(SOLID_LINE, 0, 10);

    int steps = 30;
    float startX = winningLine[0];
    float startY = winningLine[1];
    float endX = winningLine[2];
    float endY = winningLine[3];
    float dx = (endX - startX) / steps;
    float dy = (endY - startY) / steps;

    for (int i = 0; i <= steps; ++i) {
        line(startX, startY, startX + i * dx, startY + i * dy);
        setvisualpage(page);
        delay(10);
    }
}