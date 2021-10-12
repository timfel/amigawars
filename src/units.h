#ifndef UNITS_H
#define UNITS_H

#include "bob_new.h"

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
    warlock
};

extern UnitType UnitTypes[];

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

#define UNIT_SIZE_SHIFT 6
// _Static_assert(sizeof(Unit) == (1 << UNIT_SIZE_SHIFT) >> 3, "unit struct is not 4 words");
// _Static_assert(sizeof(Unit) == 4 * sizeof(UWORD), "unit struct is not 4 words");

void unitManagerCreate(void);

void unitManagerDestroy(void);

Unit * unitNew(UnitType *);

void unitDelete(Unit *);

static inline tUwCoordYX * unitLocation(Unit *self) {
    return &self->bob.sPos;
}

#endif
