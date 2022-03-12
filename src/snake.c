#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "snake.h"

int main() {

	play_game();
    return 1;
}

void play_game() {
    int field[GAME_ROWS][GAME_COLS];
    int go_game = TRUE;
    struct termios savetty;
    struct timespec time_start, time_now;
	struct snake snake;

	init_game(field, &snake);
	set_icanon(&savetty);
	see_start();
	clock_gettime(CLOCK_REALTIME, &time_start);
    while (go_game) {
        clock_gettime(CLOCK_REALTIME, &time_now);
        if (get_delta_time(time_start, time_now, snake.direction) > snake.delay) {
            get_command(&snake, &go_game);
            calculate_snake(field, &snake, &go_game);
            draw_game(field);
            time_start = time_now;
        }
		if (snake.length == END_SCORE)
			go_game = FALSE;
    }
	if (snake.length == END_SCORE)
		see_finish_win(snake.length);
	else
		see_finish_fail(snake.length);
    end_game(savetty);
}

void init_game(int matrix[GAME_ROWS][GAME_COLS], struct snake *snake) {
	FILE *f = fopen("map2.txt", "r");
    for (int row = 0; row < GAME_ROWS; row++) {
        for (int col = 0; col < GAME_COLS; col++)
			fscanf(f, "%d",&matrix[row][col]);
    }
	fclose(f);

    for (int row = 0; row < GAME_ROWS; row++)
        for (int col = 0; col < GAME_COLS; col++) {
			if (row == 0 || col == 0 || row == GAME_ROWS-1 || col == GAME_COLS-1)  {
				matrix[row][col] = GAME_WALL;	
			} 
			// else {
			// 	matrix[row][col] = GAME_SPACE;
			// }	
		}
    
    matrix[GAME_ROWS/2-1][GAME_COLS/2+1] = GAME_HEAD_SNAKE;

	snake->head_y = GAME_ROWS/2-1;
	snake->head_x = GAME_COLS/2+1;
	snake->length = 1;
	snake->delay = START_DELAY;
	snake->direction = DIR_LEFT;

	set_food(matrix, 0, *snake);
	set_stone(matrix, *snake);

}

void set_icanon(struct termios *savetty) {
    struct termios tty;
    tcgetattr(0, &tty);
    *savetty = tty;
    tty.c_lflag &= ~(ICANON|ECHO);
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;
    tcsetattr(0, TCSAFLUSH, &tty);
}

void draw_game(int matrix[GAME_ROWS][GAME_COLS]) {
    printf("\033c");
    for (int row = 0; row < GAME_ROWS; row++) {
        for (int col = 0; col < GAME_COLS; col++) {
            if (matrix[row][col] == GAME_WALL) {
                char s[4];
                get_part_wall( matrix, row, col, s);
                printf("\033[46;31m%s",s);
			}
            else if (matrix[row][col] >= GAME_HEAD_SNAKE) {
                char s[4];
                get_part_snake( matrix, row, col, s);
                printf("\033[46;31m%s",s);
            } else if (matrix[row][col] == GAME_FOOD) {
                printf("\033[46;30m%c", SHOW_FOOD);
            } else if (matrix[row][col] == GAME_STONE) {
                printf("\033[46;30m%c", SHOW_STONE);
            } else {
                printf("\033[46m ");
            }
        }
        printf("\n");
    }
}

void end_game(struct termios savetty) {
    tcsetattr(0, TCSANOW, &savetty);
}

void get_part_wall(int grid[GAME_ROWS][GAME_COLS], int row, int col, char *s) {
    int left = col-1 >= 0 ? grid[row][col-1] : GAME_SPACE;
    int right = col+1 < GAME_COLS ? grid[row][col+1] : GAME_SPACE;
    int up = row-1 >= 0 ? grid[row-1][col] : GAME_SPACE;
    int down = row+1 < GAME_ROWS ? grid[row+1][col] : GAME_SPACE;
	
    if (left == GAME_WALL && right == GAME_WALL)
        strcpy(s, "─");
    else if (up == GAME_WALL && down == GAME_WALL) 
        strcpy(s, "│");
    else if (right == GAME_WALL && down == GAME_WALL) 
        strcpy(s, "┌");    
    else if (up == GAME_WALL && right == GAME_WALL) 
        strcpy(s, "└");
    else if (left == GAME_WALL && down == GAME_WALL) 
        strcpy(s, "┐");
    else if (up == GAME_WALL && left == GAME_WALL) 
        strcpy(s, "┘");
	else if ((left == GAME_WALL && right == GAME_SPACE) || 
			 (left == GAME_SPACE && right == GAME_WALL))
		strcpy(s, "─");
	else if ((up == GAME_WALL && down == GAME_SPACE) ||
			 (up == GAME_SPACE && down == GAME_WALL))
		strcpy(s, "│");
}

void get_part_snake(int grid[GAME_ROWS][GAME_COLS], int row, int col, char *s) {
    int left = grid[row][col-1];
    int right = grid[row][col+1];
    int up = grid[row-1][col];
    int down = grid[row+1][col];
    int minus = grid[row][col]-1;
    int plus = grid[row][col]+1;
    
    if (grid[row][col] == GAME_HEAD_SNAKE) 
        strcpy(s, "█");
    else if ((left == minus && right == plus) || (left == plus && right == minus))
        strcpy(s, "═");
    else if ((up == minus && down == plus) || (up == plus && down == minus))
        strcpy(s, "║");
    else if ((right == minus && down == plus) || (right == plus && down == minus))
        strcpy(s, "╔");    
    else if ((up == minus && right == plus) || (up == plus && right == minus))
        strcpy(s, "╚");
    else if ((left == minus && down == plus) || (left == plus && down == minus))
        strcpy(s, "╗");
    else if ((up == minus && left == plus) || (up == plus && left == minus))
        strcpy(s, "╝");
    else if (up == minus)
        strcpy(s, "║");//strcpy(s, "╨");
    else if (down == minus)
        strcpy(s, "║");//strcpy(s, "╥");
    else if (left == minus)
        strcpy(s, "═");//strcpy(s, "╡");
    else if (right == minus)
        strcpy(s, "═");//strcpy(s, "╞");
}

void get_command(struct snake *snake, int *isGame) {
    char control;

    if (read(0, &control, 1)) {
        if (control == KEY_LEFT)
            snake->direction = is_direction(snake->direction, DIR_LEFT) ? DIR_LEFT : snake->direction;
        else if (control == KEY_UP)
            snake->direction = is_direction(snake->direction, DIR_UP) ? DIR_UP : snake->direction;
        else if (control == KEY_RIGHT)
            snake->direction = is_direction(snake->direction, DIR_RIGHT) ? DIR_RIGHT : snake->direction;
        else if (control == KEY_DOWN)
            snake->direction = is_direction(snake->direction, DIR_DOWN) ? DIR_DOWN : snake->direction;
        else if (control == KEY_ESC)
            *isGame = FALSE;
    }
	// printf("snake->direction = %d\n", snake->direction);
	// press_any_key();
}

int is_direction(int direction, int new_direction) {
    if ((direction == DIR_UP && new_direction == DIR_DOWN) ||
        (direction == DIR_DOWN && new_direction == DIR_UP) ||
        (direction == DIR_LEFT && new_direction == DIR_RIGHT) ||
        (direction == DIR_RIGHT && new_direction == DIR_LEFT) )
        return FALSE;
    else
        return TRUE;
}

void calculate_snake(int frame[GAME_ROWS][GAME_COLS], struct snake *snake, int *is_game) {
    int is_snake_eat = FALSE;
    int next_snake_x = snake->head_x;
    int next_snake_y = snake->head_y;

    if (snake->direction == DIR_LEFT)
        snake->head_x = snake->head_x-1;

    if (snake->direction == DIR_RIGHT)
        snake->head_x = snake->head_x+1;

    if (snake->direction == DIR_UP)
        snake->head_y = snake->head_y-1;

    if (snake->direction == DIR_DOWN)
        snake->head_y = snake->head_y+1;

    if (frame[snake->head_y][snake->head_x] == GAME_WALL || 
        frame[snake->head_y][snake->head_x] == GAME_STONE || 
        frame[snake->head_y][snake->head_x] >= GAME_HEAD_SNAKE ) {
            *is_game = FALSE;
			frame[snake->head_y][snake->head_x] = GAME_SPACE;
        }    

    if (frame[snake->head_y][snake->head_x] == GAME_FOOD) {
        is_snake_eat = TRUE;
        frame[snake->head_y][snake->head_x] = GAME_SPACE;
    }

    if (frame[snake->head_y][snake->head_x] == GAME_SPACE) {
        frame[snake->head_y][snake->head_x] = GAME_HEAD_SNAKE;

        for (int i = 1; i < snake->length; i++) {
            frame[next_snake_y][next_snake_x] = GAME_HEAD_SNAKE+i;

            if ( frame[next_snake_y-1][next_snake_x] == frame[next_snake_y][next_snake_x] ) 
                next_snake_y = next_snake_y-1;
            else if ( frame[next_snake_y+1][next_snake_x] == frame[next_snake_y][next_snake_x] ) 
                next_snake_y = next_snake_y+1;
            else if ( frame[next_snake_y][next_snake_x-1] == frame[next_snake_y][next_snake_x] ) 
                next_snake_x = next_snake_x-1;
            else if ( frame[next_snake_y][next_snake_x+1] == frame[next_snake_y][next_snake_x] ) 
                next_snake_x = next_snake_x+1;
        }

        if (is_snake_eat) {
            frame[next_snake_y][next_snake_x] = GAME_HEAD_SNAKE + snake->length;
            (snake->length)++;
			set_food(frame, MAX_FOOD-1, *snake);
			snake->delay -= (snake->delay)/20;
        } else {
            frame[next_snake_y][next_snake_x] = GAME_SPACE;
        }
    }
}

void set_food(int matrix[GAME_ROWS][GAME_COLS], int count, struct snake snake) {
	srand(time(0));
	while (count < MAX_FOOD) {
	    int y = rand()%GAME_ROWS;
	    int x = rand()%GAME_COLS;

		if (matrix[y][x] == GAME_SPACE && 
			sqrt(pow(snake.head_y-y, 2) + pow(snake.head_x-x, 2)) > DISTANSE_FROM_HEAD) {
			matrix[y][x] = GAME_FOOD;
			count++;
		}
	}
}

void set_stone(int matrix[GAME_ROWS][GAME_COLS], struct snake snake) {
	srand(time(0));
	int count = 0;
	while (count < MAX_STONE) {
	    int y = rand()%GAME_ROWS;
	    int x = rand()%GAME_COLS;

		if (matrix[y][x] == GAME_SPACE && 
			sqrt(pow(snake.head_y-y, 2) + pow(snake.head_x-x, 2)) > DISTANSE_FROM_HEAD) {
			matrix[y][x] = GAME_STONE;
			count++;
		}
	}
}

void see_finish_win(int score) {
	printf("\033c");
	printf("             ╖╓                                                                 \n");
	printf("      ╔══════╨╨══════╗                                                          \n");
	printf("      ║     o  o     ║                             ╔═════════════╗              \n");
	printf("      ║   ╳      ╳   ║                             ║    ░░░   ╔══╝              \n");
	printf("      ║              ║                             ║ ░░    ╔══╝                 \n");
	printf("      ║ ╎╎╎     ╎╎╎  ║                             ║    ▒  ║                    \n");
	printf("      ╚═══╗      ╔═══╝                             ║ ▒▒    ║                    \n");
	printf("          ║░     ║                                 ║       ║\n");
	printf("          ║     ░║        ╔════════════════╗       ║    ▒▒▒║\n");
	printf("          ║      ║        ║    ░░░         ║       ║ ▒▒▒   ║\n");
	printf("          ║░░░   ╚════════╝           ░░░  ╚═══════╝       ║\n");
	printf("          ║ ░░░       ░░░░      ░░          ░░░     ▒▒     ║\n");
	printf("          ║                   ░░░░░      ░░░        ▒▒▒▒   ║\n");
	printf("          ╚════════════════════════════════════════════════╝\n");
	printf("\n");                                                                         
	printf("  ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄  \n");
	printf("   _______  _______  _______  _______    _______           _______  _______     \n");
	printf("  (  ____ )(  ___  )(       )(  ____ )  (  ___  )|)     /|(  ____ )(  ____ )    \n");
	printf("  | (    )/| (   ) || () () || (    )/  | (   ) || )   ( || (    )/| (    )|    \n");
	printf("  | |      | (___) || || || || (__      | |   | || |   | || (__    | (____)|    \n");
	printf("  | | ____ |  ___  || |(_)| ||  __)     | |   | |( (   ) )|  __)   |     __)    \n");
	printf("  | | )_  )| (   ) || |   | || (        | |   | | ) )_/ / | (      | () (       \n");
	printf("  | (___) || )   ( || )   ( || (____/)  | (___) |  )   /  | (____/)| ) ) )__    \n");
	printf("  (_______)|/     )||/     )|(_______/  (_______)   )_/   (_______/|/   )__/    \n");
	printf("  ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄  \n");
	printf("                                YOUR SCORE: %02d                               \n", score);
}

void see_finish_fail(int score) {
	printf("\033c");
	printf("             ╖╓                                                                 \n");
	printf("      ╔══════╨╨══════╗                                                          \n");
	printf("      ║     o  o     ║                                                          \n");
	printf("      ║   ╳      ╳   ║                                                          \n");
	printf("      ║              ║                  ╔═══════╗                               \n");
	printf("      ║ ╎╎╎     ╎╎╎  ║                  ║░ ╔════╝                               \n");
	printf("      ╚═════╗   ╔════╝                  ║  ║                                    \n");
	printf("            ║░  ║            ╔══════════╝  ║                                    \n");
	printf("            ║  ░║            ║░  ╔═════════╝                                     \n");
	printf("            ║   ║            ║   ║                                   \n");
	printf("            ║░░░║            ║░░ ║                                             \n");
	printf("            ║ ░░╚══(((((((═══╝   ║                                             \n");
	printf("            ╚══════(((((((═══════╝                                             \n");
	printf("\n");
	printf("\n");
	printf("  ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄  \n");
	printf("   _______  _______  _______  _______    _______           _______  _______     \n");
	printf("  (  ____ )(  ___  )(       )(  ____ )  (  ___  )|)     /|(  ____ )(  ____ )    \n");
	printf("  | (    )/| (   ) || () () || (    )/  | (   ) || )   ( || (    )/| (    )|    \n");
	printf("  | |      | (___) || || || || (__      | |   | || |   | || (__    | (____)|    \n");
	printf("  | | ____ |  ___  || |(_)| ||  __)     | |   | |( (   ) )|  __)   |     __)    \n");
	printf("  | | )_  )| (   ) || |   | || (        | |   | | ) )_/ / | (      | () (       \n");
	printf("  | (___) || )   ( || )   ( || (____/)  | (___) |  )   /  | (____/)| ) ) )__    \n");
	printf("  (_______)|/     )||/     )|(_______/  (_______)   )_/   (_______/|/   )__/    \n");
	printf("  ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄  \n");
	printf("                                YOUR SCORE: %02d                               \n", score);
}

void see_start() {
	printf("\033c");
	printf("                                                                                \n");
	printf("                                                                                \n");
	printf("                         YOU ARE IN THE SUPER GAME                              \n");
	printf("                                                                                \n");
	printf("                                                                                \n");
	printf("                  ╔═══╗   ╔═╗─╔╗   ╔═══╗   ╔╗╔═╗   ╔═══╗                        \n");
	printf("                  ║╔═╗║   ║║╚╗║║   ║╔═╗║   ║║║╔╝   ║╔══╝                        \n");
	printf("                  ║╚══╗   ║╔╗╚╝║   ║║─║║   ║╚╝╝─   ║╚══╗                        \n");
	printf("                  ╚══╗║   ║║╚╗║║   ║╚═╝║   ║╔╗║─   ║╔══╝                        \n");
	printf("                  ║╚═╝║   ║║─║║║   ║╔═╗║   ║║║╚╗   ║╚══╗                        \n");
	printf("                  ╚═══╝   ╚╝─╚═╝   ╚╝─╚╝   ╚╝╚═╝   ╚═══╝                        \n");
	printf("                                                                                \n");
	printf("                             ╔══════════════╗                                   \n");
	printf("                             ║ ╎╎╎     ╎╎╎  ║                                   \n");
	printf("                             ║              ║                                   \n");
	printf("                             ║   ╳      ╳   ║                                   \n");
	printf("                             ║     o  o     ║                                   \n");
	printf("                             ╚══════╥╥══════╝                                   \n");
	printf("                                    ╜╙                                          \n");
	printf("                               LET'S PLAY? ;)                                   \n");
	printf("                                                                                \n");
	printf("              FROM CREATORS PLAY GAME PONG ANG GAME OF LIFE                     \n");
	printf("        @timonade   @artanisv   @antwantu   @santiago   @clymeneb               \n");
	printf("                        21-school_novosibirsk_2022                              \n");
	printf("                                                                                \n");
	printf("                              \033[2;5mpress any key....                                 \n");
	press_any_key();
}

long int get_delta_time(struct timespec time_start, struct timespec time_now, int direction) {
    long int delta_time = (time_now.tv_sec - time_start.tv_sec) * 1e9 + (time_now.tv_nsec - time_start.tv_nsec);
    if (direction == DIR_DOWN || direction == DIR_UP) {
        delta_time /= 1.5;
    }
	return delta_time;
}

void press_any_key() {
	while(TRUE) {
		char ch;
		if (read(0, &ch, 1))
			break;
	}
}
