#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "gol.h"

static size_t find_tile(const point_array_t *tiles, int x, int y)
{
    for (size_t i = 0; i < tiles->size; i++)
        if (tiles->arr[i].x == x && tiles->arr[i].y == y)
            return i;
    return UINT64_MAX;
}

static tile_t get_tile(const point_array_t *tiles, int x, int y)
{
    return (find_tile(tiles, x, y) == UINT64_MAX) ? Dead : Alive;
}

static void set_tile(point_array_t *tiles, int x, int y, tile_t value)
{
    size_t idx = find_tile(tiles, x, y);

    switch (value) {
        case Alive:
            if (idx == UINT64_MAX)
                append_to_array(tiles, &(point_t){x, y}, sizeof *tiles->arr);
            break;
        case Dead:
            if (idx != UINT64_MAX)
                remove_from_array(tiles, idx, sizeof *tiles->arr);
            break;
    }
}

static size_t count_neighbors(const point_array_t *b, int x, int y)
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

static void update_tile(
    const point_array_t *cpy, point_array_t *tiles, int x, int y)
{
    size_t neighbors = count_neighbors(cpy, x, y);
    if (get_tile(cpy, x, y) == Alive) {
        if (neighbors < 2 || neighbors > 3)
            set_tile(tiles, x, y, Dead);
    } else {
        if (neighbors == 3)
            set_tile(tiles, x, y, Alive);
    }
}

static void update_board(board_t *b)
{
    point_array_t copy = b->tiles;
    copy.arr = malloc(copy.size * sizeof *copy.arr),
    memcpy(copy.arr, b->tiles.arr, copy.size * sizeof *copy.arr);

    for (size_t i = 0; i < b->tiles.size; i++) {
        point_t *p = &copy.arr[i];
        update_tile(&copy, &b->tiles, p->x, p->y);
    }
    free(copy.arr);
}

static void draw_board(const board_t *b, uint8_t zoom)
{
    BeginDrawing();
    ClearBackground(BLACK);

    for (size_t i = 0; i < b->tiles.size; i++) {
        point_t *t = &b->tiles.arr[i];
        DrawRectangle(
            (t->x - b->camera.x) * zoom, (t->y - b->camera.y) * zoom, zoom,
            zoom, WHITE);
    }

    EndDrawing();
}

int main(void)
{
    srand(time(NULL));

    uint8_t zoom_level = 1;
    const int width = 800;
    const int height = 450;

    board_t b = {0};

    // set game at a random state
    for (int x = 0; x < width; ++x)
        for (int y = 0; y < height; ++y)
            if (rand() % 10 == 0)
                set_tile(&b.tiles, x, y, Alive);

    InitWindow(width, height, "Game of life");

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
