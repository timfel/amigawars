#ifndef UNITS_H
#define UNITS_H

#include "bob_new.h"
#include "map.h"

#include <ace/utils/bitmap.h>
#include <ace/utils/file.h>

// TODO: this can be configurable for different systems
#define FRAME_SIZE 32
#define WALK_FRAMES 2
#define ATTACK_FRAMES 3

typedef struct {
    union {
        const char *spritesheetPath;
        tBitMap *spritesheet;
    };
    union {
        const char *maskPath;
        tBitMap *mask;
    };
    UBYTE maxHP;
    unsigned hasMana:1;
    unsigned isBuilding:1;
    unsigned speed:4;
} UnitType;

enum UnitTypes {
    dead = 0,
    //
    peasant,
    peon,
    footman,
    grunt,
    archer,
    spearman,
    catapult,
    knight,
    raider,
    cleric,
    necrolyte,
    conjurer,
    warlock,
    spider,
    daemon,
    elemental,
    ogre,
    slime,
    thief
};

typedef struct {
    union {
        struct {
            unsigned padding1:8;
            unsigned padding2:7;
            unsigned isBuilding:1;
        };
        struct {
            unsigned unitHp:8;
            unsigned mana:7;
        };
        struct {
            unsigned buildingHp:15;
        };
    };
} UnitStats;

_Static_assert(sizeof(UnitStats) == sizeof(UWORD), "unit stats is not 1 word");

typedef struct {
    unsigned type:6;
    unsigned currentAction:4;
    unsigned frame:6;
    // the frame doesn't really need to be in the header. But the current action
    // is basically always needed, and the type is needed by almost all actions.
    // So there's a little bit of room, and many actions do care about the frame,
    // so it's okay to keep it here.
} UnitHeader;

_Static_assert(sizeof(UnitHeader) == sizeof(UWORD), "unit header is not 1 word");

typedef struct {
    UnitHeader header;
    UWORD actionData;
    UnitStats stats;
    tBobNew bob;
} Unit;

#define UNIT_SIZE 32
#define UNIT_SIZE_SHIFT 5

/* Units are centered on a 2x2 tilegrid. So to position a unit on a tile, we subtract 8px */
#define UNIT_POSITION_OFFSET 8

// _Static_assert(sizeof(Unit) == (1 << UNIT_SIZE_SHIFT) >> 3, "unit struct is not 4 words");
// _Static_assert(sizeof(Unit) == 4 * sizeof(UWORD), "unit struct is not 4 words");

/* The global list of unit types */
extern UnitType UnitTypes[];

/* The maximum number of units the game will allocate memory for. */
#define MAX_UNITS 200

/* The list of all units. Inactive unit slots are NULL. */
extern Unit **s_pUnitList;

void unitManagerCreate(void);

void unitManagerDestroy(void);

Unit * unitNew(UnitType *);

void unitDelete(Unit *);

_Static_assert(MAP_SIZE * TILE_SIZE < 0xfff, "map is small enough to fit locations in bytes");
static inline tUbCoordYX unitGetTilePosition(Unit *self) {
    tUbCoordYX loc;
    tUwCoordYX bobLoc = self->bob.sPos;
    loc.ubX = (bobLoc.uwX >> TILE_SHIFT) + 1;
    loc.ubY = (bobLoc.uwY >> TILE_SHIFT) + 1;
    return loc;
}

static inline void unitSetTilePosition(Unit *self, tUbCoordYX pos) {
    self->bob.sPos.uwX = (pos.ubX << TILE_SHIFT) - 8;
    self->bob.sPos.uwY = (pos.ubY << TILE_SHIFT) - 8;
}

static inline void unitDraw(Unit *self) {
    bobNewPush(&self->bob);
}

static inline void unitSetFrame(Unit *self, UBYTE ubFrame) {
    // TODO: guard that we're not double buffered
    // When we're single buffered, we can re-use some memory in the bob
    self->bob.pOldPositions[1].uwY = ubFrame;
    bobNewSetBitMapOffset(&self->bob, ubFrame);
}

static inline UBYTE unitGetFrame(Unit *self) {
    // TODO: guard that we're not double buffered
    // When we're single buffered, we can re-use some memory in the bob
    return self->bob.pOldPositions[1].uwY;
}

#endif
