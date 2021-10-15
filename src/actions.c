#include "actions.h"

enum ActionTypes {
    ActionStill,
    ActionMove,
    ActionAttackMove,
    ActionAttackTarget,
    ActionHarvest,
    ActionCast,
    ActionDie
};

void actionMoveTo(Unit *unit, tUbCoordYX goal) {
    unit->uwActionDataA = goal.uwYX;
    unit->action = ActionMove;
}

#define moveTargetXMask 0b1111110000000000
#define moveTargetYMask 0b0000001111110000
#define moveCounterMask 0b0000000000001111
#define moveTargetXShift 10
#define moveTargetYShift 4
#define moveCounterShift 0
void actionMove(Unit *unit) {
    tUbCoordYX goal;
    goal.uwYX = unit->uwActionDataA;
    
    tUbCoordYX loc = unitGetTilePosition(unit);    

    UnitType type = UnitTypes[unit->type];
    UBYTE speed = type.speed;

    WORD vectorX = ((WORD)goal.ubX - loc.ubX) << TILE_SHIFT;
    WORD vectorY = ((WORD)goal.ubY - loc.ubY) << TILE_SHIFT;
    UWORD absVX = vectorX < 0 ? -vectorX : vectorX;
    UWORD absVY = vectorY < 0 ? -vectorY : vectorY;
    UWORD length = absVX + absVY;
    if (length == 0) {
        // reached goal
        unitSetFrame(unit, 0);
        unit->action = ActionStill;
    }
    if (absVX) {
        unit->bob.sPos.uwX += vectorX / absVX * speed;
    }
    if (absVY) {
        unit->bob.sPos.uwY += vectorY / absVY * speed;
    }
    UBYTE nextFrame = unitGetFrame(unit) ? 0 : 1;
    if (absVX > absVY) {
        if (vectorX > 0) {
            unitSetDirectionAndFrame(unit, DIRECTION_EAST, nextFrame);
        } else {
            unitSetDirectionAndFrame(unit, DIRECTION_WEST, nextFrame);
        }
    } else {
        if (vectorY > 0) {
            unitSetDirectionAndFrame(unit, DIRECTION_SOUTH, nextFrame);
        } else {
            unitSetDirectionAndFrame(unit, DIRECTION_NORTH, nextFrame);
        }
    }
};

void actionAttackMove(Unit  __attribute__((__unused__)) *unit) {
    return;
};

void actionAttackTarget(Unit  __attribute__((__unused__)) *unit) {
    return;
};

void actionHarvest(Unit  __attribute__((__unused__)) *unit) {
    return;
};

void actionCast(Unit  __attribute__((__unused__)) *unit) {
    return;
};

void actionDie(Unit  __attribute__((__unused__)) *unit) {
    return;
};

void actionDo(Unit *unit) {
    switch (unit->action) {
        case ActionStill:
            return;
        case ActionMove:
            actionMove(unit);
            return;
        // TODO:
    }
}
