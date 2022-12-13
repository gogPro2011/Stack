#include <glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#define STACK_OVERFLOW  -100
#define STACK_UNDERFLOW -101
#define OUT_OF_MEMORY   -102
#define MAZE_SIZE 2

typedef struct CellPos {
    int x;
    int y;
}CellPos;
CellPos playerPos = { 1, 1 };
typedef struct Node {
    CellPos pos;
    struct Node* next;
} Node;


typedef struct MapCell {
    CellPos pos;
    int status;//0 не посещено, 1 посещено, 2 стена
    int is_de_way; //по умолчанию 0, 1 если да
} MapCell;

MapCell** map;
int show_de_way;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if ((key == GLFW_KEY_A || key == GLFW_KEY_LEFT) && action == GLFW_PRESS) {
        if (map[playerPos.x - 1][playerPos.y].status != 2) playerPos.x--;
    }
    else if ((key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) && action == GLFW_PRESS) {
        if (map[playerPos.x + 1][playerPos.y].status != 2) playerPos.x++;
    }
    else if ((key == GLFW_KEY_W || key == GLFW_KEY_UP) && action == GLFW_PRESS) {
        if (map[playerPos.x][playerPos.y + 1].status != 2) playerPos.y++;
    }
    else if ((key == GLFW_KEY_S || key == GLFW_KEY_DOWN) && action == GLFW_PRESS) {
        if (map[playerPos.x][playerPos.y - 1].status != 2) playerPos.y--;
    }
    else if ((key == GLFW_KEY_E) && action == GLFW_PRESS) {
        show_de_way = (!show_de_way);
    }
}

void ShowCell(int x, int y) {
    if (map[x][y].status == 1) {//фиолетовые проход
        glBegin(GL_QUADS);
        glColor3f(1, 0, 1);
        glVertex2f(0.0, 1.0);
        glVertex2f(1.0, 1.0);
        glVertex2f(1.0, 0.0);
        glVertex2f(0.0, 0.0);
        glEnd();
    }
    if (map[x][y].is_de_way == 1 && show_de_way == 1) {
        glBegin(GL_QUADS);
        glColor3f(1, 0.8, 0);
        glVertex2f(0.0, 1.0);
        glVertex2f(1.0, 1.0);
        glVertex2f(1.0, 0.0);
        glVertex2f(0.0, 0.0);
        glEnd();
    }
    if (x == (MAZE_SIZE * 2 - 1) && y == (MAZE_SIZE * 2 - 1)) {
        glBegin(GL_QUADS);
        glColor3f(0, 1, 0);
        glVertex2f(0.0, 1.0);
        glVertex2f(1.0, 1.0);
        glVertex2f(1.0, 0.0);
        glVertex2f(0.0, 0.0);
        glEnd();
    }
    if (x == playerPos.x && y == playerPos.y) {
        glBegin(GL_QUADS);
        glColor3f(1, 0, 0);
        glVertex2f(0.0, 1.0);
        glVertex2f(1.0, 1.0);
        glVertex2f(1.0, 0.0);
        glVertex2f(0.0, 0.0);
        glEnd();
    }
}

Node* stack_init(CellPos i) {
    Node* tmp = (Node*)malloc(sizeof(Node));
    tmp->next = NULL;
    tmp->pos.x = i.x;
    tmp->pos.y = i.y;
    map[i.x][i.y].status = 1;
    return tmp;
}

void push(Node** head, CellPos i) {
    Node* tmp = (Node*)malloc(sizeof(Node));
    if (tmp == NULL) {
        exit(STACK_OVERFLOW);
    }
    map[i.x][i.y].status = 1;
    map[(i.x + (*head)->pos.x) / 2][(i.y + (*head)->pos.y) / 2].status = 1;
    tmp->next = (*head);
    tmp->pos.x = i.x;
    tmp->pos.y = i.y;
    *head = tmp;
}

CellPos pop(Node** head) {
    if ((*head) == NULL) {
        exit(STACK_UNDERFLOW);
    }
    Node* out = (*head);
    CellPos value = (*head)->pos;
    (*head) = (*head)->next;
    free(out);
    return value;
}

void delete_pop(Node ** head) {
    if ((*head) == NULL) {
        exit(STACK_UNDERFLOW);
    }
    Node* out = (*head);
    *head = (*head)->next;;
    free(out);
}

int possible_around_count(Node* head_stack) {
    int count = 0;
    if (head_stack->pos.x + 2 < (MAZE_SIZE * 2 + 1) && map[head_stack->pos.x + 2][head_stack->pos.y].status < 1) {
        count++;
    }
    if (head_stack->pos.y + 2 < (MAZE_SIZE * 2 + 1) && map[head_stack->pos.x][head_stack->pos.y + 2].status < 1) {
        count++;
    }
    if (head_stack->pos.x - 2 > 0 && map[head_stack->pos.x - 2][head_stack->pos.y].status < 1) {
        count++;
    }
    if (head_stack->pos.y - 2 > 0 && map[head_stack->pos.x][head_stack->pos.y - 2].status < 1) {
        count++;
    }
    return count;
}

CellPos* possible_around(Node* head_stack) {
    int count = 0;
    CellPos* value = (CellPos*)malloc(sizeof(CellPos) * 3);
    if (head_stack->pos.x + 2 < (MAZE_SIZE * 2 + 1) && map[head_stack->pos.x + 2][head_stack->pos.y].status < 1) {
        value[count] = map[head_stack->pos.x + 2][head_stack->pos.y].pos;
        count++;
    }
    if (head_stack->pos.y + 2 < (MAZE_SIZE * 2 + 1) && map[head_stack->pos.x][head_stack->pos.y + 2].status < 1) {
        value[count] = map[head_stack->pos.x][head_stack->pos.y + 2].pos;
        count++;
    }
    if (head_stack->pos.x - 2 > 0 && map[head_stack->pos.x - 2][head_stack->pos.y].status < 1) {
        value[count] = map[head_stack->pos.x - 2][head_stack->pos.y].pos;
        count++;
    }
    if (head_stack->pos.y - 2 > 0 && map[head_stack->pos.x][head_stack->pos.y - 2].status < 1) {
        value[count] = map[head_stack->pos.x][head_stack->pos.y - 2].pos;
        count++;
    }

    return value;
}
void make_de_way(Node* stack) {
    while (stack != NULL) {
        map[stack->pos.x][stack->pos.y].is_de_way = 1;
        stack = stack->next;
    }
}

void Game_start() {
    show_de_way = 0;
    playerPos.x = 1;
    playerPos.y = 1;
    int visited = 1;
    int mnozh = MAZE_SIZE * 2 + 1;
    map = (MapCell**)malloc(sizeof(MapCell*) * mnozh);
    for (int i = 0; i < mnozh; i++) {
        map[i] = (MapCell*)malloc(sizeof(MapCell) * mnozh);
    }
    for (int i = 0; i < mnozh; i++) {
        for (int j = 0; j < mnozh; j++) {
            if (i % 2 == 1 && j % 2 == 1) {
                map[i][j].status = 0;
            }
            else {
                map[i][j].status = 2;
            }
            map[i][j].pos.x = i;
            map[i][j].pos.y = j;
            map[i][j].is_de_way = 0;
        }
    }

    srand(time(NULL));
    CellPos start = { 1 , 1 };
    CellPos end = { mnozh - 2, mnozh - 2 };
    Node* stack = stack_init(start);
    do {
        while (visited < (MAZE_SIZE * MAZE_SIZE) && possible_around_count(stack) > 0) {
            CellPos* i = possible_around(stack);
            int count = possible_around_count(stack);
            count = rand() % count;
            push(&stack, i[count]);

            visited++;

            free(i);
            if (stack->pos.x == end.x && stack->pos.y == end.y) { 
                make_de_way(stack);
                delete_pop(&stack); 
            }
        }
        while (stack != NULL && possible_around_count(stack) == 0 ) {
            delete_pop(&stack);
        }

    } while (visited < (MAZE_SIZE * MAZE_SIZE));


}
void Game() {
    glLoadIdentity();
    glScalef(2.0 / (MAZE_SIZE * 2 + 1), 2.0 / (MAZE_SIZE * 2 + 1), 1.0);
    glTranslatef(-(MAZE_SIZE * 2 + 1) * 0.5, -(MAZE_SIZE * 2 + 1) * 0.5, 0.0);

    for (int i = 0; i < (MAZE_SIZE * 2 + 1); i++) {
        for (int j = 0; j < (MAZE_SIZE * 2 + 1); j++) {
            glPushMatrix();

            glTranslatef((float)i, (float)j, 0);

            ShowCell(i, j);

            glPopMatrix();
        }
    }
    if (playerPos.x == (MAZE_SIZE * 2 - 1) && playerPos.y == (MAZE_SIZE * 2 - 1)) Game_start();
};

int main() {
    Game_start();
    
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    if (!glfwInit())
        return -1;
    
    GLFWwindow* window = glfwCreateWindow(1000, 1000, "gege", NULL, NULL);

    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        Game();

        glfwSetKeyCallback(window, key_callback);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }
    glfwTerminate();

    for (int i = 0; i < (MAZE_SIZE * 2 + 1); i++) {
        free(map[i]);
    }
    free(map);
}