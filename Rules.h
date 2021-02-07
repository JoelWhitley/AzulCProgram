#ifndef AZUL_RULES
#define AZUL_RULES

// Alias for context + code clarity
typedef char Tile;

// Game sizes and counts
//size determines rows/cols of mosaic, rows of storage, and amount of tile types
#define SIZE           5
#define FACTORIES      5
#define FACTORY_SIZE   4
#define FLOOR_SIZE     7
#define FLOOR_ROW      6    //SIZE+1
#define GAME_TILES     100  //20 of each type (20*SIZE)

// Tile codes
#define FIRST_PLAYER  'F'
#define RED           'R'
#define YELLOW        'Y'
#define BLUE          'B'
#define LIGHT_BLUE    'L'
#define BLACK         'U'
#define NO_TILE       '.'

// Fixed arrays
static const Tile tileSelection[SIZE] = {RED,YELLOW,BLUE,LIGHT_BLUE,BLACK};
static const Tile topRowOrder[SIZE]   = {BLUE,YELLOW,RED,BLACK,LIGHT_BLUE};
static const int demerits[FLOOR_SIZE+1] = {0, -1, -2, -4, -6, -8, -11, -14};

#endif // AZUL_RULES