#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum
{
    Dead = 0,
    Alive = 1,
} tile_t;

typedef struct
{
    const int width;
    const int height;
    tile_t *data;
} board_t;

static tile_t get_tile(const board_t *b, int x, int y)
{
    if (x < 0 || x >= b->width || y < 0 || y >= b->height)
        return Dead;
    return b->data[y * b->width + x];
}

static void set_tile(board_t *b, int x, int y, tile_t value)
{
    if (x < 0 || x >= b->width || y < 0 || y >= b->height)
        return;
    b->data[y * b->width + x] = value;
}

static size_t count_neighbors(const board_t *b, int x, int y)
{
    size_t count = 0;

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0)
                continue;
            if (get_tile(b, x + dx, y + dy) == Alive)
                count++;
        }
    }
    return count;
}

static void update_board(board_t *b)
{
    board_t copy = {
        .width = b->width,
        .height = b->height,
        .data = malloc(((size_t)b->width * b->height) * sizeof *b->data),
    };
    memcpy(copy.data, b->data, (size_t)b->width * b->height * sizeof *b->data);

    for (int x = 0; x < b->width; ++x) {
        for (int y = 0; y < b->height; ++y) {
            size_t neighbors = count_neighbors(&copy, x, y);
            if (get_tile(&copy, x, y) == Alive) {
                if (neighbors < 2 || neighbors > 3)
                    set_tile(b, x, y, Dead);
            } else {
                if (neighbors == 3)
                    set_tile(b, x, y, Alive);
            }

        }
    }
    free(copy.data);
}

int main(void)
{
    srand(time(NULL));

    board_t b = {
        .width = 800,
        .height = 450,
        .data = NULL,
    };

    b.data = malloc(((size_t)b.width * b.height) * sizeof *b.data);
    if (b.data == NULL)
        return perror("malloc"), 1;

    // set game at a random state
    for (int x = 0; x < b.width; ++x)
        for (int y = 0; y < b.height; ++y)
            set_tile(&b, x, y, (rand() % 10 == 0) ? Alive : Dead);

    InitWindow(b.width, b.height, "Game of life");

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (GetKeyPressed()) {
            case KEY_SPACE:
                update_board(&b);
                break;
            default:
                break;
        }

        for (int x = 0; x < b.width; ++x)
            for (int y = 0; y < b.height; ++y)
                DrawPixel(x, y, (get_tile(&b, x, y) == Alive) ? WHITE : BLACK);

        EndDrawing();
    }
    CloseWindow();
}
