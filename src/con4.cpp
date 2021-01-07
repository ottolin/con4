#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "con4.hpp"

#define DEBUG //printf

struct GameState {
  char board[BOARD_WIDTH * BOARD_HEIGHT];
  int nextIndex[BOARD_WIDTH];
  int lastFilledPos;
  int state;
  int step;
  int player;

  GameState()
    :lastFilledPos(-1)
     ,state(GAME_CONTINUE)
     ,step(0)
     ,player(-1) {
       memset(board, B_EMPTY, sizeof(board));
       for(int i=0; i< BOARD_WIDTH; i++) {
         nextIndex[i] = PADDING;
       }
     }
};

void printFullBoard(char (&board)[BOARD_SIZE]) {
  for(int i = BOARD_HEIGHT-1; i>= 0; i--) {
    for(int j = 0; j< BOARD_WIDTH ; j++) {
      DEBUG("%d ", board[ (i * BOARD_WIDTH) + j ]);
    }
    DEBUG("\n");
  }
}

void printBoard(char (&board)[BOARD_SIZE]) {
  for(int i = GAME_HEIGHT + PADDING -1; i>= PADDING; i--) {
    for(int j = PADDING; j < GAME_WIDTH + PADDING ; j++) {
      DEBUG("%d ", board[ ((i * BOARD_WIDTH) + j )]);
    }
    DEBUG("\n");
  }
}

int validateInput(int inputColumn) {
  if (inputColumn < 0 || inputColumn >= 7) {
    return GAME_ERROR_INPUT;
  } else {
    return GAME_CONTINUE;
  }
}

// filling up the board with user input
void fillColumn(GameState &game, int inputColumn) {
  int i = game.nextIndex[inputColumn];

  if (i >= GAME_HEIGHT + PADDING) {
    game.state = GAME_ERROR_INPUT;
  } else {
    int index =  i * BOARD_WIDTH + inputColumn + PADDING;
    char target = game.board[index];
    if (target == B_EMPTY) {
      game.board[ i * BOARD_WIDTH + inputColumn + PADDING ] = game.player;
      game.nextIndex[inputColumn]++;
      game.state = GAME_CONTINUE;
      game.lastFilledPos = index;
    } else {
      // shouldnt happen. anyway.
      game.state = GAME_ERROR_INPUT;
    }
  }
}

// Check the only vertical winning condition
void checkVertical(GameState &game) {
  int position = game.lastFilledPos;
  char target = game.board[position];
  if ((target == game.board[position])
      && (target == game.board[position - BOARD_WIDTH])
      && (target == game.board[position - 2 * BOARD_WIDTH])
      && (target == game.board[position - 3 * BOARD_WIDTH])) {
    DEBUG("Win: vertical\n");
    game.state = GAME_WON;
  }
}

// Check horizontal winning conditions
void checkHorizontal(GameState &game) {
  bool isHoriConnected = false;
  auto isConnected = [=](int offset) {
    int startPos = game.lastFilledPos - offset;
    char target = game.board[startPos];
    if ((target == game.board[startPos + 1])
        && (target == game.board[startPos + 2])
        && (target == game.board[startPos + 3])) {
      return true;
    } else {
      return false;
    }
  };

  for(int i = 0; i< WIN_COND; i++) {
    isHoriConnected = isHoriConnected || isConnected(i);
    if (isHoriConnected) {
      DEBUG("Win: horizontal\n");
      game.state = GAME_WON;
      break;
    }
  }
}

// Check diagonal winning conditions
// for both positive and negative slope
void checkDiagonals(GameState &game) {
  bool isDiagConnected = false;
  auto isConnected = [=](int offset, int stepSize) {
    int startPos = game.lastFilledPos + offset;
    char target = game.board[startPos];
    if ((target == game.board[startPos - stepSize])
        && (target == game.board[startPos - 2 * stepSize])
        && (target == game.board[startPos - 3 * stepSize])) {
      return true;
    } else {
      return false;
    }
  };

  int negStep = BOARD_WIDTH -1;
  int posStep = BOARD_WIDTH +1;
  for(int i = 0; i< WIN_COND; i++) {
    isDiagConnected = isDiagConnected || isConnected(i * negStep, negStep) || isConnected(i * posStep, posStep);
    if (isDiagConnected) {
      DEBUG("Win: diagonal\n");
      game.state = GAME_WON;
      break;
    }
  }
}

void checkConnections(GameState &game) {
  do
  {
    checkVertical(game);
    if (game.state == GAME_WON) break;
    checkHorizontal(game);
    if (game.state == GAME_WON) break;
    checkDiagonals(game);
  }while(0);
}

void runGame(GameState &game, int inputColumn) {
  game.player = (game.player + 1) % 2;
  game.step++;

  do
  {
    int rv = validateInput(inputColumn);
    if (rv != GAME_CONTINUE) {
      game.state = rv;
      break;
    }

    fillColumn(game, inputColumn);
    DEBUG("Filled column %d (Pos: board[%d])\n", inputColumn, game.lastFilledPos);
    if (game.state != GAME_CONTINUE) break;

    checkConnections(game);
    printBoard(game.board);
    //printFullBoard(board);

  } while(0);

  if (game.state == GAME_CONTINUE && game.step >= GAME_SIZE) {
    game.state = GAME_DRAW;
  }
}

void gameLoop(GameState &game) {
  int32_t input = 0;
  while(game.state == GAME_CONTINUE) {
    DEBUG("Gameloop step %u\n", game.step);

    if (scanf(" %d", &input) > 0) {
      DEBUG("Gameloop step %u, input: %d\n", game.step, input);
      runGame(game, input);
    } else {
      // input fail
      game.state = GAME_ERROR_INPUT;
      break;
    }
  }
  DEBUG("Game ends, steps: %u, final state: %d\n", game.step, game.state);
  switch(game.state) {
    case GAME_DRAW:
      printf("DRAW\n");
      break;
    case GAME_WON:
      printf("WINNER: %s\n", game.player ? "Player 2" : "Player 1");
      break;
    case GAME_ERROR_INPUT:
      DEBUG("Error input\n");
      break;
    default:
      DEBUG("Unknown Error\n");
      break;
  }
}

////////////////////////
// Tests
//

void test01() {
  GameState game;
  int input[] = { 0, 1, 0, 1, 0, 1, 0 };
  for (int i=0; i<7; i++) {
    runGame(game, input[i]);
  }
  assert(game.state == GAME_WON);
  assert(game.player == 0);
}

void test02() {
  GameState game;
  int input[] = { 0,0,0,1,1,2,1,2,2,3,3,3,3 };
  for (int i=0; i<13; i++) {
    runGame(game, input[i]);
  }
  assert(game.state == GAME_WON);
  assert(game.player == 0);
}

void test03() {
  GameState game;
  int input[] = { 0,0,1,1,2,2,3 };
  for (int i=0; i<7; i++) {
    runGame(game, input[i]);
  }
  assert(game.state == GAME_WON);
  assert(game.player == 0);
}

void test04() {
  GameState game;
  int input[] = { 0,1,3,0,1,0,0,0,1,2,2 };
  for (int i=0; i<11; i++) {
    runGame(game, input[i]);
  }
  assert(game.state == GAME_WON);
  assert(game.player == 0);
}

void test05() {
  // invalid input test
  GameState game;
  int input[] = { 0,1,3,9 };
  for (int i=0; i<4; i++) {
    runGame(game, input[i]);
  }
  assert(game.state == GAME_ERROR_INPUT);
}

void test06() {
  // draw test
  GameState game;
  int input[] = { 0,1,0,1,0,1,1,0,1,0,1,0,2,3,2,3,2,3,3,2,3,2,3,2,4,5,4,5,4,5,5,4,5,4,5,4,6,6,6,6,6,6 };
  for (int i=0; i<42; i++) {
    runGame(game, input[i]);
  }
  assert(game.state == GAME_DRAW);
}

void runTest() {
  test01();
  test02();
  test03();
  test04();
  test05();
  test06();
}

////////////////////////
// Entry
//
int main(int argc, char** argv) {
  if (argc == 1) {
    GameState game;
    gameLoop(game);
  } else {
    runTest();
  }
  return 0;
}

