#include "game.h"
#include "scenes.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// prototypes from items.cpp
extern void addItemToInventory(GameState &state, const Item &item);
extern bool useHealingPotion(GameState &state);

static void showInventory(const GameState &state) {
    std::cout << "\n-- Inventory --\n";
    if (state.inventory.empty()) {
        std::cout << "(empty)\n";
        return;
    }
    for (size_t i = 0; i < state.inventory.size(); ++i) {
        std::cout << i+1 << ") " << state.inventory[i].name
                  << " - " << state.inventory[i].description << "\n";
    }
}

static bool hasItem(const GameState &state, const std::string &name) {
    for (const auto &it : state.inventory) {
        if (it.name == name) return true;
    }
    return false;
}

static void tryFinishIfHasHeart(GameState &state) {
    if (hasItem(state, "Heart Shard")) {
        std::cout << "\nYou possess a Heart Shard. If you reach the surface alive, you might reconstruct the artifact.\n";
    }
}

void startGame() {
    std::srand((unsigned)std::time(nullptr));
    GameState state;
    std::cout << "Welcome to Legends of the Forgotten Depths!\n";
    std::cout << "Goal: Descend, find the Heart Shard, and escape alive with treasure.\n";

    while (!state.gameOver) {
        std::cout << "\nMain Menu: What do you want to do?\n";
        std::cout << "1) Explore the next chamber\n";
        std::cout << "2) Show status and inventory\n";
        std::cout << "3) Use a healing potion\n";
        std::cout << "4) Attempt to ascend to the surface (end run)\n";
        std::cout << "5) Quit (abandon run)\n";
        std::cout << "Choice: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000,'\n');
            choice = 0;
        }

        switch (choice) {
            case 1:
                exploreRoom(state);
                break;
            case 2:
                std::cout << "\nHP: " << state.health << "/" << state.maxHealth
                          << "  Stamina: " << state.stamina << "/" << state.maxStamina
                          << "  Gold: " << state.gold << "\n";
                showInventory(state);
                break;
            case 3:
                useHealingPotion(state);
                break;
            case 4:
                // Attempt to ascend: success depends on depth and whether you have shard
                std::cout << "You attempt to find your way back to the surface...\n";
                if (state.depthReached < 2) {
                    std::cout << "You found the stairs back easily. You surface safely.\n";
                    if (hasItem(state, "Heart Shard")) {
                        std::cout << "You still hold the Heart Shard. It resonates and reassembles into the Heart!\n";
                        std::cout << "Victory! You restored the artifact and escaped with treasure.\n";
                        state.won = true;
                    } else {
                        std::cout << "You escaped but without the Heart Shard. You survive with your spoils.\n";
                    }
                    state.gameOver = true;
                } else {
                    // chance to surface depends on health and depth
                    int chance = 40 + std::max(0, state.health - 5) * 5 - (state.depthReached - 3) * 5;
                    int roll = std::rand() % 100 + 1;
                    if (roll <= chance) {
                        std::cout << "Against the odds you navigated back to the surface!\n";
                        if (hasItem(state, "Heart Shard")) {
                            std::cout << "You reconstructed the Heart. Victory and riches await.\n";
                            state.won = true;
                        } else {
                            std::cout << "You escaped to the surface with your gold but without the Heart.\n";
                        }
                        state.gameOver = true;
                    } else {
                        std::cout << "You stumble and lose your way deeper into the depths.\n";
                        // penalty
                        state.health = std::max(0, state.health - 2);
                        if (state.health <= 0) {
                            std::cout << "You succumbed while trying to find the surface...\n";
                            state.gameOver = true;
                        }
                    }
                }
                break;
            case 5:
                std::cout << "You decide to abandon the run. The Depths will wait for another day.\n";
                state.gameOver = true;
                break;
            default:
                std::cout << "Invalid choice.\n";
                break;
        }

        // small passive regeneration + check
        if (!state.gameOver) {
            state.stamina = std::min(state.maxStamina, state.stamina + 0); // keep stable
            if (state.health <= 0) {
                std::cout << "Your wounds are too great. You perish in the depths.\n";
                state.gameOver = true;
            }
            tryFinishIfHasHeart(state);
        }
    }

    if (state.won) {
        std::cout << "\n*** CONGRATULATIONS — YOU WON! ***\n";
    } else if (state.gameOver) {
        std::cout << "\nGame Over. You reached depth " << state.depthReached << " and collected "
                  << state.gold << " gold.\n";
        if (hasItem(state, "Heart Shard")) {
            std::cout << "You carried a Heart Shard, but failed to escape.\n";
        }
    }
    std::cout << "Thanks for playing.\n";
}
