#include "game.h"
#include "bob_new.h"
#include "map.h"
#include <ace/managers/copper.h>
#include <ace/managers/log.h>
#include <ace/managers/mouse.h>
#include <ace/types.h>
#include <ace/managers/viewport/camera.h>
#include <ace/utils/bitmap.h>
#include <ace/utils/custom.h>
#include <ace/utils/extview.h>
#include <ace/utils/file.h>
#include <ace/utils/palette.h>
#include <ace/managers/key.h>
#include <ace/managers/game.h>
#include <ace/managers/system.h>
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/viewport/scrollbuffer.h>
#include <ace/managers/viewport/tilebuffer.h>
#include <ace/managers/blit.h>
#include <stdint.h>


#define TILE_SHIFT 4
#define TILE_SIZE (1 << TILE_SHIFT)
#define MAP_SIZE 64

#define BPP 5
#define COLORS (1 << BPP)

static tView *s_pView; // View containing all the viewports
static tVPort *s_pVpPanel; // Viewport for panel
static tSimpleBufferManager *s_pPanelBuffer;
static tVPort *s_pVpMain; // Viewport for playfield
static tTileBufferManager *s_pMapBuffer;
static tCameraManager *s_pMainCamera;
static tBitMap *s_pMapBitmap;

static tBitMap *s_pGoldMineBitmap;
static tBobNew s_GoldMineBob;
static tBobNew s_TileCursor;

static tBobNew s_Spearthrower;
static tBitMap *s_pSpearthrower;
static tBitMap *s_pSpearthrower_mask;

// palette switching
static uint16_t s_pMapPalette[COLORS];
static uint16_t s_pPanelPalette[COLORS];

#define IMGDIR "resources/imgs/"
#define MAPDIR "resources/maps/"
#define LONGEST_MAPNAME "human12.map"

#define MAP_HEIGHT 200
#define PANEL_HEIGHT 48


/*
 * struct map {
 *     tBitMap *terrain;
 *     tTileBufferManager *mapBuffer;
 * };
 */

static void drawBuildingOnTile(UWORD uwTileX, UWORD uwTileY, tBitMap *pBitMap, UWORD uwBitMapX, UWORD uwBitMapY) {
}

void loadMap(const char* name) {
    char* mapname = MAPDIR LONGEST_MAPNAME;
    char* palname = IMGDIR "for.plt";
    char* imgname = IMGDIR "for.bm";

    UWORD tileStartPos = 0;
    UWORD mapColorsPos = tileStartPos + tileBufferGetRawCopperlistInstructionCountStart(BPP);
    UWORD tileBreakPos = mapColorsPos + COLORS;

    snprintf(mapname + strlen(MAPDIR), strlen(LONGEST_MAPNAME) + 1, "%s.map", name);
    tFile *map = fileOpen(mapname, "r");
    if (!map) {
        logWrite("ERROR: Cannot open file %s!\n", mapname);
    }

    // first three bytes are simply name of the palette/terrain
    fileRead(map, palname + strlen(IMGDIR), 3);
    strncpy(imgname + strlen(IMGDIR), palname + strlen(IMGDIR), 3);

    logWrite("Loading map: %s %s\n", palname, imgname);
    // create map area
    paletteLoad(palname, s_pMapPalette, COLORS);
    tCopCmd *pCmds = &s_pView->pCopList->pBackBfr->pList[mapColorsPos];
    for (uint8_t i = 0; i < COLORS; i++) {
        copSetMove(&pCmds[i].sMove, &g_pCustom->color[i], s_pMapPalette[i]);
    }
    pCmds = &s_pView->pCopList->pFrontBfr->pList[mapColorsPos];
    for (uint8_t i = 0; i < COLORS; i++) {
        copSetMove(&pCmds[i].sMove, &g_pCustom->color[i], s_pMapPalette[i]);
    }

    s_pVpMain = vPortCreate(0,
                            TAG_VPORT_VIEW, s_pView,
                            TAG_VPORT_BPP, BPP,
                            TAG_VPORT_HEIGHT, MAP_HEIGHT,
                            TAG_END);
    s_pMapBitmap = bitmapCreateFromFile(imgname, 0);
    s_pMapBuffer = tileBufferCreate(0,
                                    TAG_TILEBUFFER_VPORT, s_pVpMain,
                                    TAG_TILEBUFFER_BITMAP_FLAGS, BMF_CLEAR | BMF_INTERLEAVED,
                                    TAG_TILEBUFFER_BOUND_TILE_X, MAP_SIZE,
                                    TAG_TILEBUFFER_BOUND_TILE_Y, MAP_SIZE,
                                    TAG_TILEBUFFER_TILE_SHIFT, TILE_SHIFT,
                                    TAG_TILEBUFFER_TILESET, s_pMapBitmap,
                                    TAG_TILEBUFFER_IS_DBLBUF, 0,
                                    TAG_TILEBUFFER_REDRAW_QUEUE_LENGTH, 6,
                                    TAG_TILEBUFFER_COPLIST_OFFSET_START, tileStartPos,
                                    TAG_TILEBUFFER_COPLIST_OFFSET_BREAK, tileBreakPos,
                                    TAG_TILEBUFFER_CALLBACK_TILE_DRAW, drawBuildingOnTile,
                                    TAG_END);
    s_pMainCamera = s_pMapBuffer->pCamera;
    cameraSetCoord(s_pMainCamera, 0, 0);

    for (int x = 0; x < MAP_SIZE; x++) {
        fileRead(map, s_pMapBuffer->pTileData[x], MAP_SIZE);
    }
    fileClose(map);

    tileBufferRedrawAll(s_pMapBuffer);
}

void initBobs(void) {
    bobNewManagerCreate(s_pMapBuffer->pScroll->pFront, s_pMapBuffer->pScroll->pBack, s_pMapBuffer->pScroll->uwBmAvailHeight);

    bobNewInit(&s_TileCursor, TILE_SIZE, TILE_SIZE, 1, s_pMapBitmap, 0, 0, 0);
    bobNewSetBitMapOffset(&s_TileCursor, 0x10 << TILE_SHIFT);

    s_pSpearthrower = bitmapCreateFromFile("resources/units/spearthrower.bm", 0);
    s_pSpearthrower_mask = bitmapCreateFromFile("resources/units/spearthrower.msk", 0);
    bobNewInit(&s_Spearthrower, TILE_SIZE * 3, TILE_SIZE * 3, 1, s_pSpearthrower, s_pSpearthrower_mask, 0, 0);

    bobNewReallocateBgBuffers();
}

void gameGsCreate(void) {
    viewLoad(0);

    UWORD tileStartPos = 0;
    UWORD mapColorsPos = tileStartPos + tileBufferGetRawCopperlistInstructionCountStart(BPP);
    UWORD tileBreakPos = mapColorsPos + COLORS;
    UWORD simplePos = tileBreakPos + tileBufferGetRawCopperlistInstructionCountBreak(BPP);
    UWORD panelColorsPos = simplePos + simpleBufferGetRawCopperlistInstructionCount(BPP);
    UWORD copListLength = panelColorsPos + COLORS;

    s_pView = viewCreate(0,
                         TAG_VIEW_COPLIST_MODE, VIEW_COPLIST_MODE_RAW,
                         TAG_VIEW_COPLIST_RAW_COUNT, copListLength,
                         TAG_DONE);

    loadMap("game2");
    
    initBobs();
    // loadGoldmine();

    // create panel area
    // paletteLoad("resources/human_panel.plt", s_pPanelPalette, COLORS);
    // pCmds = &pCopBfrBack->pList[panelColorsPos];
    // for (uint8_t i = 0; i < COLORS; i++) {
    //     copSetMove(&pCmds[i].sMove, &g_pCustom->color[i], s_pPanelPalette[i]);
    // }
    // pCmds = &pCopBfrFront->pList[panelColorsPos];
    // for (uint8_t i = 0; i < COLORS; i++) {
    //     copSetMove(&pCmds[i].sMove, &g_pCustom->color[i], s_pPanelPalette[i]);
    // }
    
    s_pVpPanel = vPortCreate(0,
                             TAG_VPORT_VIEW, s_pView,
                             TAG_VPORT_BPP, BPP,
                             // TAG_VPORT_OFFSET_TOP, 1,
                             TAG_VPORT_HEIGHT, PANEL_HEIGHT,
                             TAG_END);
    logWrite("create panel buffer\n");
    s_pPanelBuffer = simpleBufferCreate(0,
                                        TAG_SIMPLEBUFFER_VPORT, s_pVpPanel,
                                        TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_CLEAR | BMF_INTERLEAVED,
                                        TAG_SIMPLEBUFFER_IS_DBLBUF, 0,
                                        TAG_SIMPLEBUFFER_COPLIST_OFFSET, simplePos,
                                        TAG_END);
    // bitmapLoadFromFile(s_pPanelBuffer->pFront, "resources/human_panel/graphics/ui/human/panel_2.bm", 48, 0);

    viewLoad(s_pView);
    systemUnuse();
}

static tUbCoordYX screenPosToTile(UWORD x, UWORD y) {
    tUbCoordYX pos;
    pos.ubX = x >> TILE_SHIFT;
    pos.ubY = y >> TILE_SHIFT;
    return pos;
}

static uint16_t GameCycle = 0;
static UBYTE SelectedTile = 0x10;

void gameGsLoop(void) {
    tUwCoordYX mousePos = {.uwX = mouseGetX(MOUSE_PORT_1) & 0xfff0, .uwY = mouseGetY(MOUSE_PORT_1) & 0xfff0};

    if (!(GameCycle % 10)) {
        if (keyCheck(KEY_UP)) {
            SelectedTile++;
            bobNewSetBitMapOffset(&s_TileCursor, SelectedTile << TILE_SHIFT);
        } else if (keyCheck(KEY_DOWN)) {
            SelectedTile--;
            bobNewSetBitMapOffset(&s_TileCursor, SelectedTile << TILE_SHIFT);
        } else if (keyCheck(KEY_ESCAPE)) {
            gameExit();
        } else if (keyCheck(KEY_C)) {
            copDumpBfr(s_pView->pCopList->pBackBfr);
        } else if (keyCheck(KEY_RETURN)) {
            const char* mapname = MAPDIR "game.map";
            tFile *map = fileOpen(mapname, "w");
            if (!map) {
                logWrite("ERROR: Cannot open file %s!\n", mapname);
            } else {
                fileWrite(map, "for", 3);
                for (int x = 0; x < MAP_SIZE; x++) {
                    fileWrite(map, s_pMapBuffer->pTileData[x], MAP_SIZE);
                }
            }
        }
    }
    if (mouseCheck(MOUSE_PORT_1, MOUSE_LMB)) {
        tUbCoordYX tile = screenPosToTile(mousePos.uwX, mousePos.uwY);
        tileBufferSetTile(s_pMapBuffer, tile.ubX, tile.ubY, SelectedTile);
        bobNewDiscardUndraw();
    }
    // This will loop every frame
    if (keyCheck(KEY_W)) {
        cameraMoveBy(s_pMainCamera, 0, -4);
    } else if (keyCheck(KEY_S)) {
        cameraMoveBy(s_pMainCamera, 0, 4);
    } else if (keyCheck(KEY_A)) {
        cameraMoveBy(s_pMainCamera, -4, 0);
    } else if (keyCheck(KEY_D)) {
        cameraMoveBy(s_pMainCamera, 4, 0);
    }

    tileBufferQueueProcess(s_pMapBuffer);

    bobNewBegin(s_pMapBuffer->pScroll->pBack);
    s_TileCursor.sPos.ulYX = mousePos.ulYX;
    bobNewPush(&s_TileCursor);

    bobNewPush(&s_Spearthrower);

    bobNewPushingDone();
    bobNewProcessNext();
    bobNewEnd();

    viewProcessManagers(s_pView);
    copProcessBlocks();
    vPortWaitForEnd(s_pVpPanel);

    GameCycle++;
}

void gameGsDestroy(void) {
    systemUse();

    // This will also destroy all associated viewports and viewport managers
    viewDestroy(s_pView);

    bitmapDestroy(s_pMapBitmap);
}
