#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <termios.h>
#include "lib.h"


/* chrome-like dinosaur for LINUX terminal */


int main (int argc, char **argv) {

	/* Hide cursor */
	printf("\x1b[?25l");

	/* Remove echo */
	struct termios old_t, new_t;
	tcgetattr(STDERR_FILENO, &old_t);
	new_t = old_t;
	new_t.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_t);

	/* Clear screen and move cursor to top-left (line 15, column 1) */
	printf("\x1b[%dJ", 2);
   	printf("\x1b[%d;%df", 15, 1);
	
	/* Variables */
	Player player;
	int gameover = 0;
	int quit = 0;
	int win = 0;
	int score = 0;
	int pause_obstacles = 0;
	int len;
	char score_buffer[SCORE_BUFFER_SIZE];
	char input_buffer[INPUT_BUFFER_SIZE];

	char** board;
	board = malloc(NUM_ROWS * sizeof(char*));
	for (int i = 0; i < NUM_ROWS; i++) {
		board[i] = malloc(NUM_COLS * sizeof(char));
	}

	NodeCactus* head_cactus = NULL;
	NodePtero* head_ptero = NULL;
	
	/* Initialize player */
	init_player(&player);
	
	printf("\nCommands :\n\n");
	printf("\tb : jump\n");
	printf("\tq : quit\n\n");

	/* Here the level starts */
	srand(time(NULL));

	while (! gameover && ! quit && ! win) {

		/* Fill the board */
		fill_board(board);

		/* Adding cactus or pterodactyl */
		if (pause_obstacles == 0) {
			if (rand() % 5 == 0) {
				add_cactus(&head_cactus);
				pause_obstacles = FRAMES_BETWEEN_OBSTACLES;
			}
		}
		if (pause_obstacles == 0) {
			if (rand() % 8 == 0) {
				add_pterodactyl(&head_ptero);
				pause_obstacles = FRAMES_BETWEEN_OBSTACLES;
			}
		}
		if (pause_obstacles > 0) {
			pause_obstacles--;
		}

		/* Place the player */
		check_jump(&player);
		if (player.jump_state != 0) {player.jump_state++;}
		draw_player(board, player);

		/* Move the cactus */
		int bool_remove = move_cactus(&head_cactus, &score);
		if (bool_remove == 1) {
			remove_cactus(&head_cactus);
		}

		/* Move the pterodactyl */
		bool_remove = move_pterodactyl(&head_ptero, &score);
		if (bool_remove == 1) {
			remove_pterodactly(&head_ptero);
		}

		/* Draw the cactus */
		if(head_cactus != NULL) { draw_cactus(&head_cactus, board); }

		/* Draw the pterodactyl */
		if(head_ptero != NULL) { draw_pterodactyl(&head_ptero, board); }

		/* Check if the player is dead */
		check_death(player, &head_cactus, &head_ptero, &gameover);

		/* Print the board */
		print_board(board);
		int len_score_buf = snprintf(score_buffer, SCORE_BUFFER_SIZE, "\nScore : %d", score);
		puts(score_buffer);

		/* Pause: more points, game faster */
		if (score < 20) {
			usleep(3 * 1000000 / 60);
		}
		if (20 <= score && score < 40) {
			usleep((int) (2.5 * 1000000 / 60));
		}
		if (40 <= score && score < 60 ) {
			usleep(2 * 1000000 / 60);
		}
		if (60 <= score) {
			usleep(1.5 * 1000000 / 60);
		}
		

		/* Make a move */
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);
		select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
		
		if(FD_ISSET(STDIN_FILENO, &fds)) {
			len = read(STDIN_FILENO, input_buffer, INPUT_BUFFER_SIZE);
			int input = input_buffer[len - 1];
			check_keyboard(&player, input, &quit);
		}

		/* Clean stuff */
		if( ! gameover && ! quit ) {
			fflush(stdout);
			printf("\x1b[%dD", len_score_buf);
			printf("\x1b[%dA", NUM_ROWS + 2);
			printf("\x1b[%dJ", 0);
		}

	}

	if (gameover) {
		printf("\n\nGameover!\n\nYour final score is : %d\n\n", score);
	}
	
	if(quit) {
		printf("\n\nThank you for having played with this game! Let me know if you have suggestions to improve it!\n\n");
	}

	/* Free board */
	for (int i = 0; i < NUM_ROWS; i++) {
		free(board[i]);
	}
	free(board);

	/* Free cactus list */
	while(remove_cactus(&head_cactus)){}

	/* Free pterodactyl list */
	while(remove_pterodactly(&head_ptero)){}
	
	/* Display cursor */
	printf("\x1b[?25h");

	/* Return to echo mode */
	tcsetattr(STDIN_FILENO, TCSANOW, &old_t);
	
	return 0;
}

