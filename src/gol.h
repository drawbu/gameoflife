#pragma once

#include "utils.h"

typedef enum {
    Dead = 0,
    Alive = 1,
} tile_t;

typedef struct {
    int x;
    int y;
} point_t;

typedef DEF_ARR(point_t) point_array_t;

typedef struct {
    point_t camera;
    point_array_t tiles;
} board_t;
