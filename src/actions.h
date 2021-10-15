#ifndef ACTIONS_H
#define ACTIONS_H

#include "units.h"
#include "ace/types.h"

void actionDo(Unit *unit);
void actionMoveTo(Unit *unit, tUbCoordYX goal);

#endif
