#ifndef TEXT_RPG
#ifdef __cplusplus
#include <LuaCpp/LuaCpp.hpp>
extern "C" {   
#endif
    #include <ncurses.h>
    #include <string.h>
#ifdef __cplusplus
}
#endif
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

void RPG_terminate(int __status) {
    endwin();
    exit(__status);
}

static int RPGPrint(lua_State* L) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        if (lua_isstring(L, i)) {
            printw(lua_tostring(L, i));
        }
        else if (lua_isnumber(L, i)) {
            printw("%d", lua_tonumber(L, i));
        }
        else if (lua_isboolean(L, i)) {
            if (lua_toboolean(L, i)) {
                printw("true");
            }
            else {
                printw("false");
            }
        }
        else {
            lua_pushliteral(L, "incorrect argument");
            lua_error(L);
        }
    }
    return 0;
}

static int RPGPrintln(lua_State* L) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        if (lua_isstring(L, i)) {
            printw(lua_tostring(L, i));
        }
        else if (lua_isnumber(L, i)) {
            printw("%d", lua_tonumber(L, i));
        }
        else if (lua_isboolean(L, i)) {
            if (lua_toboolean(L, i)) {
                printw("true");
            }
            else {
                printw("false");
            }
        }
        else {
            lua_pushliteral(L, "incorrect argument");
            lua_error(L);
        }
    }
    printw("\n");
    return 0;
}

static int RPGReadln(lua_State* L) {
    char* str = (char*)malloc(sizeof(char*) * 256);
    char ch = getch();
    int y,x = 0;
    getyx(stdscr, y, x);
    while (ch != 10) {
        if (ch == 7 && strlen(str) > 0) { // delete
            str[strlen(str) - 1] = '\0';
        }
        else if (ch == 9) { // tab
            printw("    ");
            strcat(str, "    ");
        }
        else if (ch == 2) { // down arrow

        }
        else if (ch == 3) { // up arrow

        }
        else if (ch == 4) { // left arrow

        }
        else if (ch == 5) { // right arrow

        }
        else if (ch != -1) { // exclude no char
            printw("%c", ch);
            strncat(str, &ch, 1);
        }
        mvprintw(y, x, "%s", str);
        printw(" ");
        move(y, x + strlen(str));
        refresh();
        ch = getch();
    }
    printw("\n");
    lua_pushstring(L, str);
    return 1;
}

#ifdef __cplusplus
// C++ CODE
#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <fstream>

namespace RPG {
    LuaCpp::LuaContext LUA;

    class Entity {
        public:
            int id;
            std::string name;
            int pos[2];
            char symbol;
            bool multiplayer;
            const char* interact;
            Entity(std::string entity_name, int posX = 0, int posY = 0, char entity_symbol = 'X', bool is_multiplayer = false, const char* interact_file = "") {
                int id_gen;
                id = id_gen;
                id_gen++;
                name = entity_name;
                pos[0] = posX;
                pos[1] = posY;
                symbol = entity_symbol;
                multiplayer = is_multiplayer;
                interact = interact_file;
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
            void new_entity(std::string entity_name, int posX = 0, int posY = 0, char entity_symbol = 'X', bool is_multiplayer = false, const char* interact_file = "") {
                Entity ent = Entity(entity_name, posX, posY, entity_symbol, is_multiplayer, interact_file);
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
    bool do_redraw = false;
    bool stop_draw = false;
}


static RPG::Board* RPG_PLAYER_BOARD = &RPG::DEFAULT_BOARD;

void RPG_PLAYER_teleport(int x, int y, RPG::Board* board = RPG_PLAYER_BOARD) {
    bool redraw = false;
    if (RPG_PLAYER_BOARD != board) {
        redraw = true;
    }
    RPG_PLAYER_BOARD = board;
    if (redraw) {
        RPG::do_redraw = true;
    }
    if (x >= 0 && x < RPG_PLAYER_BOARD->dim[0]) {
        RPG_PLAYER_POS[0] = x;
    }
    if (y >= 0 && y < RPG_PLAYER_BOARD->dim[1]) {
        RPG_PLAYER_POS[1] = y;
    }
}

void RPG_PLAYER_move(int x_additive, int y_additive) {
    int new_pos[2] = {0, 0};
    new_pos[0] = RPG_PLAYER_POS[0] + x_additive;
    new_pos[1] = RPG_PLAYER_POS[1] + y_additive;
    for (int i = 0; i < RPG_PLAYER_BOARD->entities.size(); i++) {
        if (new_pos[0] == RPG_PLAYER_BOARD->entities[i]->pos[0] && new_pos[1] == RPG_PLAYER_BOARD->entities[i]->pos[1]) {
            return;
        }
    }
    RPG_PLAYER_teleport(new_pos[0], new_pos[1]);
}

namespace RPG {
    void wait_for_enter() {
        int y,x = 0;
        getmaxyx(stdscr, y, x);
        mvprintw(y - 1, 0, "Press any key to continue... ");
        char ch = -1;
        while (ch == -1) {
            ch = getch();
        }
    }

    void init_draw() {
        clear();
        refresh();
        mvprintw(0, 0, "%s", RPG_TITLE);
    }

    void draw() {
        char** grid = (char**)malloc(sizeof(char**) * RPG_PLAYER_BOARD->dim[1]);
        for (int i = 0; i < RPG_PLAYER_BOARD->dim[1]; i++) {
            grid[i] = (char*)malloc(sizeof(char*) * RPG_PLAYER_BOARD->dim[0]);
            for (int j = 0; j < RPG_PLAYER_BOARD->dim[0]; j++) {
                grid[i][j] = ' ';
            }
        }

        for (int i = 0; i < RPG_PLAYER_BOARD->entities.size(); i++) {
            grid[RPG_PLAYER_BOARD->entities[i]->pos[1]][RPG_PLAYER_BOARD->entities[i]->pos[0]] = RPG_PLAYER_BOARD->entities[i]->symbol;
        }
        grid[RPG_PLAYER_POS[1]][RPG_PLAYER_POS[0]] = RPG_PLAYER_SYMBOL;

        const char* dir = "None";
        switch (RPG_PLAYER_DIR) {
            case RPG_NORTH:
                dir = "North";
                break;
            case RPG_EAST:
                dir = "East";
                break;
            case RPG_SOUTH:
                dir = "South";
                break;
            case RPG_WEST:
                dir = "West";
                break;
            default:
                break;
        }

        refresh();
        move(2, 0);
        for (int i = 0; i < RPG_PLAYER_BOARD->dim[1]; i++) {
            for (int j = 0; j < RPG_PLAYER_BOARD->dim[0]; j++) {
                printw("[%c]", grid[i][j]);
            }
            printw("\n");
        }
        int y,x = 0;
        getmaxyx(stdscr, y, x);
        mvprintw(y - 2, 0, "Compass Direction: %s\n", dir);
        printw("Input (h for help): ");
        refresh();
    }

    bool interact(int posX, int posY) {
        Entity* target = nullptr;
        for (int i = 0; i < RPG_PLAYER_BOARD->entities.size(); i++) {
            if (RPG_PLAYER_BOARD->entities[i]->pos[0] == posX && RPG_PLAYER_BOARD->entities[i]->pos[1] == posY) {
                target = RPG_PLAYER_BOARD->entities[i];
                break;
            }
        }
        if (target != nullptr) {
            clear();
            refresh();
            if (strlen(target->interact) > 0) {
                if (std::filesystem::exists(target->interact)) {
                    clear();
                    scrollok(stdscr, TRUE);
                    LUA.CompileFileAndRun(target->interact);
                    scrollok(stdscr, FALSE);
                    init_draw();
                    return false;
                }
            }
            mvprintw(0, 0, "You cannot interact with this entity");
            wait_for_enter();
            init_draw();
        }
        return false;
    }

    void input() {
        char c = getch();
        if (c == 27) {
            clear();
            refresh();
            mvprintw(0, 0, "Are you sure you want to quit");
            int y,x = 0;
            getmaxyx(stdscr, y, x);
            mvprintw(y - 1, 0, "Input [N/y]: ");
            char ch = -1;
            while (ch == -1) {
                ch = getch();
            }
            if (ch == 'y') {
                RPG_terminate(0);
            }
            init_draw();
        }
        else if (c == 'h') {
            clear();
            refresh();
            mvprintw(0, 0, "HELP:\n");
            printw("wasd to move\n");
            printw("e to interact\n");
            printw("i to show inventory\n");
            printw("p to show character stats\n");
            printw("h to show this menu\n");
            wait_for_enter();
            init_draw();
        }
        else if (c == 'w') {
            RPG_PLAYER_DIR = RPG_NORTH;
            RPG_PLAYER_move(0, -1);
        }
        else if (c == 'd') {
            RPG_PLAYER_DIR = RPG_EAST;
            RPG_PLAYER_move(1, 0);
        }
        else if (c == 's') {
            RPG_PLAYER_DIR = RPG_SOUTH;
            RPG_PLAYER_move(0, 1);
        }
        else if (c == 'a') {
            RPG_PLAYER_DIR = RPG_WEST;
            RPG_PLAYER_move(-1, 0);
        }
        else if (c == 'e') {
            int target[2];
            target[0] = RPG_PLAYER_POS[0];
            target[1] = RPG_PLAYER_POS[1];
            switch (RPG_PLAYER_DIR) {
                case RPG_NORTH:
                    target[1] -= 1;
                    break;
                case RPG_EAST:
                    target[0] += 1;
                    break;
                case RPG_SOUTH:
                    target[1] += 1;
                    break;
                case RPG_WEST:
                    target[0] -= 1;
                    break;
                default:
                    break;
            }
            if (interact(target[0], target[1])) {
                RPG_terminate(0);
            }
        }
    }

    void start_game() {
        initscr();
        raw();
        keypad(stdscr, TRUE);
        noecho();
        timeout(50);
        std::shared_ptr<LuaCpp::Registry::LuaLibrary> rpglib = std::make_shared<LuaCpp::Registry::LuaLibrary>("RPG");
        rpglib->AddCFunction("print", RPGPrint);
        rpglib->AddCFunction("println", RPGPrintln);
        rpglib->AddCFunction("readln", RPGReadln);
        LUA.AddLibrary(rpglib);
        while (!stop_draw) {
            init_draw();
            do_redraw = false;
            while (!do_redraw && !stop_draw) {
                draw();
                input();
            }
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