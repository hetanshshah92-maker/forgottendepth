#include "combat.h"
#include "items.h"
#include <iostream>
#include <cstdlib>

extern void addItemToInventory(GameState &state, const Item &item);
extern bool useHealingPotion(GameState &state);

static int randRange(int low, int high) {
    return low + std::rand() % (high - low + 1);
}

void combatEncounter(GameState &state, const std::string &enemyName, int enemyHP, int enemyAtk) {
    std::cout << "\n-- Combat: " << enemyName << " appears! --\n";
    while (enemyHP > 0 && state.health > 0) {
        std::cout << "\nPlayer HP: " << state.health << " / " << state.maxHealth
                  << "  |  Stamina: " << state.stamina << "\n";
        std::cout << enemyName << " HP: " << enemyHP << "\n";
        std::cout << "Choose action:\n";
        std::cout << "1) Attack\n";
        std::cout << "2) Defend (reduce incoming damage)\n";
        std::cout << "3) Use potion (if available)\n";
        std::cout << "4) Flee (chance)\n";
        std::cout << "Choice: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000,'\n');
            choice = 0;
        }

        if (choice == 1) {
            // Attack: damage depends on stamina
            int base = randRange(1, 4);
            if (state.stamina > 0) {
                base += state.stamina > 6 ? 2 : 1;
                state.stamina = std::max(0, state.stamina - 2);
            }
            std::cout << "You strike for " << base << " damage.\n";
            enemyHP -= base;
        } else if (choice == 2) {
            std::cout << "You brace yourself to reduce the next attack.\n";
            // enemy attacks with reduced power
            int dmg = std::max(0, enemyAtk - randRange(1,3));
            dmg = dmg / 2; // defend halves damage
            std::cout << enemyName << " hits you for " << dmg << " damage.\n";
            state.health -= dmg;
            state.stamina = std::max(0, state.stamina - 1);
        } else if (choice == 3) {
            bool used = useHealingPotion(state);
            if (!used) {
                // Skip enemy turn? enemy still attacks
                int dmg = randRange(enemyAtk-1, enemyAtk+1);
                std::cout << enemyName << " attacks you for " << dmg << " damage while you fumble.\n";
                state.health -= dmg;
            }
        } else if (choice == 4) {
            // Attempt flee: success chance depends on stamina
            int chance = 40 + state.stamina * 4; // 40% base
            int roll = randRange(1,100);
            if (roll <= chance) {
                std::cout << "You successfully fled the battle!\n";
                // fleeing costs stamina
                state.stamina = std::max(0, state.stamina - 3);
                return;
            } else {
                std::cout << "Flee failed!\n";
                // enemy attacks full
                int dmg = randRange(enemyAtk-1, enemyAtk+2);
                std::cout << enemyName << " hits you for " << dmg << " damage.\n";
                state.health -= dmg;
            }
        } else {
            std::cout << "Invalid action; the enemy seizes the moment.\n";
            int dmg = randRange(enemyAtk-1, enemyAtk+2);
            state.health -= dmg;
            std::cout << enemyName << " hits you for " << dmg << " damage.\n";
        }

        // If enemy still alive, they attack (unless we already used defend logic)
        if (enemyHP > 0 && state.health > 0 && choice != 2) {
            int dmg = randRange(enemyAtk-1, enemyAtk+2);
            std::cout << enemyName << " attacks and deals " << dmg << " damage.\n";
            state.health -= dmg;
        }
    }

    if (state.health <= 0) {
        std::cout << "\nYou have been slain by " << enemyName << "...\n";
        state.gameOver = true;
        return;
    }

    std::cout << "\nYou defeated " << enemyName << "!\n";
    // Reward: gold + chance for potion
    int goldReward = randRange(3,9);
    state.gold += goldReward;
    std::cout << "You loot " << goldReward << " gold.\n";
    // Small chance to drop a potion
    int drop = randRange(1,100);
    if (drop <= 35) {
        addItemToInventory(state, Item("Minor Potion", "Restores 4 HP", 4, 10));
    }
}
