#include "items.h"
#include "game.h"
#include <iostream>

// Helper functions for inventory management
void addItemToInventory(GameState &state, const Item &item) {
    state.inventory.push_back(item);
    std::cout << "You obtained: " << item.name << " - " << item.description << "\n";
}

bool useHealingPotion(GameState &state) {
    for (size_t i = 0; i < state.inventory.size(); ++i) {
        if (state.inventory[i].healAmount > 0) {
            int heal = state.inventory[i].healAmount;
            state.health += heal;
            if (state.health > state.maxHealth) state.health = state.maxHealth;
            std::cout << "You used " << state.inventory[i].name << " and healed " << heal << " HP.\n";
            // remove used item
            state.inventory.erase(state.inventory.begin() + i);
            return true;
        }
    }
    std::cout << "You have no healing potions!\n";
    return false;
}
