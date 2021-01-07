#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "con4.hpp"

struct GameState {
  char board[BOARD_WIDTH * BOARD_HEIGHT];
  int nextIndex[BOARD_WIDTH];
  int state;
  int step;
};

#define DEBUG //printf

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

void fillColumn(GameState &game, int inputColumn, int player, int &filledPosition) {
  int i = game.nextIndex[inputColumn];

  if (i >= GAME_HEIGHT + PADDING) {
    game.state = GAME_ERROR_INPUT;
  } else {
    int index =  i * BOARD_WIDTH + inputColumn + PADDING;
    char target = game.board[index];
    if (target == B_EMPTY) {
      game.board[ i * BOARD_WIDTH + inputColumn + PADDING ] = player;
      game.nextIndex[inputColumn]++;
      game.state = GAME_CONTINUE;
      filledPosition = index;
    } else {
      // shouldnt happen. anyway.
      game.state = GAME_ERROR_INPUT;
    }
  }
}

void checkVertical(GameState &game, int position) {
  char target = game.board[position];
  if ((target == game.board[position])
      && (target == game.board[position - BOARD_WIDTH])
      && (target == game.board[position - 2 * BOARD_WIDTH])
      && (target == game.board[position - 3 * BOARD_WIDTH])) {
    DEBUG("Win: vertical\n");
    game.state = GAME_WON;
  }
}

void checkHorizontal(GameState &game, int position) {
  bool isHoriConnected = false;

  auto isConnected = [=](int offset) {
    int startPos = position - offset;
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

void checkDiagonals(GameState &game, int position) {
  bool isDiagConnected = false;
  auto isConnected = [=](int offset, int stepSize) {
    int startPos = position + offset;
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

void checkConnectionAtPosition(GameState &game, int position) {
  do
  {
    checkVertical(game, position);
    if (game.state == GAME_WON) break;
    checkHorizontal(game, position);
    if (game.state == GAME_WON) break;
    checkDiagonals(game, position);
  }while(0);
}

void runGame(GameState &game, int inputColumn, int player) {
  int pos = -1;

  do
  {
    int rv = validateInput(inputColumn);
    if (rv != GAME_CONTINUE) {
      game.state = rv;
      break;
    }

    fillColumn(game, inputColumn, player, pos);
    DEBUG("Filled column %d (Pos: board[%d])\n", inputColumn, pos);
    if (game.state != GAME_CONTINUE) break;

    checkConnectionAtPosition(game, pos);
    printBoard(game.board);
    //printFullBoard(board);

  } while(0);
}

void gameLoop(GameState &game) {
  int32_t input = 0;
  int player = -1;
  while(game.state == GAME_CONTINUE) {
    DEBUG("Gameloop step %u\n", game.step);
    player = (player + 1) % 2;

    if (scanf(" %d", &input) > 0) {
      DEBUG("Gameloop step %u, input: %d\n", game.step, input);
      runGame(game, input, player);
      game.step++;
      if (game.step >= GAME_SIZE) {
        game.state = GAME_DRAW;
      }
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
      printf("WINNER: %s\n", player ? "Player 2" : "Player 1");
      break;
    case GAME_ERROR_INPUT:
      DEBUG("Error input\n");
      break;
    default:
      DEBUG("Unknown Error\n");
      break;
  }
}

void initState(GameState &game) {
  memset(game.board, B_EMPTY, sizeof(game.board));
  game.state = GAME_CONTINUE;
  game.step = 0;

  for(int i=0; i< BOARD_WIDTH; i++) {
    game.nextIndex[i] = PADDING;
  }
}

int main(int argc, char** argv) {
  GameState game;
  initState(game);
  gameLoop(game);
  return 0;
}
