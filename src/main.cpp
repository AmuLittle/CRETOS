#include <iostream>
#include "TextRPG.h"

int main(int argc, char** argv) {
    RPG::DEFAULT_BOARD.new_entity("Foxy", 2, 2, 'F', false, "./Foxy.lua");

    RPG::start_game();
    return 0;
}