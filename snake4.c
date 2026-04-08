#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

#define cols 40
#define rows 20

struct Node {
    int x, y;
    struct Node* next;
};

struct Snake {
    struct Node* head;
    struct Node* tail;
    int length;
};

struct ScoreNode {
    int score;
    struct ScoreNode* next;
};

struct Snake snake;
struct ScoreNode* scoreHistory = NULL;
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

void add_score(int s) {
    struct ScoreNode* newNode = (struct ScoreNode*)malloc(sizeof(struct ScoreNode));
    newNode->score = s;
    newNode->next = scoreHistory;
    scoreHistory = newNode;
}

void add_head(int x, int y) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->x = x;
    newNode->y = y;
    newNode->next = snake.head;
    snake.head = newNode;
    if (snake.tail == NULL) snake.tail = newNode;
}

void remove_tail() {
    if (snake.head == NULL) return;
    if (snake.head == snake.tail) {
        free(snake.head);
        snake.head = snake.tail = NULL;
        return;
    }
    struct Node* temp = snake.head;
    while (temp->next != snake.tail) temp = temp->next;
    free(snake.tail);
    snake.tail = temp;
    snake.tail->next = NULL;
}

void spawn_food() {
    int valid = 0;
    while (!valid) {
        foodX = rand() % (cols - 2) + 1;
        foodY = rand() % (rows - 2) + 1;
        valid = 1;
        struct Node* curr = snake.head;
        while (curr != NULL) {
            if (curr->x == foodX && curr->y == foodY) {
                valid = 0;
                break;
            }
            curr = curr->next;
        }
    }
}

void move_snake() {
    if (dirX == 0 && dirY == 0) return;

    int nextX = snake.head->x + dirX;
    int nextY = snake.head->y + dirY;

    if (nextX <= 0 || nextX >= cols - 1 || nextY <= 0 || nextY >= rows - 1) {
        isGameover = 1;
        return;
    }

    struct Node* curr = snake.head;
    while (curr != NULL) {
        if (curr->x == nextX && curr->y == nextY) {
            isGameover = 1;
            return;
        }
        curr = curr->next;
    }

    add_head(nextX, nextY);

    if (nextX == foodX && nextY == foodY) {
        snake.length++;
        spawn_food();
    } else {
        remove_tail();
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
                struct Node* curr = snake.head;
                int count = 0;
                while (curr != NULL) {
                    if (curr->x == x && curr->y == y) {
                        printf(count == 0 ? "@@" : "++");
                        isSnake = 1;
                        break;
                    }
                    curr = curr->next;
                    count++;
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
void clear_snake() {
    while (snake.head != NULL) {
        struct Node* temp = snake.head;
        snake.head = snake.head->next;
        free(temp);
    }
    snake.tail = NULL;
}
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    int choice;

    do {
        printf("\n===== SNAKE GAME MENU =====\n");
        printf("1. Members List\n");
        printf("2. Program Statement\n");
        printf("3. Scorecard\n");
        printf("4. Play Game\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clear_input_buffer();

        switch(choice) {

            case 1:
                printf("\n--- MEMBERS LIST ---\n");
                printf("1. Anurag (The one doing all the work)\n");
                printf("2. Your imaginary teammate\n");
                break;

            case 2:
                printf("\n--- PROGRAM STATEMENT ---\n");
                printf("This program implements a Snake Game using linked lists.\n");
                printf("The snake moves in a 2D grid, eats food, grows in size,\n");
                printf("and the game ends on collision with wall or itself.\n");
                break;

            case 3: {
                printf("\n--- SCORECARD ---\n");
                struct ScoreNode* sCurr = scoreHistory;
                if (sCurr == NULL) {
                    printf("No scores yet. Go play the game first.\n");
                } else {
                    while (sCurr != NULL) {
                        printf("- %d\n", sCurr->score);
                        sCurr = sCurr->next;
                    }
                }
                break;
            }

            case 4:
    isGameover = 0;
    dirX = dirY = 0;

    srand(time(0));
    enable_input_mode();

    snake.head = snake.tail = NULL;
    snake.length = 1;
    add_head(cols / 2, rows / 2);
    spawn_food();

    while (!isGameover) {
        read_key();
        move_snake();
        print_board();
        usleep(100000);
    }

    add_score(snake.length - 1);
    clear_snake();

    printf("\033[2J\033[H");
    printf("GAME OVER\nFinal Score: %d\n", scoreHistory->score);

    disable_input_mode();   // only ONCE, not twice

    printf("\nPress Enter to return to menu...");
    getchar();   // now this will actually wait

    choice = 0;  // reset so menu doesn't loop

    break;

            case 5:
                printf("\nExiting... finally giving the CPU some rest.\n");
                break;

            default:
                printf("\nInvalid choice. Try using your eyes next time.\n");
        }

    } while (choice != 5);

    return 0;
}

