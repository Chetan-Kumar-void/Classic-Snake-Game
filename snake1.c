#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
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

// ------------------ FOOD SPAWNING ------------------
void spawn_food() {
    int valid = 0;
    while (!valid) {
        // Generate random coordinates within the walls
        foodX = rand() % (cols - 2) + 1;
        foodY = rand() % (rows - 2) + 1;
        
        valid = 1; // Assume it's valid until proven otherwise

        // Check if the new food position overlaps with any part of the snake
        for (int i = 0; i < snake.length; i++) {
            if (snake.part[i].x == foodX && snake.part[i].y == foodY) {
                valid = 0; // Overlap found, try again
                break;
            }
        }
    }
}

// ------------------ BOARD ------------------
void fill_board() {
    int x, y;
    for (y = 0; y < rows; y++) {
        for (x = 0; x < cols; x++) {
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
    system("cls");
}

void print_board() {
    clear_screen();
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            putch(board[y * cols + x]);
        }
        putch('\n');
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

        // move body
        for (int i = snake.length - 1; i > 0; i--) {
            snake.part[i] = snake.part[i - 1];
        }

        // move head
        snake.part[0].x += dx;
        snake.part[0].y += dy;

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
            spawn_food(); // Use the smarter spawn logic
        }
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

    
    if (snake.part[0].x == foodX && snake.part[0].y == foodY) {
        snake.length++;
        spawn_food(); // Use the smarter spawn logic
    }
}

// ------------------ MAIN ------------------
int main() {
    srand(time(0));

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
        Sleep(100);
    }

    printf("Game Over\n");
    return 0;
}
