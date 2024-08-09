#include "game.h"
#include <stdio.h>
#include "stb_image.h"


Level load_from_file(u32 stage){

    Level level = {};

    char path1[1024];
    sprintf(path1, "../assets/levels/%upart1.png", stage);
    u8* tmp = stbi_load(path1, (i32*) &level.width, (i32*) &level.height, NULL, STBI_rgb);

    return {};

}