#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // for usleep
#include <termios.h> // for terminal input
#include <fcntl.h>   // for non-blocking input
#include <time.h>

#define cols 40
#define rows 20
#define len 256

char board[cols * rows];
int isGameover = 0;

struct snakepart {
    int x, y;
};

struct Snake {
    int length;
    struct snakepart part[len];
};

struct Snake snake;

int foodX, foodY;
int dirX = 0, dirY = 0;

// ------------------ NON-BLOCKING INPUT ------------------
void enable_nonblocking_input() {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    ttystate.c_lflag &= ~ICANON; // disable buffered i/o
    ttystate.c_lflag &= ~ECHO;   // disable echo
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); // non-blocking input
}

int kbhit() {
    char ch;
    int nread;
    nread = read(STDIN_FILENO, &ch, 1);
    if (nread == 1) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

int getch() {
    return getchar();
}

// ------------------ FOOD ------------------
void spawn_food() {
    int valid = 0;
    while (!valid) {
        foodX = rand() % (cols - 2) + 1;
        foodY = rand() % (rows - 2) + 1;
        valid = 1;
        for (int i = 0; i < snake.length; i++) {
            if (snake.part[i].x == foodX && snake.part[i].y == foodY) {
                valid = 0;
                break;
            }
        }
    }
}

// ------------------ BOARD ------------------
void fill_board() {
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (x == 0 || y == 0 || x == cols - 1 || y == rows - 1)
                board[y * cols + x] = '#';
            else
                board[y * cols + x] = ' ';
        }
    }
}

// ------------------ DRAW ------------------
void draw_snake() {
    for (int i = snake.length - 1; i > 0; i--) {
        board[snake.part[i].y * cols + snake.part[i].x] = '+';
    }
    board[snake.part[0].y * cols + snake.part[0].x] = '@';
}

void draw_food() {
    board[foodY * cols + foodX] = 'F';
}

// ------------------ PRINT ------------------
void clear_screen() {
    printf("\033[H\033[J"); // ANSI escape to clear terminal
}

void print_board() {
    clear_screen();
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            putchar(board[y * cols + x]);
        }
        putchar('\n');
    }
}

// ------------------ INPUT ------------------
void read_key() {
    if (kbhit()) {
        int ch = getch();
        int dx = 0, dy = 0;

        switch (ch) {
            case '8': dx = 0; dy = -1; break;
            case '2': dx = 0; dy = 1; break;
            case '4': dx = -1; dy = 0; break;
            case '6': dx = 1; dy = 0; break;
            default: return;
        }

        dirX = dx;
        dirY = dy;
    }
}

// ------------------ LOGIC ------------------
void move_snake() {
    // move body
    for (int i = snake.length - 1; i > 0; i--) {
        snake.part[i] = snake.part[i - 1];
    }

    // move head
    snake.part[0].x += dirX;
    snake.part[0].y += dirY;

    // collision with wall
    if (snake.part[0].x <= 0 || snake.part[0].x >= cols - 1 ||
        snake.part[0].y <= 0 || snake.part[0].y >= rows - 1) {
        isGameover = 1;
    }

    // collision with self
    for (int i = 1; i < snake.length; i++) {
        if (snake.part[0].x == snake.part[i].x &&
            snake.part[0].y == snake.part[i].y) {
            isGameover = 1;
        }
    }

    // eating food
    if (snake.part[0].x == foodX && snake.part[0].y == foodY) {
        snake.length++;
        spawn_food();
    }
}

// ------------------ MAIN ------------------
int main() {
    srand(time(0));
    enable_nonblocking_input();

    snake.length = 1;
    snake.part[0].x = 5;
    snake.part[0].y = 5;

    spawn_food();

    while (!isGameover) {
        fill_board();
        draw_food();
        draw_snake();
        print_board();
        read_key();
        move_snake();
        usleep(100000); // 100ms delay
    }

    printf("Game Over\n");
    return 0;
}
