/**
 * @inproceedings{haraborG14,
 *	author = {Daniel Damir Harabor and Alban Grastien},
 *	title = {Improving Jump Point Search},
 *	booktitle = {ICAPS},
 *	year = {2014}}
 */

#include <ace/types.h>
#include <ace/macros.h>

#include "binary_heap.h"

// Configuration
#define MAX_X 63
#define MAX_Y 63
#define IS_WALKABLE_TILE(tile) (tile < 16)

static tBinaryHeap s_frontier;

// stores the came_from information. came from is always one of the 8 directions (1 byte) + distance (1 byte for a 64x64 map)
typedef struct {
    UBYTE direction;
    UBYTE distance;
} Parent;
static Parent s_came_from[64 * 64];

// the size of this needs to fit Manhattan(a,b) + Chebyshev(a,b).
// For 64x64 maps, that is a 7 bit value + a 6 bit value => it always fits in a byte
typedef UBYTE Priority;
static Priority s_cost_so_far[64 * 64];

// internal defines
#define Chebyshev(positionA, positionB) MAX(ABS((positionA).x - (positionB).x), ABS((positionA).y - (positionB).y))
#define Manhattan(positionA, positionB) (ABS((positionA).x - (positionB).x) + ABS((positionA).y - (positionB).y))

#define SET_NORTH(x) (x |= 0b1)
#define SET_SOUTH(x) (x |= 0b10)
#define SET_EAST(x)  (x |= 0b100)
#define SET_WEST(x)  (x |= 0b1000)
#define SET_NE(x)    (x |= 0b10000)
#define SET_NW(x)    (x |= 0b100000)
#define SET_SE(x)    (x |= 0b1000000)
#define SET_SW(x)    (x |= 0b10000000)
#define HAS_NORTH(x) ((x & 0b1) != 0)
#define HAS_SOUTH(x) ((x & 0b10) != 0)
#define HAS_EAST(x)  ((x & 0b100) != 0)
#define HAS_WEST(x)  ((x & 0b1000) != 0)
#define HAS_NE(x)    ((x & 0b10000) != 0)
#define HAS_NW(x)    ((x & 0b100000) != 0)
#define HAS_SE(x)    ((x & 0b1000000) != 0)
#define HAS_SW(x)    ((x & 0b10000000) != 0)

void aStarSearch(UBYTE **pTileData, tUbCoordYX start, tUbCoordYX goal) {
    s_frontier.ubSize = 0;
    insert(s_frontier, start.uwYX);
    while (s_frontier.ubSize > 0) {
         pop_max(heap);
    }
}

void insertIntoHeap(tUbCoordYX x, Priority p) {    
    while (s_cost_so_far[s_fillLevel] < p) {
        s_cost_so_far[s_fillLevel + 1] = s_cost_so_far[s_fillLevel];
        s_fillLevel--;
    }
    ++s_fillLevel;
    s_cost_so_far[s_fillLevel] = p;
    s_frontier[s_fillLevel] = x;
}

/**
 * Return bitset of directions that have neighbours to explore
 */
UBYTE get_neighbours(UBYTE** map, tUbCoordYX loc) {
    UBYTE ubX = loc.ubX;
    UBYTE ubY = loc.ubY;
    UBYTE *column = map[loc.ubX];
    UBYTE result = 0;
    if (ubY > 0 && IS_WALKABLE_TILE(column[ubY - 1])) {
        SET_NORTH(result);
    }
    if (ubY < MAX_Y && IS_WALKABLE_TILE(column[ubY + 1])) {
        SET_SOUTH(result);
    }
    if (ubX > 0) {
        column = map[loc.ubX - 1];
        if (IS_WALKABLE_TILE(column[ubY])) {
            SET_WEST(result);
        }
        if (ubY > 0 && IS_WALKABLE_TILE(column[ubY - 1])) {
            SET_NW(result);
        }
        if (ubY < MAX_Y && IS_WALKABLE_TILE(column[ubY + 1])) {
            SET_SW(result);
        }
    }
    if (ubX < MAX_X) {
        column = map[loc.ubX + 1];
        if (IS_WALKABLE_TILE(column[ubY])) {
            SET_EAST(result);
        }
        if (ubY > 0 && IS_WALKABLE_TILE(column[ubY - 1])) {
            SET_NE(result);
        }
        if (ubY < MAX_Y && IS_WALKABLE_TILE(column[ubY + 1])) {
            SET_SE(result);
        }
    }
    return result;
}
