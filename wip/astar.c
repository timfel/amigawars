#include <ace/types.h>
#include <ace/macros.h>

typedef struct {
    tUbCoordYX id;
    // edges are implicit
} Node;

typedef struct {
    UBYTE edgeCost;
} Cost;

/**
 * Must pass in two arrays of 8 elements
 */
void nodeNeighbours(Node self, Node *nodes, Cost *cost) {
    tUbCoordYX yx = self.id;
    // u,d,l,r,tl,tr,bl,br
    cost[0].edgeCost = 1;
    cost[1].edgeCost = 1;
    cost[2].edgeCost = 1;
    cost[3].edgeCost = 1;
    cost[4].edgeCost = 2;
    cost[5].edgeCost = 2;
    cost[6].edgeCost = 2;
    cost[7].edgeCost = 2;
    // up, down, left, right
    nodes[0].id.ubX = yx.ubX;
    nodes[0].id.ubY = yx.ubY - 1;
    nodes[1].id.ubX = yx.ubX;
    nodes[1].id.ubY = yx.ubY + 1;
    nodes[2].id.ubX = yx.ubX - 1;
    nodes[2].id.ubY = yx.ubY;
    nodes[3].id.ubX = yx.ubX + 1;
    nodes[3].id.ubY = yx.ubY;
    // tl, tr, bl, br
    nodes[4].id.ubX = yx.ubX - 1;
    nodes[4].id.ubY = yx.ubY - 1;
    nodes[5].id.ubX = yx.ubX + 1;
    nodes[5].id.ubY = yx.ubY - 1;
    nodes[6].id.ubX = yx.ubX - 1;
    nodes[6].id.ubY = yx.ubY + 1;
    nodes[7].id.ubX = yx.ubX + 1;
    nodes[7].id.ubY = yx.ubY + 1;
}

void nodeNeighbourAt(Node self, UBYTE dx, UBYTE dy, Node *out, UBYTE *cost) {
    *cost = dx + dy;
    out->id.ubX = self.id.ubX + dx;
    out->id.ubY = self.id.ubY + dy;    
}

