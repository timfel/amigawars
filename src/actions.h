#ifndef ACTIONS_H
#define ACTIONS_H

#include "units.h"
#include "ace/types.h"

typedef void (*action)(Unit *);

void actionStill(Unit *);
void actionMove(Unit *);
void actionAttackMove(Unit *);
void actionAttackTarget(Unit *);
void actionHarvest(Unit *);
void actionCast(Unit *);
void actionDie(Unit *);

typedef enum ActionTypes {
    ActionStill,
    ActionMove,
    ActionAttackMove,
    ActionAttackTarget,
    ActionHarvest,
    ActionCast,
    ActionDie
} ActionTypes;

extern action Actions[];

#endif
