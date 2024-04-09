#ifndef __LIB_H__
#define __LIB_H__


/* Constants */
#define NUM_ROWS 15
#define NUM_COLS 70
#define PLAYER_X 30
#define FRAMES_BETWEEN_OBSTACLES 26
#define LEN_JUMP 22

#define PLAYER_DIM 11
#define CACTUS_HEIGHT 3
#define CACTUS_WIDTH 3
#define PTERODACTYL_HEIGHT 3
#define PTERODACTYL_WIDTH 5
#define VELOCITY_CACTUS 1
#define VELOCITY_PTERODACTYL 1
#define SCORE_BUFFER_SIZE 256
#define INPUT_BUFFER_SIZE 1028



/* Typedef */

typedef struct {
    int x,y;
} Block;


typedef struct {
    Block blocks[PLAYER_DIM];
    int jump_state;
} Player;


typedef struct {
    Block blocks[CACTUS_HEIGHT * CACTUS_WIDTH];
} Cactus;


typedef struct NodeCactus{
    Cactus cactus;
    struct NodeCactus* next;
} NodeCactus;


typedef struct {
    Block blocks[PTERODACTYL_WIDTH * PTERODACTYL_HEIGHT];
} Pterodactyl;


typedef struct NodePtero {
    Pterodactyl pterodactly;
    struct NodePtero* next;
} NodePtero;



/* Headers of functions */

void init_player(Player* player);

void fill_board(char** board);

void check_jump(Player* player);

void draw_player(char** board, Player player);

void print_board(char** board);

void check_keyboard(Player* player, int input, int* quit);

void add_cactus(NodeCactus** head);

int remove_cactus(NodeCactus** head);

int move_cactus(NodeCactus** head, int* score);

void draw_cactus(NodeCactus** head, char** board);

void check_death(Player player, NodeCactus** head_cactus, NodePtero** head_ptero, int* gameover);

void add_pterodactyl(NodePtero** head);

int remove_pterodactly(NodePtero** head);

int move_pterodactyl(NodePtero** head, int* score);

void draw_pterodactyl(NodePtero** head, char** board);

#endif

