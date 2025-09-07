#include "scenes.h"
#include "combat.h"
#include "items.h"
#include <iostream>
#include <cstdlib>

extern void addItemToInventory(GameState &state, const Item &item);
static int randRange(int low, int high) { return low + std::rand() % (high - low + 1); }

void showStatus(const GameState &state) {
    std::cout << "\n--- Status ---\n";
    std::cout << "HP: " << state.health << "/" << state.maxHealth
              << "  |  Stamina: " << state.stamina << "/" << state.maxStamina
              << "  |  Gold: " << state.gold << "\n";
    std::cout << "Depth: " << state.depthReached << "\n";
    std::cout << "--------------\n";
}

void inspectChest(GameState &state) {
    int trapChance = randRange(1,100);
    if (trapChance <= 30) {
        std::cout << "The chest was trapped! A volley of spikes hits you.\n";
        int dmg = randRange(1,4);
        state.health -= dmg;
        std::cout << "You take " << dmg << " damage.\n";
    } else {
        int gold = randRange(5,15);
        std::cout << "You open the chest and find " << gold << " gold!\n";
        state.gold += gold;
        // chance for item
        if (randRange(1,100) <= 40) {
            addItemToInventory(state, Item("Minor Potion", "Restores 4 HP", 4, 10));
        }
    }
}

void findWanderer(GameState &state) {
    std::cout << "A wounded wanderer asks for help in exchange for a map piece.\n";
    std::cout << "1) Help (costs a potion or some gold)\n2) Ignore\nChoice: ";
    int c;
    if (!(std::cin >> c)) { std::cin.clear(); std::cin.ignore(10000,'\n'); c = 0; }
    if (c == 1) {
        if (state.gold >= 5) {
            std::cout << "You pay 5 gold. The wanderer gives you a map piece (reduces future trap chance).\n";
            state.gold -= 5;
            // We model map piece as increased stamina (simple effect)
            state.stamina = std::min(state.maxStamina, state.stamina + 2);
        } else {
            std::cout << "You lack gold. The wanderer curses your greed and leaves.\n";
            // small penalty
            state.health = std::max(1, state.health - 1);
        }
    } else {
        std::cout << "You walk away. The wanderer glares at you.\n";
    }
}

void exploreRoom(GameState &state) {
    if (state.gameOver) return;

    state.depthReached++;
    std::cout << "\n=== You enter a new chamber (" << state.depthReached << ") ===\n";
    showStatus(state);

    int roll = randRange(1,100);
    // Weighted choices: 40% normal, 25% combat, 20% chest, 10% event, 5% special (artifact)
    if (roll <= 25) {
        // Combat
        int which = randRange(1,3);
        if (which == 1) {
            combatEncounter(state, "Goblin", 6, 2);
        } else if (which == 2) {
            combatEncounter(state, "Skeleton", 8, 3);
        } else {
            combatEncounter(state, "Crawling Horror", 10, 4);
        }
    } else if (roll <= 45) {
        // Trap / empty
        int trap = randRange(1,100);
        if (trap <= 40) {
            std::cout << "A trap triggers! Spikes erupt from the floor.\n";
            int dmg = randRange(1,5);
            state.health -= dmg;
            std::cout << "You take " << dmg << " damage.\n";
        } else {
            std::cout << "This chamber is eerily quiet. Nothing but old bones.\n";
        }
    } else if (roll <= 65) {
        // Chest
        std::cout << "You find an old chest at the center of the chamber.\n";
        std::cout << "1) Open it\n2) Inspect for traps\n3) Leave it\nChoice: ";
        int choice;
        if (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(10000,'\n'); choice = 0; }
        if (choice == 1) {
            inspectChest(state);
        } else if (choice == 2) {
            int disarm = randRange(1,100);
            if (disarm <= 65) {
                std::cout << "You disarm a hidden trap and safely open the chest.\n";
                int gold = randRange(8,20);
                state.gold += gold;
                std::cout << "You get " << gold << " gold.\n";
                if (randRange(1,100) <= 50) addItemToInventory(state, Item("Minor Potion", "Restores 4 HP", 4, 10));
            } else {
                std::cout << "You fail to disarm the trap! It triggers.\n";
                int dmg = randRange(2,6);
                state.health -= dmg;
                std::cout << "You take " << dmg << " damage.\n";
            }
        } else {
            std::cout << "You decide not to risk it and move on.\n";
        }
    } else if (roll <= 85) {
        // Event / NPC
        findWanderer(state);
    } else if (roll <= 95) {
        // Small treasure room
        std::cout << "You discover a small treasure alcove.\n";
        int gold = randRange(10,30);
        state.gold += gold;
        std::cout << "You collect " << gold << " gold.\n";
        if (randRange(1,100) <= 60) addItemToInventory(state, Item("Minor Potion", "Restores 4 HP", 4, 10));
    } else {
        // Special - artifact or boss chance: deeper rooms more likely to contain goals
        std::cout << "This chamber hums with power. In its center lies a polished stone: the Heart Shard.\n";
        std::cout << "You recognize it as a fragment of the Heart of the Depths!\n";
        std::cout << "If you collect 1 Heart Shard and reach the surface you may unlock the artifact.\n";
        addItemToInventory(state, Item("Heart Shard", "A fragment of the Depths' Heart (quest item)", 0, 0));
    }

    // small natural stamina regen
    state.stamina = std::min(state.maxStamina, state.stamina + 1);

    // Check death
    if (state.health <= 0) {
        std::cout << "\nYou collapsed from your wounds in the depths...\n";
        state.gameOver = true;
    }
}
