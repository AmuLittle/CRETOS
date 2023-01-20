#ifndef TEXT_RPG
#include <ncurses.h>
// C & C++ CODE

typedef enum {
    RPG_NORTH,
    RPG_EAST,
    RPG_SOUTH,
    RPG_WEST,
    RPG_NO_DIRECTION
} RPG_Dir;

static const char* RPG_TITLE = "Game";

static const char* RPG_PLAYER_NAME = "Player";
static int RPG_PLAYER_POS[2] = {0 ,0};
static char RPG_PLAYER_SYMBOL = 'X';
static RPG_Dir RPG_PLAYER_DIR = RPG_NO_DIRECTION;

void RPG_init() {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    scrollok(stdscr, TRUE);
}

void RPG_terminate(int __status) {
    endwin();
    exit(__status);
}

#ifdef __cplusplus
// C++ CODE
#include <string>
#include <vector>
#include <iostream>

namespace RPG {

    class Entity {
        public:
            int id;
            std::string name;
            int pos[2];
            char symbol;
            Entity(std::string entity_name, int posX = 0, int posY = 0, char entity_symbol = 'X') {
                int id_gen;
                id = id_gen;
                id_gen++;
                name = entity_name;
                pos[0] = posX;
                pos[1] = posY;
                symbol = entity_symbol;
            }
    };
    static std::vector<Entity> _entity_stack = std::vector<Entity>();

    class Portal {
        public:
            int id;
            std::string name;
            int pos[2];
            int dest_pos[2];
            char symbol;
            bool open;
            Portal(std::string portal_name, int posX = 0, int posY = 0, int dest_posX = 1, int dest_posY = 1, char portal_symbol = '0', bool is_open = true) {
                int id_gen;
                id = id_gen;
                id_gen++;
                name = portal_name;
                pos[0] = posX;
                pos[1] = posY;
                dest_pos[0] = dest_posX;
                dest_pos[1] = dest_posY;
                symbol = portal_symbol;
                open = is_open;
            }
    };
    static std::vector<Portal> _portal_stack = std::vector<Portal>();

    class Board {
        public:
            int id;
            std::string name;
            int dim[2];
            std::vector<Entity*> entities;
            std::vector<Portal*> portal_starts;
            std::vector<Portal*> portal_ends;
            Board(std::string board_name, int dimX, int dimY) {
                int id_gen;
                id = id_gen;
                id_gen++;
                name = board_name;
                dim[0] = dimX;
                dim[1] = dimY;
                entities = std::vector<Entity*>();
                portal_starts = std::vector<Portal*>();
                portal_ends = std::vector<Portal*> ();
            }
            void new_entity(std::string entity_name, int posX = 0, int posY = 0, char entity_symbol = 'X') {
                Entity ent = Entity(entity_name, posX, posY, entity_symbol);
                _entity_stack.push_back(ent);
                entities.push_back(&_entity_stack[_entity_stack.size() - 1]);
            }
            void new_portal(std::string portal_name, int posX = 0, int posY = 0, int dest_posX = 1, int dest_posY = 1, Board* dest_board = nullptr, char portal_symbol = '0', bool is_open = true) {
                Portal por = Portal(portal_name, posX, posY, dest_posX, dest_posY, portal_symbol, is_open);
                _portal_stack.push_back(por);
                portal_starts.push_back(&_portal_stack[_portal_stack.size() - 1]);
                dest_board->portal_ends.push_back(&_portal_stack[_portal_stack.size() - 1]);
            }
    };
    static Board DEFAULT_BOARD = Board("DEFAULT", 5, 5);
}

static RPG::Board* RPG_PLAYER_BOARD = &RPG::DEFAULT_BOARD;
void RPG_PLAYER_move(int x, int y, RPG::Board* board = RPG_PLAYER_BOARD) {
    RPG_PLAYER_BOARD = board;
    if (x >= 0 && x < RPG_PLAYER_BOARD->dim[0]) {
        RPG_PLAYER_POS[0] = x;
    }
    if (y >= 0 && y < RPG_PLAYER_BOARD->dim[1]) {
        RPG_PLAYER_POS[1] = y;
    }
}

namespace RPG {
    void draw() {
        const char* facing_dir = "";
        switch (RPG_PLAYER_DIR) {
            case RPG_NORTH:
                facing_dir = "North";
                break;
            case RPG_EAST:
                facing_dir = "East";
                break;
            case RPG_SOUTH:
                facing_dir = "South";
                break;
            case RPG_WEST:
                facing_dir = "West";
                break;
            default:
                facing_dir = "None";
                break;
        }
        char** grid = (char**)malloc(RPG_PLAYER_BOARD->dim[1] * sizeof(char**));
        for (int i = 0; i < RPG_PLAYER_BOARD->dim[1]; i++) {
            grid[i] = (char*)malloc(RPG_PLAYER_BOARD->dim[0] * sizeof(char*));
            for (int j = 0; j < RPG_PLAYER_BOARD->dim[0]; j++) {
                grid[i][j] = ' ';
            }
        }
        

        for (int i = 0; i < RPG_PLAYER_BOARD->portal_starts.size(); i++) {
            if (RPG_PLAYER_BOARD->portal_starts[i]->open) {
                grid[RPG_PLAYER_BOARD->portal_starts[i]->pos[1]][RPG_PLAYER_BOARD->portal_starts[i]->pos[0]] = RPG_PLAYER_BOARD->portal_starts[i]->symbol;
            }
        }
        for (int i = 0; i < RPG_PLAYER_BOARD->portal_ends.size(); i++) {
            if (RPG_PLAYER_BOARD->portal_ends[i]->open) {
                grid[RPG_PLAYER_BOARD->portal_ends[i]->pos[1]][RPG_PLAYER_BOARD->portal_ends[i]->pos[0]] = RPG_PLAYER_BOARD->portal_ends[i]->symbol;
            }
        }
        for (int i = 0; i < RPG_PLAYER_BOARD->entities.size(); i++) {
            grid[RPG_PLAYER_BOARD->entities[i]->pos[1]][RPG_PLAYER_BOARD->entities[i]->pos[0]] = RPG_PLAYER_BOARD->entities[i]->symbol;
        }
        grid[RPG_PLAYER_POS[1]][RPG_PLAYER_POS[0]] = RPG_PLAYER_SYMBOL;

        refresh();
        printw("\n");
        printw("%s\n", RPG_TITLE);
        printw("\n");
        for (int i = 0; i < RPG_PLAYER_BOARD->dim[1]; i++) {
            for (int j = 0; j < RPG_PLAYER_BOARD->dim[0]; j++) {
                printw("[%c]", grid[i][j]);
            }
            printw("\n");
        }
        printw("\n");
        printw("Compass Direction: %s\n", facing_dir);
        printw("\n");
        printw("Input (help for help): ");
    }

    void wait_for_enter() {
        printw("Press any key to continue... ");
        char ch;
        ch = getch();
    }

    void input() {
        std::string input = "";
        while (true) {
            refresh();
            char ch = getch();
            if (ch == '\n') {
                printw("\n");
                break;
            }
            else if (ch == 3) {
                RPG_terminate(0);
            }
            else if (ch == 7) {
                if (input.length() > 0) {
                    input.pop_back();
                    int x,y = 0;
                    getyx(stdscr, y, x);
                    mvaddch(y, x - 1, ' ');
                    move(y, x - 1);
                }
            }
            else {
                input.push_back(ch);
                printw("%c", ch);
            }
        }
        refresh();
        if (!input.compare("help")) {
            printw("\n");
            printw("Help is comming soon!\n");
            printw("If this is an alpha or beta build please refer to README.md\n");
            printw("\n");
            wait_for_enter();
        }
        else if (!input.compare("quit")) {
            printw("\n");
            RPG_terminate(0);
        }
        else {
            for (int i = 0; i < input.length(); i++) {
                int player_posX = RPG_PLAYER_POS[0];
                int player_posY = RPG_PLAYER_POS[1];
                if (input[i] == 'w') {
                    RPG_PLAYER_DIR = RPG_NORTH;
                    player_posY--;
                    RPG_PLAYER_move(player_posX, player_posY);
                    continue;
                }
                else if (input[i] == 'd') {
                    RPG_PLAYER_DIR = RPG_EAST;
                    player_posX++;
                    RPG_PLAYER_move(player_posX, player_posY);
                    continue;
                }
                else if (input[i] == 's') {
                    RPG_PLAYER_DIR = RPG_SOUTH;
                    player_posY++;
                    RPG_PLAYER_move(player_posX, player_posY);
                    continue;
                }
                else if (input[i] == 'a') {
                    RPG_PLAYER_DIR = RPG_WEST;
                    player_posX--;
                    RPG_PLAYER_move(player_posX, player_posY);
                    continue;
                }
            }
        }
    }

    void start_game() {
        while (true) {
            draw();
            input();
        }
    }
}

#elif // !__cplusplus
// C CODE
#include <string.h>

static const char* RPG_PLAYER_BOARD = "DEFAULT"

#endif // __cplusplus

#define TEXT_RPG
#endif // TEXT_RPG