#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

#define cols 40
#define rows 20

int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(0, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &newt);
    oldf = fcntl(0, F_GETFL, 0);
    fcntl(0, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(0, TCSANOW, &oldt);
    fcntl(0, F_SETFL, oldf);
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(0, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(0, TCSANOW, &oldt);
    return ch;
}

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

void add_score(int s) {
    struct ScoreNode* newNode = malloc(sizeof(struct ScoreNode));
    newNode->score = s;
    newNode->next = scoreHistory;
    scoreHistory = newNode;
}

void add_head(int x, int y) {
    struct Node* newNode = malloc(sizeof(struct Node));
    newNode->x = x;
    newNode->y = y;
    newNode->next = snake.head;
    snake.head = newNode;
    if (snake.tail == NULL) snake.tail = newNode;
}

void remove_tail() {
    if (!snake.head) return;
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

void clear_snake() {
    while (snake.head) {
        struct Node* temp = snake.head;
        snake.head = snake.head->next;
        free(temp);
    }
    snake.tail = NULL;
}

void spawn_food() {
    int valid = 0;
    while (!valid) {
        foodX = rand() % (cols - 2) + 1;
        foodY = rand() % (rows - 2) + 1;
        valid = 1;
        struct Node* curr = snake.head;
        while (curr) {
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
    struct Node* curr = snake.head->next;
    while (curr) {
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

void read_key() {
    if (kbhit()) {
        char ch = getch();
        switch (ch) {
            case 'w': case '8': if (dirY != 1) { dirX = 0; dirY = -1; } break;
            case 's': case '2': if (dirY != -1) { dirX = 0; dirY = 1; } break;
            case 'a': case '4': if (dirX != 1) { dirX = -1; dirY = 0; } break;
            case 'd': case '6': if (dirX != -1) { dirX = 1; dirY = 0; } break;
            case 'q': isGameover = 1; break;
        }
    }
}

void print_board() {
    printf("\033[H"); 
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (x == 0 || x == cols - 1 || y == 0 || y == rows - 1) {
                printf("██");
                continue;
            }
            int printed = 0;
            struct Node* curr = snake.head;
            int index = 0;
            while (curr) {
                if (curr->x == x && curr->y == y) {
                    printf(index == 0 ? "@@" : "++");
                    printed = 1;
                    break;
                }
                curr = curr->next;
                index++;
            }
            if (!printed && x == foodX && y == foodY) {
                printf("◆◆");
                printed = 1;
            }
            if (!printed) printf("  ");
        }
        printf("\n");
    }
    printf("\nScore: %d\n", snake.length - 1);
}

int main() {
    int choice;
    do {
        system("clear");
        printf("===== SNAKE GAME MENU =====\n");
        printf("1. Members List\n");
        printf("2. Program Statement\n");
        printf("3. Scorecard\n");
        printf("4. Play Game\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) choice = 0;
        while (getchar() != '\n') 
        { 
            ; 
        }

        switch (choice) {
            case 1:
                printf("\nChetan Kumar (Leader)\nAnupam Anand\nDebabrata Mandal\n");
                (void)getchar();
                break;
            case 2: {
                system("clear");
                FILE *fp = fopen("program_statement_symbols.txt", "r");
                if (!fp) printf("File not found!\n");
                else {
                    int ch;
                    while ((ch = fgetc(fp)) != EOF) printf("%c", ch);
                    fclose(fp);
                }
                (void)getchar();
                break;
            }
            case 3: {
                struct ScoreNode* s = scoreHistory;
                if (!s) printf("No scores yet\n");
                while (s) {
                    printf("Score: %d\n", s->score);
                    s = s->next;
                }
                (void)getchar();
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
                system("clear");
                while (!isGameover) {
                    read_key();
                    move_snake();
                    print_board();
                    usleep(100000);
                }
                add_score(snake.length - 1);
                clear_snake();
                printf("GAME OVER\nPress Enter to continue...");
                (void)getchar();
                break;
        }
    } while (choice != 5);
    return 0;
}
