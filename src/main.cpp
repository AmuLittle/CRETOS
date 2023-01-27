#include <iostream>
#include "TextRPG.h"

int main(int argc, char** argv) {
    RPG::DEFAULT_BOARD.new_entity("Foxy", 2, 2, 'F');

    RPG::start_game();
    return 0;
}