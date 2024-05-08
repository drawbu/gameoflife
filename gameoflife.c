#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WIDTH ((2 << 9) - 1)
#define HEIGHT ((2 << 9) - 1)

typedef enum
{
    Dead = false,
    Alive = true,
} tile_t;

typedef struct
{
    bool *data;
    bool *last;
    struct
    {
        int x;
        int y;
    } camera;
} board_t;

#define IDX(x, y) (((y) & HEIGHT) * WIDTH + ((x) & WIDTH))

#define foreach_cell(x, y) \
    for (int (y) = 0; (y) < HEIGHT; ++(y)) \
        for (int (x) = 0; (x) < WIDTH; ++(x))

static size_t count_neighbors(const bool *board, int x, int y)
{
    return (
        board[IDX(x - 1, y - 1)]
        + board[IDX(x, y - 1)]
        + board[IDX(x + 1, y - 1)]
        + board[IDX(x - 1, y)]
        + board[IDX(x + 1, y)]
        + board[IDX(x - 1, y + 1)]
        + board[IDX(x, y + 1)]
        + board[IDX(x + 1, y + 1)]
    );
}

static void update_board(board_t *b)
{
    bool *tmp = b->data;
    b->data = b->last;
    b->last = tmp;

    foreach_cell(x, y) {
        size_t neighbors = count_neighbors(b->last, x, y);
        if (neighbors == 3)
            b->data[IDX(x, y)] = Alive;
        else if (neighbors != 2)
            b->data[IDX(x, y)] = Dead;
        else
            b->data[IDX(x, y)] = b->last[IDX(x, y)];
    }
}

static void draw_board(const board_t *b, uint8_t zoom)
{
    BeginDrawing();
    ClearBackground(BLACK);

    foreach_cell(x, y)
        if (b->data[IDX(x, y)] == Alive)
            DrawRectangle(
                (x - b->camera.x) * zoom,
                (y - b->camera.y) * zoom,
                zoom, zoom, WHITE);
    EndDrawing();
}

int main(void)
{
    srand(time(NULL));
    uint8_t zoom_level = 1;

    board_t b = {0};

    // double malloc to have buffers side by side
    b.data = malloc(2UL * WIDTH * HEIGHT * sizeof *b.data);
    if (b.data == NULL)
        return perror("malloc"), 1;
    b.last = &b.data[1UL * WIDTH * HEIGHT];

    // set game at a random state
    foreach_cell(x, y)
        b.data[IDX(x, y)] = (rand() % 10 == 0) ? Alive : Dead;

    InitWindow(WIDTH, HEIGHT, "Game of life");

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_SPACE))
            update_board(&b);

        // zoooom
        float wheel = GetMouseWheelMove();
        if (wheel < 0) {
            if (zoom_level > 1)
                zoom_level -= 1;
        } else if (wheel > 0)
            if (zoom_level < 20)
                zoom_level += 1;

        if (IsKeyDown(KEY_LEFT))
            b.camera.x -= 1;
        if (IsKeyDown(KEY_RIGHT))
            b.camera.x += 1;
        if (IsKeyDown(KEY_UP))
            b.camera.y -= 1;
        if (IsKeyDown(KEY_DOWN))
            b.camera.y += 1;

        draw_board(&b, zoom_level);
    }
    CloseWindow();
}
