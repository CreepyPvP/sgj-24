#pragma once

#include "game.h"

Game LoadGameFromFile(u32 stage, u32 *buffer1, u32 *buffer2);

Level LoadLevelFromFile(u32 stage, u32 *buffer, bool part_one);
