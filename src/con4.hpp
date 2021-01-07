#define GAME_WIDTH    (7)
#define GAME_HEIGHT   (6)
#define GAME_SIZE     (GAME_WIDTH * GAME_HEIGHT)
#define WIN_COND      (4)
#define PADDING       (WIN_COND - 1)
#define BOARD_WIDTH   ((GAME_WIDTH + (2 * PADDING)))
#define BOARD_HEIGHT  ((GAME_HEIGHT + (2 * PADDING)))
#define BOARD_SIZE    (BOARD_HEIGHT * BOARD_WIDTH)

#define B_EMPTY         (9)

// Game state def
#define GAME_CONTINUE         (0)
#define GAME_WON              (1)
#define GAME_DRAW             (2)
#define GAME_ERROR_INPUT      (3)

