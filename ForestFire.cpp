#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <omp.h>
#include "Timing.h"

using namespace std;

double g;
double p;

HDC deviceContextHandle;

const char EMPTY = '.';
const char TREE = 'T';
const char FIRE = 'x';

const unsigned int BOARD_SIZE = 1024;
vector<vector<char>> board ((size_t) BOARD_SIZE, vector<char>((size_t) BOARD_SIZE, EMPTY));

bool isEmpty (char cell)
{
  return cell == EMPTY;
}
bool isTree (char cell)
{
  return cell == TREE;
}
bool isFire (char cell)
{
  return cell == FIRE;
}

bool hasFireNeighbor (unsigned int rowIndex, unsigned int columnIndex)
{
  int up = rowIndex - 1;
  unsigned int down = rowIndex + 1;
  int left = columnIndex - 1;
  unsigned int right = columnIndex + 1;

  bool hasFireNeighbors = false;

  if (up >= 0)
  {
    hasFireNeighbors |= isFire (board[up][columnIndex]);

    if (left >= 0)
      hasFireNeighbors |= isFire (board[up][left]);
    if (right <= BOARD_SIZE - 1)
      hasFireNeighbors |= isFire (board[up][right]);
  }
  if (down <= BOARD_SIZE - 1)
  {
    hasFireNeighbors |= isFire (board[down][columnIndex]);

    if (left >= 0)
      hasFireNeighbors |= isFire (board[down][left]);
    if (right <= BOARD_SIZE - 1)
      hasFireNeighbors |= isFire (board[down][right]);
  }
  if (left >= 0)
    hasFireNeighbors |= isFire (board[rowIndex][left]);
  if (right <= BOARD_SIZE - 1)
    hasFireNeighbors |= isFire (board[rowIndex][right]);

  return hasFireNeighbors;
}

const COLORREF EMPTY_COLOR = RGB (0, 0, 0);
const COLORREF TREE_COLOR = RGB (34, 139, 34);
const COLORREF FIRE_COLOR = RGB (255, 165, 0);

void DrawEmpty (unsigned int rowIndex, unsigned int columnIndex)
{
  SetPixel (deviceContextHandle, rowIndex, columnIndex, EMPTY_COLOR);
}
void DrawTree (unsigned int rowIndex, unsigned int columnIndex)
{
  SetPixel (deviceContextHandle, rowIndex, columnIndex, TREE_COLOR);
}
void DrawFire (unsigned int rowIndex, unsigned int columnIndex)
{
  SetPixel (deviceContextHandle, rowIndex, columnIndex, FIRE_COLOR);
}

void update ()
{
  vector<vector<char>> newBoard = vector<vector<char>> (BOARD_SIZE, vector<char> (BOARD_SIZE));

#pragma omp parallel for
  for (unsigned int rowIndex = 0; rowIndex < BOARD_SIZE; rowIndex++)
    for (unsigned int columnIndex = 0; columnIndex < BOARD_SIZE; columnIndex++)
    {
      char currentCell = board[rowIndex][columnIndex];

      if (isEmpty (currentCell)
      && (rand () % 100) < g)
      {
        newBoard[rowIndex][columnIndex] = TREE;
        DrawTree (rowIndex, columnIndex);
      }
      else
      if (isTree (currentCell)
      && (hasFireNeighbor(rowIndex, columnIndex)
          || (rand () % 100) < p))
      {
        newBoard[rowIndex][columnIndex] = FIRE;
        DrawFire (rowIndex, columnIndex);
      }
      else
      if (isFire (currentCell))
      {
        newBoard[rowIndex][columnIndex] = EMPTY;
        DrawEmpty (rowIndex, columnIndex);
      }
      else
        newBoard[rowIndex][columnIndex] = currentCell;
    }

  board = newBoard;
}

int main (int argc, char* argv[])
{
  omp_set_num_threads (atoi (argv[2]));
  int generations = atoi (argv[4]);
  g = atoi(argv[6]);
  p = atoi(argv[8]);

  srand(time(NULL));

  HWND consoleHandle = GetConsoleWindow ();
  deviceContextHandle = GetDC (consoleHandle);

  Timing* timing = Timing::getInstance();
  timing->startComputation ();

  for (int generation = 0; generation < generations; generation++) // 1, 10, 100, 1000, 10000
    update ();

  timing->stopComputation ();
  cout << "Time elapsed for " << generations << " generations: " << timing->getResults() << endl;

  ReleaseDC (consoleHandle, deviceContextHandle);

  return 0;
}