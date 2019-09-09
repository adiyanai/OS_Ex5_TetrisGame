#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>

#define BOARD_SIZE 20
#define LEFT_KEY 'a'
#define RIGHT_KEY 'd'
#define DOWN_KEY 's'
#define FLIP_KEY 'w'
#define QUIT_KEY 'q'


typedef enum {
    Vertical = 0,
    Horizontal = 1
} shapeDirection;

typedef struct shape {
    int start_shape_x;
    int start_shape_y;
    shapeDirection direction;
} Shape;

struct board {
    int size;
    char board_data[BOARD_SIZE][BOARD_SIZE];
    Shape shape;
} typedef Board;

Board board;

/**
 * create init shape
 * @param board - the board
 */
void create_init_shape() {
    board.shape.start_shape_x = 0;
    board.shape.start_shape_y = (board.size / 2) - 1;
    board.shape.direction = Horizontal;
}

/**
 * remove the shape from the board
 * @param board - the board
 */
void remove_shape() {
    int x, y;
    x = board.shape.start_shape_x;
    y = board.shape.start_shape_y;

    // remove the shape from the board
    // if the shape in horizontal direction
    if (board.shape.direction == Horizontal) {
        board.board_data[x][y] = ' ';
        board.board_data[x][y + 1] = ' ';
        board.board_data[x][y + 2] = ' ';

        // if the shape in vertical direction
    } else if (board.shape.direction == Vertical) {
        board.board_data[x][y] = ' ';
        board.board_data[x + 1][y] = ' ';
        board.board_data[x + 2][y] = ' ';
    }
}

/**
 * create init board
 * @param board - the board
 */
void create_init_board() {
    // clear the board and initialize a new one
    system("clear");

    int i, j;

    // initialize the bottom
    for (j = 0; j < board.size; j++) {
        board.board_data[board.size - 1][j] = '*';
    }

    // initialize the sides
    for (i = 0; i < board.size; i++) {
        // right side
        board.board_data[i][0] = '*';
        // left side
        board.board_data[i][board.size - 1] = '*';
    }

    // initialize the spaces
    for (i = 0; i < (board.size - 1); i++) {
        for (j = 1; j < (board.size - 1); j++){
            // add space
            board.board_data[i][j] = ' ';
        }
    }

    // initialize the shape
    create_init_shape();
}

/**
 * draw the board
 * @param board - the board
 */
void draw_board() {
    // clear the last draw and draw new one
    system("clear");

    int x, y, i, j;
    x = board.shape.start_shape_x;
    y = board.shape.start_shape_y;

    // add the shape to the board
    // if the shape in horizontal direction
    if (board.shape.direction == Horizontal) {
        board.board_data[x][y] = '-';
        board.board_data[x][y + 1] = '-';
        board.board_data[x][y + 2] = '-';

        // if the shape in vertical direction
    } else if (board.shape.direction == Vertical) {
        board.board_data[x][y] = '-';
        board.board_data[x + 1][y] = '-';
        board.board_data[x + 2][y] = '-';
    }

    // draw the board
    for (i = 0; i < board.size; i++) {
        for (j = 0; j < board.size; j++) {
            printf("%c", board.board_data[i][j]);
        }
        printf("\n");
    }
}

/**
 * update shape location
 * @param board - the board
 * @param key - the user input key to which direction to move the shape
 */
void update_shape_location(char key) {
    int x, y;
    x = board.shape.start_shape_x;
    y = board.shape.start_shape_y;

    // if the shape in horizontal direction
    if (board.shape.direction == Horizontal) {
        if (key == LEFT_KEY) {
            // if we have place, move to this direction
            if (board.board_data[x][y - 1] == ' ') {
                remove_shape();
                board.shape.start_shape_y = y - 1;
            }
        } else if (key == RIGHT_KEY) {
            // if we have place, move to this direction
            if (board.board_data[x][y + 3] == ' ') {
                remove_shape();
                board.shape.start_shape_y = y + 1;
            }
        } else if (key == DOWN_KEY) {
            // if we have place, move to this direction
            if (board.board_data[x + 1][y] == ' ') {
                remove_shape();
                board.shape.start_shape_x = x + 1;

                // if we reach to the end of the board
            } else if (board.board_data[x + 1][y] == '*') {
                // remove the shape and create a new one in the start of the board
                remove_shape();
                create_init_shape();
            }
        } else if (key == FLIP_KEY) {
            // if we have place, flip the shape
            if ((board.board_data[x + 1][y] == ' ') && (board.board_data[x - 1][y] == ' ')) {
                remove_shape();
                // update shape location and direction
                board.shape.direction = Vertical;
                board.shape.start_shape_x = x - 1;
                board.shape.start_shape_y = y + 1;
            }
        }

        // if the shape in vertical direction
    } else if (board.shape.direction == Vertical) {
        if (key == LEFT_KEY) {
            // if we have place, move to this direction
            if (board.board_data[x][y - 1] == ' ') {
                remove_shape();
                board.shape.start_shape_y = y - 1;
            }
        } else if (key == RIGHT_KEY) {
            // if we have place, move to this direction
            if (board.board_data[x][y + 1] == ' ') {
                remove_shape();
                board.shape.start_shape_y = y + 1;
            }
        } else if (key == DOWN_KEY) {
            // if we have place, move to this direction
            if (board.board_data[x + 3][y] == ' ') {
                remove_shape();
                board.shape.start_shape_x = x + 1;

                // if we reach to the end of the board
            } else if (board.board_data[x + 3][y] == '*') {
                // remove the shape and create a new one in the start of the board
                remove_shape();
                create_init_shape();
            }
        } else if (key == FLIP_KEY) {
            // if we have place, flip the shape
            if ((board.board_data[x + 1][y + 1] == ' ') && (board.board_data[x + 1][y - 1] == ' ')) {
                remove_shape();
                // update shape location and direction
                board.shape.direction = Horizontal;
                board.shape.start_shape_x = x + 1;
                board.shape.start_shape_y = y - 1;
            }
        }
    }

    // draw the board after the shape move
    draw_board();
}

/**
 * operation to do when SIGUSR2 recevied - read user input key
 * @param sig
 */
void usr2_read_user_input(int sig) {
    signal(SIGUSR2, usr2_read_user_input);

    char key;
    // read key from stdin
    key = (char)getchar();
    // if the key is q - QUIT, the program end
    if (key == QUIT_KEY) {
        exit(0);
    }
    // update the shape location according to the user input key
    update_shape_location(key);
    // draw the board after the shape move
    draw_board();
}

/**
 * operation to do when SIGALRM recevied - move down the shape every second
 * @param sig
 */
void alarm_shape_move_down(int sig) {
    signal(SIGALRM, alarm_shape_move_down);
    // set an alarm for one second
    alarm(1);

    // update the shape location to move down
    update_shape_location(DOWN_KEY);
    // draw the board after the shape move
    draw_board();
}

int main() {

    board.size = BOARD_SIZE;
    create_init_board();
    draw_board();

    // define what to do next time receives SIGUSR2
    signal(SIGUSR2, usr2_read_user_input);

    // define what to do next time receives SIGALRM
    signal(SIGALRM, alarm_shape_move_down);
    // set an alarm for one second
    alarm(1);

    // always wait for signals until the user press 'q'
    while (1) { pause(); }
}