#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

#define cols 40
#define rows 20
#define len 256

struct snakepart { int x, y; };
struct Snake { int length; struct snakepart part[len]; };
struct Snake snake;
struct termios orig_ttystate;
int isGameover = 0, foodX, foodY, dirX = 0, dirY = 0;

void disable_input_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_ttystate);
}

void enable_input_mode() {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &orig_ttystate);
    atexit(disable_input_mode);
    ttystate = orig_ttystate;
    ttystate.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

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

void move_snake() {
    if (dirX == 0 && dirY == 0) return;
    for (int i = snake.length - 1; i > 0; i--) {
        snake.part[i] = snake.part[i - 1];
    }
    snake.part[0].x += dirX;
    snake.part[0].y += dirY;
    if (snake.part[0].x <= 0 || snake.part[0].x >= cols - 1 ||
        snake.part[0].y <= 0 || snake.part[0].y >= rows - 1) {
        isGameover = 1;
    }
    for (int i = 1; i < snake.length; i++) {
        if (snake.part[0].x == snake.part[i].x && snake.part[0].y == snake.part[i].y) {
            isGameover = 1;
        }
    }
    if (snake.part[0].x == foodX && snake.part[0].y == foodY) {
        snake.length++;
        spawn_food();
    }
}

void print_board() {
    printf("\033[H");
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (x == 0 || y == 0 || x == cols - 1 || y == rows - 1) {
                printf("██");
            } else if (x == foodX && y == foodY) {
                printf("FF");
            } else {
                int isSnake = 0;
                for (int i = 0; i < snake.length; i++) {
                    if (snake.part[i].x == x && snake.part[i].y == y) {
                        printf(i == 0 ? "@@" : "++");
                        isSnake = 1;
                        break;
                    }
                }
                if (!isSnake) printf("  ");
            }
        }
        printf("\n");
    }
}

void read_key() {
    char ch;
    while (read(STDIN_FILENO, &ch, 1) == 1) {
        switch (ch) {
            case 'w': case '8': if (dirY != 1) { dirX = 0; dirY = -1; } break;
            case 's': case '2': if (dirY != -1) { dirX = 0; dirY = 1; } break;
            case 'a': case '4': if (dirX != 1) { dirX = -1; dirY = 0; } break;
            case 'd': case '6': if (dirX != -1) { dirX = 1; dirY = 0; } break;
            case 'q': isGameover = 1; break;
        }
    }
}

int main() {
    srand(time(0));
    enable_input_mode();
    snake.length = 1;
    snake.part[0].x = cols / 2;
    snake.part[0].y = rows / 2;
    spawn_food();
    while (!isGameover) {
        read_key();
        move_snake();
        print_board();
        usleep(100000);
    }
    printf("Game Over! Score: %d\n", snake.length - 1);
    return 0;
}
