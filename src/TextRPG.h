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
    int y,x,xmod = 0;
    getyx(stdscr, y, x);
    while (ch != 10) {
        if (ch == 7 && strlen(str) > 0) { // delete
            char* strcopy = (char*)malloc(sizeof(str));
            strcpy(strcopy, str);
            char* strpoint = str;
            str = (char*)malloc(sizeof(strcopy));
            for (int i = 0; i < sizeof(strcopy) / sizeof(char*); i++) {
                str[i] = '\0';
            }
            free(strpoint);
            int i = 0;
            for (i; i < strlen(strcopy) - xmod; i++) {
                strncat(str, &strcopy[i], 1);
            }
            str[strlen(str) - 1] = '\0';
            for (i; i < strlen(strcopy); i++) {
                strncat(str, &strcopy[i], 1);
            }
            free(strcopy);
        }
        else if (ch == 9) { // tab
            printw("    ");
            strcat(str, "    ");
        }
        else if (ch == 2) { // down arrow

        }
        else if (ch == 3) { // up arrow

        }
        else if (ch == 4 && getcurx(stdscr) > x) { // left arrow
            xmod++;
        }
        else if (ch == 5 && getcurx(stdscr) < x + strlen(str)) { // right arrow
            xmod--;
        }
        else if (ch != -1 && !(ch >= 2 && ch <= 5)) { // exclude no char
            char* strcopy = (char*)malloc(sizeof(str));
            strcpy(strcopy, str);
            char* strpoint = str;
            str = (char*)malloc(sizeof(strcopy));
            for (int i = 0; i < sizeof(strcopy) / sizeof(char*); i++) {
                str[i] = '\0';
            }
            free(strpoint);
            int i = 0;
            for (i; i < strlen(strcopy) - xmod; i++) {
                strncat(str, &strcopy[i], 1);
            }
            strncat(str, &ch, 1);
            for (i; i < strlen(strcopy); i++) {
                strncat(str, &strcopy[i], 1);
            }
            free(strcopy);
        }
        mvprintw(y, x, "%s ", str);
        move(y, x + strlen(str) - xmod);
        refresh();
        ch = getch();
    }
    mvprintw(y, x + strlen(str), "\n");
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

    class Entity : public LuaCpp::LuaMetaObject {
        public:
            int id;
            std::string name;
            int pos[2];
            std::string board_name = "DEFAULT";
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
            std::shared_ptr<LuaCpp::Engine::LuaType> getValue(std::string &key) {
                if (key == "name") {
                    return std::make_shared<LuaCpp::Engine::LuaTString>(name);
                }
                else if (key == "pos") {
                    std::shared_ptr<LuaCpp::Engine::LuaTTable> out = std::make_shared<LuaCpp::Engine::LuaTTable>();
                    out->setValue(LuaCpp::Engine::Table::Key(1), std::make_shared<LuaCpp::Engine::LuaTNumber>(pos[0]));
                    out->setValue(LuaCpp::Engine::Table::Key(2), std::make_shared<LuaCpp::Engine::LuaTNumber>(pos[1]));
                    return out;
                }
                else if (key == "board") {
                    return std::make_shared<LuaCpp::Engine::LuaTString>(board_name);
                }
                else if (key == "symbol") {
                    std::string out = "";
                    out.append(&symbol);
                    return std::make_shared<LuaCpp::Engine::LuaTString>(out);
                }
                else if (key == "interact_script") {
                    return std::make_shared<LuaCpp::Engine::LuaTString>(interact);
                }
                return std::make_shared<LuaCpp::Engine::LuaTNil>();
            }
            void setValue(std::string &key, std::shared_ptr<LuaCpp::Engine::LuaType> val) {
                printw("Entity.key = value: use Entity(key, value) to set values");
                int y,x = 0;
                getmaxyx(stdscr, y, x);
                mvprintw(y - 1, 0, "Press any key to quit... ");
                char ch = -1;
                while (ch == -1) {
                    ch = getch();
                }
                RPG_terminate(0);
            }
            int Execute(LuaCpp::Engine::LuaState &L) {
                int n = lua_gettop(L);
                if (n != 3) {
                    lua_pushliteral(L, "Entity(): entity get accessor must contain two arguments");
                    lua_error(L);
                    return 0;
                }
                else if (!lua_isstring(L, 2)) {
                    lua_pushliteral(L, "Entity(): first argument must be a valid key");
                    lua_error(L);
                    return 0;
                }
                const char* key = lua_tostring(L, 2);
                if (!strcmp(key, "name") && lua_isstring(L, 3)) {
                    name = lua_tostring(L, 2);
                }
                else if (!strcmp(key, "pos") && lua_istable(L, 3)) {
                    int x = lua_rawgeti(L, 3, 1);
                    int y = lua_rawgeti(L, 3, 2);
                    printw("%i, %i", x, y);
                }
                else if (!strcmp(key, "board")) {
                    lua_pushliteral(L, "Entity(): use RPG.teleport_entity() change entity board");
                    lua_error(L);
                    return 0;
                }
                else if (!strcmp(key, "symbol") && lua_isstring(L, 3)) {
                    symbol = lua_tostring(L, 3)[0];
                }
                else if (!strcmp(key, "interact_script") && lua_isstring(L, 3)) {
                    interact = lua_tostring(L, 3);
                }
                else {
                    lua_pushliteral(L, "Attempt to assign to a nil value");
                    lua_error(L);
                    return 0;
                }
                return 0;
            }
    };
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

    class Board {
        public:
            static std::vector<Board*> boards;
            int id;
            std::string name;
            int dim[2];
            std::vector<std::shared_ptr<Entity>> entities;
            std::vector<std::shared_ptr<Portal>> portal_starts;
            std::vector<std::shared_ptr<Portal>> portal_ends;
            Board(std::string board_name, int dimX, int dimY) {
                int id_gen;
                id = id_gen;
                id_gen++;
                name = board_name;
                dim[0] = dimX;
                dim[1] = dimY;
                entities = std::vector<std::shared_ptr<Entity>>();
                portal_starts = std::vector<std::shared_ptr<Portal>>();
                portal_ends = std::vector<std::shared_ptr<Portal>>();
                boards.push_back(this);
            }
            void new_entity(std::string entity_name, int posX = 0, int posY = 0, char entity_symbol = 'X', bool is_multiplayer = false, const char* interact_file = "") {
                std::shared_ptr<Entity> ent = std::make_shared<Entity>(entity_name, posX, posY, entity_symbol, is_multiplayer, interact_file);
                ent->board_name = name;
                entities.push_back(ent);
                LUA.AddGlobalVariable(ent->name, ent);
            }
            void teleport_entity(std::shared_ptr<Entity> entity, int posX = 0, int posY = 0, Board* new_board = nullptr) {
                if (new_board != nullptr && new_board->id != id) {
                    bool found = false;
                    for (int i = 0; i < entities.size(); i++) {
                        if (entity->id == entities[i]->id) {
                            entities.erase(std::next(entities.begin(), i));
                            found = true;
                        }
                    }
                    if (!found) {
                        throw std::invalid_argument("Entity not found in board");
                    }
                    new_board->entities.push_back(entity);
                    if (posX >= 0 && posX < new_board->dim[0]) {
                        entity->pos[0] = posX;
                    }
                    if (posY >= 0 && posY < new_board->dim[1]) {
                        entity->pos[1] = posY;
                    }
                }
                else {
                    if (posX >= 0 && posX < dim[0]) {
                        entity->pos[0] = posX;
                    }
                    if (posY >= 0 && posY < dim[1]) {
                        entity->pos[1] = posY;
                    }
                }
            }
            void new_portal(std::string portal_name, int posX = 0, int posY = 0, int dest_posX = 1, int dest_posY = 1, Board* dest_board = nullptr, char portal_symbol = '0', bool is_open = true) {
                std::shared_ptr<Portal> por = std::make_shared<Portal>(portal_name, posX, posY, dest_posX, dest_posY, portal_symbol, is_open);
                portal_starts.push_back(por);
                dest_board->portal_ends.push_back(por);
            }
    };
    std::vector<Board*> Board::boards = std::vector<Board*>();
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
    static int LUA_teleport(lua_State* L) {
        int n = lua_gettop(L);
        const char* target = "$PLAYER";
        int x = RPG_PLAYER_POS[0];
        int y = RPG_PLAYER_POS[1];
        const char* board = RPG_PLAYER_BOARD->name.c_str();
        // argument gathering shit
        if (n < 2) {
            lua_pushliteral(L, "RPG.teleport: not enough arguments");
            lua_error(L);
            return 0;
        }
        else if (n == 2) { // x, y
            if (lua_isinteger(L, 1)) {
                x = lua_tointeger(L, 1);
            }
            else {
                lua_pushliteral(L, "RPG.teleport: first argument in a set of two arguments must be an int");
                lua_error(L);
                return 0;
            }
            if (lua_isinteger(L, 2)) {
                y = lua_tointeger(L, 2);
            }
            else {
                lua_pushliteral(L, "RPG.teleport: second argument in a set of two arguments must be an int");
                lua_error(L);
                return 0;
            }
        }
        else if (n == 3) { // target, x, y OR x, y, board
            if (lua_isstring(L, 1)) {
                target = lua_tostring(L, 1);
                if (lua_isinteger(L, 2)) {
                    x = lua_tointeger(L, 2);
                }
                else {
                    lua_pushliteral(L, "RPG.teleport: second argument in a set of three arguments must be an int");
                    lua_error(L);
                    return 0;
                }
                if (lua_isinteger(L, 3)) {
                    y = lua_tointeger(L, 3);
                }
                else {
                    lua_pushliteral(L, "RPG.teleport: third argument in a set of three arguments where the first argument is an string must be an int");
                    lua_error(L);
                    return 0;
                }
            }
            else if (lua_isinteger(L, 1)) {
                x = lua_tointeger(L, 1);
                if (lua_isinteger(L, 2)) {
                    y = lua_tointeger(L, 2);
                }
                else {
                    lua_pushliteral(L, "RPG.teleport: second argument in a set of three arguments must be an int");
                    lua_error(L);
                    return 0;
                }
                if (lua_isstring(L, 3)) {
                    board = lua_tostring(L, 3);
                }
                else {
                    lua_pushliteral(L, "RPG.teleport: third argument in a set of three arguments where the first argument is an int must be a string");
                    lua_error(L);
                    return 0;
                }
            }
            else {
                lua_pushliteral(L, "RPG.teleport: first argument in a set of three arguments must be an int or string");
                lua_error(L);
                return 0;
            }
        }
        else if (n == 4) { // target, x, y, board
            if (lua_isstring(L, 1)) {
                target = lua_tostring(L, 1);
            }
            else {
                lua_pushliteral(L, "RPG.teleport: first argument in a set of four arguments must be a string");
                lua_error(L);
                return 0;
            }
            if (lua_isinteger(L, 2)) {
                x = lua_tointeger(L, 2);
            }
            else {
                lua_pushliteral(L, "RPG.teleport: second argument in a set of four arguments must be an int");
                lua_error(L);
                return 0;
            }
            if (lua_isinteger(L, 3)) {
                y = lua_tointeger(L, 3);
            }
            else {
                lua_pushliteral(L, "RPG.teleport: third argument in a set of four arguments must be an int");
                lua_error(L);
                return 0;
            }
            if (lua_isstring(L, 4)) {
                board = lua_tostring(L, 4);
            }
            else {
                lua_pushliteral(L, "RPG.teleport: fourth argument in a set of four arguments must be a string");
                lua_error(L);
                return 0;
            }
        }
        else {
            lua_pushliteral(L, "RPG.teleport: too many arguments");
            lua_error(L);
            return 0;
        }
        // teleporting
        std::shared_ptr<Entity> targetpoint = nullptr;
        bool targetisportal = false;
        Board* fromboard = nullptr;
        Board* boardpoint = RPG_PLAYER_BOARD;
        if (target != "$PLAYER") {
            for (int i = 0; i < Board::boards.size(); i++) {
                for (int j = 0; j < Board::boards[i]->entities.size(); j++) {
                    if (Board::boards[i]->entities[j]->name == target) {
                        fromboard = Board::boards[i];
                        boardpoint = Board::boards[i];
                        targetpoint = Board::boards[i]->entities[j];
                    }
                }
            }
        }
        if (board != RPG_PLAYER_BOARD->name) {
            for (int i = 0; i < Board::boards.size(); i++) {
                if (board == Board::boards[i]->name) {
                    boardpoint = Board::boards[i];
                }
            }
        }
        if (targetpoint == nullptr) {
            RPG_PLAYER_teleport(x, y, boardpoint);
        }
        else {
            fromboard->teleport_entity(targetpoint, x, y, boardpoint);
        }
        return 0;
    }
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
        std::shared_ptr<Entity> target = nullptr;
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
        rpglib->AddCFunction("teleport", LUA_teleport);
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