#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <random>
#include <vector>
using namespace std;

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
  void Move();
  void Hit(vector<Enemy*>::iterator& it);
  
 private:
  SDL_Surface* enemySprite;
};

class Arrow
{
 public:
  int arrowOffsetX;
  int arrowOffsetY;
  int arrowDirection;
  bool exist;
  Arrow(int direction);

  void Draw();
  void Move();

 private:
  SDL_Surface* arrowSprite;
};

#endif // MAIN_H
