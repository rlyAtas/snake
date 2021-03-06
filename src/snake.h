#ifndef SRC_SNAKE_H_
#define SRC_SNAKE_H_

#define TRUE 1
#define FALSE 0

#define KEY_ESC 27
#define KEY_RESTART 'r'
#define KEY_LEFT 0x445b1b
#define KEY_UP 0x415b1b
#define KEY_RIGHT 0x435b1b
#define KEY_DOWN 0x425b1b

#define DIR_LEFT 1
#define DIR_UP 2
#define DIR_RIGHT 3
#define DIR_DOWN 4

#define GAME_ROWS 25
#define GAME_COLS 80
#define GAME_SPACE 0
#define GAME_WALL 1
#define GAME_STONE 2
#define GAME_FOOD 3
#define GAME_HEAD_SNAKE 10

#define MAX_STONE 20
#define SHOW_STONE '*'
#define MAX_FOOD 10
#define SHOW_FOOD 'o'
#define DISTANSE_FROM_HEAD 10

#define START_DELAY 200000000
#define END_SCORE 6

#define MAX_LEVEL 9
#define FILE_NAME "../maps/map"
#define FILE_EXT ".txt"

struct snake {
    int head_y;
    int head_x;
    int length;
    int delay;
    int direction;
};

void init_game(int[GAME_ROWS][GAME_COLS], struct snake*, int);
void set_icanon(struct termios*);
void draw_game(int[GAME_ROWS][GAME_COLS]);
void get_part_snake(int[GAME_ROWS][GAME_COLS], int, int, char*);
void get_part_wall(int[GAME_ROWS][GAME_COLS], int, int, char *);
void get_command(struct snake*, int*);
void end_game(struct termios);
int is_direction(int, int);
void calculate_snake(int[GAME_ROWS][GAME_COLS], struct snake*, int*);
void play_game();
void set_food(int[GAME_ROWS][GAME_COLS], int, struct snake);
void set_stone(int[GAME_ROWS][GAME_COLS], struct snake);
void see_finish_win(int, const int*);
void see_finish_fail(int, const int*);
void see_start();
void next_level();
long int get_delta_time(struct timespec, struct timespec, int);
void press_any_key();

#endif  // SRC_SNAKE_H_
