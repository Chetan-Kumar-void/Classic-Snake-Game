#include<stdio.h> 
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
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

int isGameover = 0, foodX, foodY, dirX = 0, dirY = 0;

// ---------------- SCORE ----------------
void add_score(int s) {
    struct ScoreNode* newNode = (struct ScoreNode*)malloc(sizeof(struct ScoreNode));
    newNode->score = s;
    newNode->next = scoreHistory;
    scoreHistory = newNode;
}

// ---------------- SNAKE ----------------
void add_head(int x, int y) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->x = x;
    newNode->y = y;
    newNode->next = snake.head;
    snake.head = newNode;

    if (snake.tail == NULL)
        snake.tail = newNode;
}

void remove_tail() {
    if (snake.head == NULL) return;

    if (snake.head == snake.tail) {
        free(snake.head);
        snake.head = snake.tail = NULL;
        return;
    }

    struct Node* temp = snake.head;
    while (temp->next != snake.tail)
        temp = temp->next;

    free(snake.tail);
    snake.tail = temp;
    snake.tail->next = NULL;
}

void clear_snake() {
    while (snake.head != NULL) {
        struct Node* temp = snake.head;
        snake.head = snake.head->next;
        free(temp);
    }
    snake.tail = NULL;
}

// ---------------- FOOD ----------------
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

// ---------------- MOVEMENT ----------------
void move_snake() {
    if (dirX == 0 && dirY == 0) return;

    int nextX = snake.head->x + dirX;
    int nextY = snake.head->y + dirY;

    // Wall collision
    if (nextX <= 0 || nextX >= cols - 1 || nextY <= 0 || nextY >= rows - 1) {
        isGameover = 1;
        return;
    }

    // Self collision
    struct Node* curr = snake.head->next;
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

// ---------------- INPUT ----------------
void read_key() {
    if (_kbhit()) {
        char ch = _getch();

        switch (ch) {
            case 'w': if (dirY != 1) { dirX = 0; dirY = -1; } break;
            case 's': if (dirY != -1) { dirX = 0; dirY = 1; } break;
            case 'a': if (dirX != 1) { dirX = -1; dirY = 0; } break;
            case 'd': if (dirX != -1) { dirX = 1; dirY = 0; } break;
            case 'q': isGameover = 1; break;
        }
    }
}

// ---------------- DISPLAY ----------------
void print_board() {
    system("cls");

    // Top wall
    for (int x = 0; x < cols; x++) {
        printf("--");
    }
    printf("\n");

    for (int y = 0; y < rows; y++) {

        for (int x = 0; x < cols; x++) {

            // Walls
            if (x == 0 || x == cols - 1) {
                printf("| ");
                continue;
            }
            if (y == 0 || y == rows - 1) {
                printf("--");
                continue;
            }

            int printed = 0;

            // Snake rendering
            struct Node* curr = snake.head;
            int index = 0;

            while (curr != NULL) {
                if (curr->x == x && curr->y == y) {
                    if (index == 0)
                        printf("@@");   // head
                    else
                        printf("* ");   // body

                    printed = 1;
                    break;
                }
                curr = curr->next;
                index++;
            }

            // Food
            if (!printed && x == foodX && y == foodY) {
                printf("F ");
                printed = 1;
            }

            // Empty space
            if (!printed) {
                printf("  ");
            }
        }

        printf("\n");
    }

    // Bottom wall (already handled in loop, but symmetry feels nice)
    for (int x = 0; x < cols; x++) {
        printf("--");
    }
    printf("\n");

    // Score display (so player isn't guessing their life choices)
    printf("Score: %d\n", snake.length - 1);
}
// ---------------- MAIN ----------------
int main() {
    int choice;

    do {
        system("cls");

        printf("===== SNAKE GAME MENU =====\n");
        printf("1. Members List\n");
        printf("2. Program Statement\n");
        printf("3. Scorecard\n");
        printf("4. Play Game\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        while (getchar() != '\n'); // clear buffer

        switch (choice) {

            case 1:
                printf("\n--- MEMBERS LIST ---\n");
                printf("1. Chetan Kumar (Leader)\n");
                printf("2. Anupam Anand\n");
                printf("3. Debabrata Mandal\n");

                printf("\nPress Enter...");
                getchar();
                break;

            case 2: {
                    system("cls");

             FILE *fp = fopen("program_statement_symbols.txt", "r");

                 if (fp == NULL) {
                        printf("Error: Could not open file!\n");
    } else {
                 char ch;

        while ((ch = fgetc(fp)) != EOF) {
            printf("%c", ch);
        }

        fclose(fp);
    }

    printf("\n\nPress Enter...");
    getchar();
    break;
}

            case 3: {
                printf("\n--- SCORECARD ---\n");
                struct ScoreNode* s = scoreHistory;

                if (!s)
                    printf("No scores yet.\n");
                else {
                    while (s) {
                        printf("- %d\n", s->score);
                        s = s->next;
                    }
                }

                printf("\nPress Enter...");
                getchar();
                break;
            }

            case 4:
                isGameover = 0;
                dirX = dirY = 0;

                srand(time(0));

                snake.head = snake.tail = NULL;
                snake.length = 1;

                add_head(cols / 2, rows / 2);
                spawn_food();

                while (!isGameover) {
                    read_key();
                    move_snake();
                    print_board();
                    Sleep(100);
                }

                add_score(snake.length - 1);
                clear_snake();

                system("cls");
                printf("GAME OVER\nFinal Score: %d\n", scoreHistory->score);

                printf("\nPress Enter...");
                getchar();

                break;

            case 5:
                printf("Thanks for Playing...\n");
                break;

            default:
                printf("Invalid choice\n");
                Sleep(1000);
        }

    } while (choice != 5);

    return 0;
}