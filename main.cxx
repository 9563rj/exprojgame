#include "main.h"
using namespace std;

bool gameRunning = true;
SDL_Event event;
SDL_Window* window = NULL;
SDL_Surface* surface = NULL;
SDL_Surface* hello = NULL;
SDL_Surface* player = NULL;

// map variables
int tileSize = 16;
int mapX = winXMax/tileSize/2-1;
int mapXPixels = mapX*tileSize;
int mapY = winYMax/tileSize-1;
int mapYPixels = mapY*tileSize;
int mapXZero = winXMax/2+tileSize/2;
int mapYZero = (winYMax-mapYPixels)/2;
int mapXCenter = mapXZero+(mapXPixels/2);
int mapYCenter = mapYZero+(mapYPixels/2);
int mapWidth = mapXPixels/2;
int mapHeight = mapYPixels/2;
int playerOffsetX = mapXCenter;
int playerOffsetY = mapYCenter;

// Frame limiting
Uint32 startTime = 0;
Uint32 endTime = 0;

// Keypress bools
bool wKeyPress = false;
bool aKeyPress = false;
bool sKeyPress = false;
bool dKeyPress = false;

// Spawning
Uint32 lastSpawn = 0;
Uint32 sinceSpawn = 0;
Uint32 spawnInterval;
vector<Enemy*> enemies;

// Random engine initializer
random_device rd;
mt19937 eng(rd());
uniform_int_distribution<> spawnIntervalDist(3000,10000);
uniform_int_distribution<> spawnSidesRange(mapYCenter-mapHeight,mapYCenter+mapHeight);
uniform_int_distribution<> edgePicker(0,3);
uniform_int_distribution<> spawnTopsRange(mapXCenter-mapWidth,mapXCenter+mapWidth);

// Enemy constructor
Enemy::Enemy()
{
  switch(edgePicker(eng))
    {
    case 0:
      cout << "Spawning edge O..." << endl;
      enemyOffsetX = mapXCenter-mapWidth+(tileSize/2);
      enemyOffsetY = spawnSidesRange(eng);
      break;

    case 1:
      cout << "Spawning edge 1..." << endl;
      enemyOffsetX = spawnTopsRange(eng);
      enemyOffsetY = mapYCenter-mapHeight+(tileSize/2);
      break;

    case 2:
      cout << "Spawning edge 2..." << endl;
      enemyOffsetX = mapXCenter+mapWidth-(tileSize/2);
      enemyOffsetY = spawnSidesRange(eng);
      break;

    case 3:
      cout << "Spawning edge 3..." << endl;
      enemyOffsetX = spawnTopsRange(eng);
      enemyOffsetY = mapYCenter+mapHeight-(tileSize/2);
      break;
    }
  enemySprite = SDL_LoadBMP("enemy.bmp");
  if(enemySprite == NULL)
    {
      cout << "Failed to load enemy.bmp" << endl;
      exit(1);
    }
}

void Enemy::Draw()
{
  // Apply enemy
  SDL_Rect enemyDest = {enemyOffsetX-(tileSize/2), enemyOffsetY-(tileSize/2), enemyOffsetX+(tileSize/2), enemyOffsetY+(tileSize/2)};
  SDL_BlitSurface(enemySprite, NULL, surface, &enemyDest);
  SDL_UpdateWindowSurface(window);
}
  
void spawnEnemy()
{
  cout << "SpawnEnemy called" << endl;
  Enemy* new_Enemy = new Enemy();
  enemies.push_back(new_Enemy);
  lastSpawn = SDL_GetTicks();
}

void drawEnemies()
{
  vector<Enemy*>::iterator it;
  for(it=enemies.begin(); it!=enemies.end(); it++)
    {
      Enemy* enemy = *it;
      enemy->Draw();
    }
}

bool drawPlayer()
{
  // Apply player
  SDL_Rect playerDest = {playerOffsetX-(tileSize/2), playerOffsetY-(tileSize/2), playerOffsetX+(tileSize/2), playerOffsetY+(tileSize/2)};
  SDL_BlitSurface(player, NULL, surface, &playerDest);
  SDL_UpdateWindowSurface(window);
}

bool init()
{
  // Init success flag
  bool success = true;

  // SDL init
  if(SDL_Init(SDL_INIT_VIDEO)<0)
    {
      cout << "SDL could not initialize. Error:" << SDL_GetError() << endl;
      success = false;
    }
  else
    {
      // Create window
      window = SDL_CreateWindow("TestGraphics", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winXMax, winYMax, SDL_WINDOW_SHOWN);
      // Window debug
      if(window == NULL)
	{
	  cout << "Window unable to be created. Error:" << SDL_GetError() << endl;
	  success = false;
	  
	}
      else
	{

	  // Grab surface
	  surface = SDL_GetWindowSurface(window);
	}
    }
  spawnInterval = spawnIntervalDist(eng);
  cout << spawnInterval << endl;
  return success;
}

bool frameHandler()
{
  // Init success flag
  bool success = true;

  // Load bitmaps
  hello = SDL_LoadBMP("grass.bmp");
  player = SDL_LoadBMP("player.bmp");
  
  // Debug
  if(hello == NULL || player == NULL)
    {
      cout << "Couldn't load texture. Error:" << SDL_GetError() << endl;
      success = false;
    }
  else
    {
      // Apply grass tiling
      for(int i=0;i<mapX;i++)
	{
	  for(int j=0;j<mapY;j++)
	    {
	      SDL_Rect dstrect = {i*tileSize+mapXZero, j*tileSize+mapYZero, (i+1)*tileSize+mapXZero, (j+1)*tileSize+mapYZero};
	      SDL_BlitSurface(hello, NULL, surface, &dstrect);
	    }
	}
    }
  return success;
}

void cleanup()
{
  // Deallocate surfaces
  SDL_FreeSurface(hello);
  hello = NULL;

  // Delete window
  SDL_DestroyWindow(window);
  window = NULL;

  SDL_Quit();
}

int main(int argc, char* argv[])
{
  if(!init())
    {
      cout << "Failed to initialize." << endl;
    }
  else
    {
      if(!frameHandler())
	{
	  cout << "Failed to load image." << endl;
	}
    }
  while(gameRunning)
    {
      // Frame limiting init
      Uint32 delta = 0;
      if(!startTime)
	{
	  startTime = SDL_GetTicks();
	}
      else
	{
	  delta = endTime - startTime;
	}
      // Spawning timer
      Uint32 time = SDL_GetTicks();
      cout << sinceSpawn << endl << spawnInterval << endl;
      if (sinceSpawn > spawnInterval)
	{
	  spawnEnemy();
	  spawnInterval = spawnIntervalDist(eng);
	}
      sinceSpawn = time-lastSpawn;
      
      // Input handler
      SDL_PumpEvents();
      const Uint8* keys = SDL_GetKeyboardState(NULL);
      wKeyPress = keys[SDL_SCANCODE_W];
      aKeyPress = keys[SDL_SCANCODE_A];
      sKeyPress = keys[SDL_SCANCODE_S];
      dKeyPress = keys[SDL_SCANCODE_D];
      if(keys[SDL_SCANCODE_ESCAPE]) {gameRunning = false;}
      
      // Keypress bool handler
      if(wKeyPress) {playerOffsetY--;}
      if(aKeyPress) {playerOffsetX--;}
      if(sKeyPress) {playerOffsetY++;}
      if(dKeyPress) {playerOffsetX++;}
      frameHandler();
      drawPlayer();
      drawEnemies();

      // Frame limiter
      if(delta<ticksFrame)
	{
	  SDL_Delay(ticksFrame - delta);
	}
      startTime = endTime;
      endTime = SDL_GetTicks();
    }
  cleanup();
  return 0;
}
