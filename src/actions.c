#include "actions.h"

action Actions[] = {
    actionStill,
    actionMove,
    actionAttackMove,
    actionAttackTarget,
    actionHarvest,
    actionCast,
    actionDie
};

void actionStill(Unit  __attribute__((__unused__)) *unit) {
    return;
};

#define moveTargetXMask 0b1111110000000000
#define moveTargetYMask 0b0000001111110000
#define moveCounterMask 0b0000000000001111
#define moveTargetXShift 10
#define moveTargetYShift 4
#define moveCounterShift 0
void actionMove(Unit *unit) {
    UWORD data = unit->actionData;
    UBYTE counter = (data & moveCounterMask) >> moveCounterShift;

    UnitHeader hdr = unit->header;
    UnitType type = UnitTypes[hdr.type];

    // not moving in this step
    if (counter > type.speed) {
        counter -= type.speed;
        return;
    }

    // reset counter
    counter = moveCounterMask;

    // set next movement frame
    // if (hdr.frame == type.stillFrame) {
    //     hdr.frame = type.moveFrame;
    // } else {
    //     hdr.frame = type.stillFrame;
    // }

    // calculate next position
    UBYTE targetX = (data & moveTargetXMask) >> moveTargetXShift;
    UBYTE targetY = (data & moveTargetYMask) >> moveTargetYShift;
    tUbCoordYX loc = unitGetTilePosition(unit);
    UBYTE currentX = loc.ubX;
    UBYTE currentY = loc.ubY;
    if (currentX == targetX && currentY == targetY) {
        hdr.currentAction = ActionStill;
    }
    BYTE dirX = targetX - currentX;
    BYTE dirY = targetY - currentY;
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
