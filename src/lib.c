#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include "lib.h"


void init_player(Player* player){

    /* Legs */
    player->blocks[0].x = PLAYER_X-1; player->blocks[0].y = 1;
    player->blocks[1].x = PLAYER_X+1; player->blocks[1].y = 1;

    /* Body */
    player->blocks[2].x = PLAYER_X-2; player->blocks[2].y = 2;
    player->blocks[3].x = PLAYER_X-1; player->blocks[3].y = 2;
    player->blocks[4].x = PLAYER_X  ; player->blocks[4].y = 2;
    player->blocks[5].x = PLAYER_X+1; player->blocks[5].y = 2;
    player->blocks[6].x = PLAYER_X+2; player->blocks[6].y = 2;

    /* Neck */
    player->blocks[7].x = PLAYER_X+1 ; player->blocks[7].y = 3;
    player->blocks[8].x = PLAYER_X+2 ; player->blocks[8].y = 3;

    /* Face */
    player->blocks[9].x = PLAYER_X+2 ; player->blocks[9].y = 4;
    player->blocks[10].x = PLAYER_X+3; player->blocks[10].y = 4;

    /* Not jumping initially */
    player->jump_state = 0;
}



void fill_board(char** board) {
    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < NUM_COLS; j++) {
            board[i][j] = ' ';
            if(i == 0) {board[i][j] = '~';}         /* Floor */
        }
    }
}



void check_jump(Player* player) {

    /* Going up */
    if (1 <= player->jump_state && player->jump_state <= LEN_JUMP/2) {
        if (player->jump_state % 2 == 1) {
            for (int i = 0; i < PLAYER_DIM; i++) {
                player->blocks[i].y += 1;
            }
        }
    }

    /* Going down */
    if (LEN_JUMP/2 <= player->jump_state && player->jump_state < LEN_JUMP) {
        if (player->jump_state % 2 == 1) {
            for (int i = 0; i < PLAYER_DIM; i++) {
                player->blocks[i].y -= 1;
            }
        }
    }

    if (player->jump_state == LEN_JUMP) {
        player->jump_state = 0;
    }
    
}



void draw_player(char** board, Player player){
    for (int i = 0; i < PLAYER_DIM; i++) {
        board[player.blocks[i].y][player.blocks[i].x] = '+';
    }
}



void print_board(char** board) {

    for (int i = NUM_ROWS - 1; i >= 0; i--) {
        for(int j = 0; j < NUM_COLS; j++) {

            switch(board[i][j]) {
                case '~':
                    printf("\x1b[46m%c", ' ');
                    printf("\x1b[%dm", 0);
                    break;
                case '#':
                    printf("\x1b[42m%c", ' ');
                    printf("\x1b[%dm", 0);
                    break;
                case '+':
                    printf("\x1b[44m%c", ' ');
                    printf("\x1b[%dm", 0);
                    break;
                default :
                    putchar(board[i][j]);
            }

        }
        putchar('\n');
    }
    
}



void check_keyboard(Player* player, int input, int* quit){
    if (input == 'q') {
        *quit = 1;
    }
    if (input == 'b' && player->jump_state == 0) {
        player->jump_state = 1;
    }
}



void add_cactus(NodeCactus** head){
    /* Add an element at the beginning of the list */

    NodeCactus* new = malloc(sizeof(*new));

    for(int i = 0; i < CACTUS_WIDTH; i++) {
        for(int j = 0; j < CACTUS_HEIGHT; j++) {

            new->cactus.blocks[i + CACTUS_WIDTH * j].x = NUM_COLS - 1 + i;
            new->cactus.blocks[i + CACTUS_WIDTH * j].y = j + 1;
            
        }
    }

    if (*head == NULL) {
        new->next = NULL;
    } else {
        new->next = *head;
    }
    
    *head = new;
}



int remove_cactus(NodeCactus** head){
    /* Remove the end of the list */

    NodeCactus* current = *head;
    NodeCactus* prev = NULL;

    if (current == NULL) {
        return 0;
    }

    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }

    if (prev == NULL) {
        *head = NULL;
    } else {
        prev->next = current->next;
    }

    free(current);
    return 1;

}



int move_cactus(NodeCactus** head, int* score){

    NodeCactus* tmp = *head;
    NodeCactus* last = NULL;

    if(tmp == NULL) { return -1; }

    /* Shift cactus */
    while (tmp != NULL) {

        for(int i = 0; i < CACTUS_HEIGHT * CACTUS_WIDTH; i++) {
            tmp->cactus.blocks[i].x -= VELOCITY_CACTUS;
        }

        if (tmp->cactus.blocks[CACTUS_WIDTH - 1].x == PLAYER_X) {
            *score += 1;            //CHECK HOW YOU EVALUATE THE SCORE
        }

        last = tmp;
        tmp = tmp->next;
    }

    /* If the cactus reaches the end of the board, then remove it from the list */
    if(last->cactus.blocks[CACTUS_WIDTH - 1].x < 0) {
        return 1;
    }

    return 0;
    
}



void draw_cactus(NodeCactus** head, char** board){

    NodeCactus* tmp = *head;

    while (tmp != NULL) {

        for(int i = 0; i < CACTUS_WIDTH * CACTUS_HEIGHT; i++) {
            if ( 0 <= tmp->cactus.blocks[i].x && tmp->cactus.blocks[i].x < NUM_COLS) {
                board[tmp->cactus.blocks[i].y][tmp->cactus.blocks[i].x] = '#';
            }
        }

        tmp = tmp->next;
    }

}



void check_death(Player player, NodeCactus** head_cactus, NodePtero** head_ptero, int* gameover) {

    NodeCactus* tmp = *head_cactus;
    while (tmp != NULL) {
        for (int i = 0; i < PLAYER_DIM; i++) {
            for (int j = 0; j < CACTUS_HEIGHT * CACTUS_WIDTH; j++){
                if (player.blocks[i].x == tmp->cactus.blocks[j].x && player.blocks[i].y == tmp->cactus.blocks[j].y) {
                    *gameover = 1;
                }
            }
        }
        tmp = tmp->next;
    }

    NodePtero* tmp_ptero = *head_ptero;
    while (tmp_ptero != NULL) {
        for (int i = 0; i < PLAYER_DIM; i++) {
            for (int j = 0; j < PTERODACTYL_HEIGHT * PTERODACTYL_WIDTH; j++){
                if (player.blocks[i].x == tmp_ptero->pterodactly.blocks[j].x && player.blocks[i].y == tmp_ptero->pterodactly.blocks[j].y) {
                    *gameover = 1;
                }
            }
        }
        tmp_ptero = tmp_ptero->next;
    }

}



void add_pterodactyl(NodePtero** head){
    /* Add an element at the beginning of the list */

    NodePtero* new = malloc(sizeof(*new));
    int y = 5 * ( rand() % 2 ) + 1;

    for(int i = 0; i < PTERODACTYL_WIDTH; i++) {
        for(int j = 0; j < PTERODACTYL_HEIGHT; j++) {
            new->pterodactly.blocks[i + PTERODACTYL_WIDTH * j].x = NUM_COLS - 1 + i;
            new->pterodactly.blocks[i + PTERODACTYL_WIDTH * j].y = y + j;
        }
    }

    if (*head == NULL) {
        new->next = NULL;
    } else {
        new->next = *head;
    }
    
    *head = new;
}



int remove_pterodactly(NodePtero** head){
    /* Remove the end of the list */

    NodePtero* current = *head;
    NodePtero* prev = NULL;

    if (current == NULL) {
        return 0;
    }

    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }

    if (prev == NULL) {
        *head = NULL;
    } else {
        prev->next = current->next;
    }

    free(current);
    return 1;
    
}



int move_pterodactyl(NodePtero** head, int* score){

    NodePtero* tmp = *head;
    NodePtero* last = NULL;

    if(tmp == NULL) { return -1; }

    /* Shift pterodactyl */
    while (tmp != NULL) {
        for(int i = 0; i < PTERODACTYL_HEIGHT * PTERODACTYL_WIDTH; i++) {
            tmp->pterodactly.blocks[i].x -= VELOCITY_PTERODACTYL;
        }

        if (tmp->pterodactly.blocks[PTERODACTYL_WIDTH - 1].x == PLAYER_X) {
            *score += 1;
        }

        last = tmp;
        tmp = tmp->next;
    }

    /* If the pterodactyl reaches the end of the board, then remove it from the list */
    if(last->pterodactly.blocks[PTERODACTYL_WIDTH - 1].x < 0) {
        return 1;
    }

    return 0;
    
}



void draw_pterodactyl(NodePtero** head, char** board){

    NodePtero* tmp = *head;

    while (tmp != NULL) {
        
        /* Beak */
        if (0 <= tmp->pterodactly.blocks[0 + PTERODACTYL_WIDTH * 0].x && tmp->pterodactly.blocks[0 + PTERODACTYL_WIDTH * 0].x < NUM_COLS) {
            board [tmp->pterodactly.blocks[0 + PTERODACTYL_WIDTH * 0].y][tmp->pterodactly.blocks[0 + PTERODACTYL_WIDTH * 0].x] = ' ';
            board [tmp->pterodactly.blocks[0 + PTERODACTYL_WIDTH * 1].y][tmp->pterodactly.blocks[0 + PTERODACTYL_WIDTH * 1].x] = '<';
            board [tmp->pterodactly.blocks[0 + PTERODACTYL_WIDTH * 2].y][tmp->pterodactly.blocks[0 + PTERODACTYL_WIDTH * 2].x] = ' ';
        }

        /* Head */
        if (0 <= tmp->pterodactly.blocks[1 + PTERODACTYL_WIDTH * 0].x && tmp->pterodactly.blocks[1 + PTERODACTYL_WIDTH * 0].x < NUM_COLS) {
            board [tmp->pterodactly.blocks[1 + PTERODACTYL_WIDTH * 0].y][tmp->pterodactly.blocks[1 + PTERODACTYL_WIDTH * 0].x] = ' ';
            board [tmp->pterodactly.blocks[1 + PTERODACTYL_WIDTH * 1].y][tmp->pterodactly.blocks[1 + PTERODACTYL_WIDTH * 1].x] = 'O';
            board [tmp->pterodactly.blocks[1 + PTERODACTYL_WIDTH * 2].y][tmp->pterodactly.blocks[1 + PTERODACTYL_WIDTH * 2].x] = ' ';
        }

        /* Body + wings */
        if (0 <= tmp->pterodactly.blocks[2 + PTERODACTYL_WIDTH * 0].x && tmp->pterodactly.blocks[2 + PTERODACTYL_WIDTH * 0].x < NUM_COLS) {
            board [tmp->pterodactly.blocks[2 + PTERODACTYL_WIDTH * 0].y][tmp->pterodactly.blocks[2 + PTERODACTYL_WIDTH * 0].x] = '`';
            board [tmp->pterodactly.blocks[2 + PTERODACTYL_WIDTH * 1].y][tmp->pterodactly.blocks[2 + PTERODACTYL_WIDTH * 1].x] = '-';
            board [tmp->pterodactly.blocks[2 + PTERODACTYL_WIDTH * 2].y][tmp->pterodactly.blocks[2 + PTERODACTYL_WIDTH * 2].x] = ',';
        }

        for (int i = 3; i < PTERODACTYL_WIDTH - 1; i++) {
            if (0 <= tmp->pterodactly.blocks[i + PTERODACTYL_WIDTH * 0].x && tmp->pterodactly.blocks[i + PTERODACTYL_WIDTH * 0].x < NUM_COLS) {
                for (int j = 0; j < PTERODACTYL_HEIGHT; j++) {
                    board [tmp->pterodactly.blocks[i + PTERODACTYL_WIDTH * j].y][tmp->pterodactly.blocks[i + PTERODACTYL_WIDTH * j].x] = '-';
                }
            } 
        }

        /* tail */
        if (0 <= tmp->pterodactly.blocks[PTERODACTYL_WIDTH - 1 + PTERODACTYL_WIDTH * 0].x  && tmp->pterodactly.blocks[PTERODACTYL_WIDTH - 1 + PTERODACTYL_WIDTH * 0].x < NUM_COLS) {
            board [tmp->pterodactly.blocks[PTERODACTYL_WIDTH - 1 + PTERODACTYL_WIDTH * 0].y][tmp->pterodactly.blocks[PTERODACTYL_WIDTH - 1 + PTERODACTYL_WIDTH * 0].x] = ' ';
            board [tmp->pterodactly.blocks[PTERODACTYL_WIDTH - 1 + PTERODACTYL_WIDTH * 1].y][tmp->pterodactly.blocks[PTERODACTYL_WIDTH - 1 + PTERODACTYL_WIDTH * 1].x] = '<';
            board [tmp->pterodactly.blocks[PTERODACTYL_WIDTH - 1 + PTERODACTYL_WIDTH * 2].y][tmp->pterodactly.blocks[PTERODACTYL_WIDTH - 1 + PTERODACTYL_WIDTH * 2].x] = ' ';
        }
        
        tmp = tmp->next;

    }

}