#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include "items.h"

struct GameState {
    int health;
    int maxHealth;
    int stamina;
    int maxStamina;
    int gold;
    int depthReached;   // how deep player has gone (rooms visited)
    bool gameOver;
    bool won;
    std::vector<Item> inventory;
    GameState() {
        maxHealth = 10;
        health = maxHealth;
        maxStamina = 10;
        stamina = maxStamina;
        gold = 0;
        depthReached = 0;
        gameOver = false;
        won = false;
    }
};

void startGame();

#endif // GAME_H
