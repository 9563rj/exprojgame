#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <iostream>
#include <random>
#include <vector>

const int winXMax = 640;
const int winYMax = 480;
const short fpsLimit = 30;
const short ticksFrame = 32; // fpsLimit

class Enemy
{
public:
  int enemyOffsetX;
  int enemyOffsetY;
  Enemy();

  void Draw();

private:
  SDL_Surface* enemySprite;
};

#endif // MAIN_H
