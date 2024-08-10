#pragma once

#include "game.h"

void LoadGameFromFile(Game* game, u32 stage_number);

void LoadLevelFromFile(Level* level, u32 stage, bool part_one);
