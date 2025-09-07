#ifndef COMBAT_H
#define COMBAT_H

#include "game.h"
#include <string>

void combatEncounter(GameState &state, const std::string &enemyName, int enemyHP, int enemyAtk);

#endif // COMBAT_H
