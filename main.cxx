#include "main.h"
using namespace std;

bool gameRunning = true;
bool gameOverRestart = false;
SDL_Event event;
SDL_Window* window = NULL;
SDL_Surface* surface = NULL;
SDL_Surface* hello = NULL;
SDL_Surface* player = NULL;
SDL_Surface* heart = NULL;
SDL_Surface* splashSprite = NULL;
SDL_Surface* deathSprite = NULL;
SDL_Surface* waveIndicator = NULL;
int playerHealth = 3;
int playerInvincibilityFrames = 0;
int score = 0;
SDL_Renderer *gRenderer = NULL;

// Music
int songState = 0;
bool songPlaying = false;
Mix_Music *TheLastStand = NULL;
Mix_Music *PrimoVictoria = NULL;
Mix_Music *Bismarck = NULL;
Mix_Music *GhostInTheTrenches = NULL;
Mix_Music *BackInControl = NULL;
Mix_Music *DevilDogs = NULL;
Mix_Music *AttackOfTheDeadMen = NULL;
Mix_Music *SevenPillarsOfWisdom = NULL;
Mix_Music *GreatWar = NULL;
Mix_Music *PanzerBattalion = NULL;
Mix_Music *Wolfpack = NULL;
Mix_Music *ToHellAndBack = NULL;

Mix_Chunk *killEffect = NULL;
Mix_Chunk *hurtSound = NULL;
Mix_Chunk *deathEffect = NULL;

// Waves
bool wave = true;
int waveCount = 0;
int waveWait = 0;
int spawnDecrease = 0;

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
int mapLeft = mapXCenter-mapWidth;
int mapRight = mapXCenter+mapWidth;
int mapTop = mapYCenter-mapHeight;
int mapBottom = mapYCenter+mapHeight;
int sidebarX = tileSize/2;
int sidebarY = tileSize/2;
int sidebarWidthInt = mapWidth*2;
int sidebarHeightInt = mapHeight*2;
Uint32 sidebarWidth = sidebarWidthInt;
Uint32 sidebarHeight = sidebarHeightInt;

// Frame limiting
Uint32 startTime = 0;
Uint32 endTime = 0;

// Keypress bools
bool wKeyPress = false;
bool aKeyPress = false;
bool sKeyPress = false;
bool dKeyPress = false;
bool upKeyPress = false;
bool leftKeyPress = false;
bool downKeyPress = false;
bool rightKeyPress = false;

// Spawning
Uint32 lastSpawn = 0;
Uint32 sinceSpawn = 0;
Uint32 spawnInterval;
Uint32 lastShoot = 0;
Uint32 sinceShoot = 0;
Uint32 shootInterval = 750;
vector<Enemy*> enemies;
vector<Arrow*> arrows;

// Random engine initializer
random_device rd;
mt19937 eng(rd());
uniform_int_distribution<> spawnIntervalDist(3000,10000);
uniform_int_distribution<> spawnSidesRange(mapYCenter-mapHeight,mapYCenter+mapHeight);
uniform_int_distribution<> edgePicker(0,3);
uniform_int_distribution<> spawnTopsRange(mapXCenter-mapWidth,mapXCenter+mapWidth);
uniform_int_distribution<> moveProbability(0,4);
uniform_int_distribution<> songPicker(0,2);
uniform_int_distribution<> titleTrackPicker(0,1);
uniform_int_distribution<> scoreAmount(10,20);

void gameOver()
{
  SDL_Rect deathScreenDest = {0, 0, winXMax, winYMax};
  SDL_BlitSurface(deathSprite, NULL, surface, &deathScreenDest);
  SDL_UpdateWindowSurface(window);
  Mix_PlayChannel(-1, deathEffect, 0);
  Mix_PlayMusic(ToHellAndBack, -1);
  bool gameOverRunning = true;
  while(gameOverRunning)
    {
      SDL_PumpEvents();
      const Uint8* keys = SDL_GetKeyboardState(NULL);
      if(keys[SDL_SCANCODE_ESCAPE])
	{
	  gameOverRunning = false;
	  gameRunning = false;
	}
    }
  SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderFillRect(gRenderer, &deathScreenDest);
  Mix_HaltMusic();
}

bool musicHandler()
{
  songPlaying = Mix_PlayingMusic();
  if(!songPlaying)
    {
      switch(songState)
	{
	case 0:
	  switch(songPicker(eng))
	    {
	    case 0:
	      Mix_PlayMusic(Bismarck, 0);
	      break;

	    case 1:
	      Mix_PlayMusic(GhostInTheTrenches, 0);
	      break;

	    case 2:
	      Mix_PlayMusic(BackInControl, 0);
	      break;
	    }
	  break;

	case 1:
	  switch(songPicker(eng))
	    {
	    case 0:
	      Mix_PlayMusic(DevilDogs, 0);
	      break;

	    case 1:
	      Mix_PlayMusic(AttackOfTheDeadMen, 0);
	      break;

	    case 2:
	      Mix_PlayMusic(SevenPillarsOfWisdom, 0);
	      break;
	    }
	  break;

	case 2:
	  switch(songPicker(eng))
	    {
	    case 0:
	      Mix_PlayMusic(GreatWar, 0);
	      break;

	    case 1:
	      Mix_PlayMusic(PanzerBattalion, 0);
	      break;

	    case 2:
	      Mix_PlayMusic(Wolfpack, 0);
	      break;
	    }
	  break;
	}
      if(songState == 2)
	{
	  songState = 0;
	}
      else {songState++;}
    }
}

void splashScreen()
{
  bool splashScreenRunning = true;
  SDL_Rect splashSpriteDest = {0, 0, winXMax, winYMax};
  SDL_BlitSurface(splashSprite, NULL, surface, &splashSpriteDest);
  SDL_UpdateWindowSurface(window);
  switch(titleTrackPicker(eng))
    {
    case 0:
      Mix_PlayMusic(TheLastStand, 0);
      break;

    case 1:
      Mix_PlayMusic(PrimoVictoria, 0);
      break;
    }
  while(splashScreenRunning)
    {
      SDL_PumpEvents();
      const Uint8* keys = SDL_GetKeyboardState(NULL);
      if(keys[SDL_SCANCODE_RETURN]) {splashScreenRunning = false;}
    }
  Mix_HaltMusic();
  SDL_Rect black = {0, 0, winXMax, winYMax};
  SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderFillRect(gRenderer, &black);
}

void playerHit()
{
  playerHealth--;
  playerInvincibilityFrames = 100;
  Mix_PlayChannel(-1, hurtSound, 0);
  if(playerHealth <= 0)
    {
      gameOver();
    }
}

// Enemy constructor
Enemy::Enemy()
{
  switch(edgePicker(eng))
    {
    case 0:
      enemyOffsetX = mapXCenter-mapWidth+(tileSize/2);
      enemyOffsetY = spawnSidesRange(eng);
      break;

    case 1:
      enemyOffsetX = spawnTopsRange(eng);
      enemyOffsetY = mapYCenter-mapHeight+(tileSize/2);
      break;

    case 2:
      enemyOffsetX = mapXCenter+mapWidth-(tileSize/2);
      enemyOffsetY = spawnSidesRange(eng);
      break;

    case 3:
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

// Arrow constructor
Arrow::Arrow(int direction)
{
  arrowOffsetX = playerOffsetX;
  arrowOffsetY = playerOffsetY;
  arrowDirection = direction;
  switch(arrowDirection)
    {
    case 0:
      arrowSprite = SDL_LoadBMP("arrow0.bmp");
      break;

    case 1:
      arrowSprite = SDL_LoadBMP("arrow1.bmp");
      break;

    case 2:
      arrowSprite = SDL_LoadBMP("arrow2.bmp");
      break;

    case 3:
      arrowSprite = SDL_LoadBMP("arrow3.bmp");
      break;
    }
  if(arrowSprite == NULL)
    {
      cout << "Failed to load one of the arrow sprites." << endl;
      exit(1);
    }
  exist = true;
}

void spawnArrow(int direction)
{
  Arrow* new_Arrow = new Arrow(direction);
  arrows.push_back(new_Arrow);
  lastShoot = SDL_GetTicks();
}

void Arrow::Draw()
{
  // Apply arrow
  SDL_Rect arrowDest = {arrowOffsetX-(tileSize/2), arrowOffsetY-(tileSize/2), arrowOffsetX+(tileSize/2), arrowOffsetY+(tileSize/2)};
  SDL_BlitSurface(arrowSprite, NULL, surface, &arrowDest);
   
}

void Arrow::Move()
{
  switch(arrowDirection)
    {
    case 0:
      if(arrowOffsetY > mapTop+(tileSize/2))
	{
	  arrowOffsetY--;
	}
      else
	{
	  exist = false;
	}
      break;

    case 1:
      if(arrowOffsetX > mapLeft+(tileSize/2))
	{
	  arrowOffsetX--;
	}
      else
	{
	  exist = false;
	}
      break;

    case 2:
      if(arrowOffsetY < mapBottom-(tileSize/2))
	{
	  arrowOffsetY++;
	}
      else
	{
	  exist = false;
	}
      break;

    case 3:
      if(arrowOffsetX < mapRight-(tileSize/2))
	{
	  arrowOffsetX++;
	}
      else
	{
	  exist = false;
	}
      break;
    }
  vector<Enemy*>::iterator it;
  for(it=enemies.begin(); it!=enemies.end(); it++)
    {
      Enemy* enemy = *it;
      if(arrowOffsetX > enemy->enemyOffsetX-tileSize && arrowOffsetX < enemy->enemyOffsetX+tileSize && arrowOffsetY > enemy->enemyOffsetY-tileSize && arrowOffsetY < enemy->enemyOffsetY+tileSize)
	{
	  enemy->Hit(it);
	  it--;
	  exist = false;
	}
    }
}

void Enemy::Hit(vector<Enemy*>::iterator& it)
{
  score += scoreAmount(eng);
  cout << score << endl;
  Mix_PlayChannel(-1, killEffect, 0);
  enemies.erase(it);
  delete this;
}

void Enemy::Draw()
{
  // Apply enemy
  SDL_Rect enemyDest = {enemyOffsetX-(tileSize/2), enemyOffsetY-(tileSize/2), enemyOffsetX+(tileSize/2), enemyOffsetY+(tileSize/2)};
  SDL_BlitSurface(enemySprite, NULL, surface, &enemyDest);
   
}

void Enemy::Move()
{
  if(moveProbability(eng) < 3)
    {
      if(playerOffsetX > enemyOffsetX) {enemyOffsetX++;}
      if(playerOffsetX < enemyOffsetX) {enemyOffsetX--;}
      if(playerOffsetY > enemyOffsetY) {enemyOffsetY++;}
      if(playerOffsetY < enemyOffsetY) {enemyOffsetY--;}
    }
}
  
void spawnEnemy()
{
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

void drawArrows()
{
  vector<Arrow*>::iterator it;
  for(it=arrows.begin(); it!=arrows.end(); it++)
    {
      Arrow* arrow = *it;
      arrow->Draw();
    }
}

bool drawPlayer()
{
  // Apply player
  SDL_Rect playerDest = {playerOffsetX-(tileSize/2), playerOffsetY-(tileSize/2), playerOffsetX+(tileSize/2), playerOffsetY+(tileSize/2)};
  SDL_BlitSurface(player, NULL, surface, &playerDest);
   
}

bool init()
{
  // Init success flag
  bool success = true;

  // SDL init
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)<0)
    {
      cout << "SDL could not initialize. Error:" << SDL_GetError() << endl;
      success = false;
    }
  else
    {
      // Create window
      window = SDL_CreateWindow("The Crusade for Jimtheism", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winXMax, winYMax, SDL_WINDOW_SHOWN);
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
  
  gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  if(!gRenderer)
    {
      cout << "Failed to initialize renderer with error " << SDL_GetError();
      exit(-1);
    }

  if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)<0)
    {
      cout << "Mixer failed to initialize with error " << Mix_GetError() << endl;
      exit(-1);
    }
  
  // Load music
  TheLastStand = Mix_LoadMUS("TheLastStand.wav");
  PrimoVictoria = Mix_LoadMUS("PrimoVictoria.wav");
  Bismarck = Mix_LoadMUS("Bismarck.wav");
  GhostInTheTrenches = Mix_LoadMUS("GhostInTheTrenches.wav");
  BackInControl = Mix_LoadMUS("BackInControl.wav");
  DevilDogs = Mix_LoadMUS("DevilDogs.wav");
  AttackOfTheDeadMen = Mix_LoadMUS("AttackOfTheDeadMen.wav");
  SevenPillarsOfWisdom = Mix_LoadMUS("SevenPillarsOfWisdom.wav");
  GreatWar = Mix_LoadMUS("GreatWar.wav");
  PanzerBattalion = Mix_LoadMUS("PanzerBattalion.wav");
  Wolfpack = Mix_LoadMUS("Wolfpack.wav");
  ToHellAndBack = Mix_LoadMUS("ToHellAndBack.wav");
  
  killEffect = Mix_LoadWAV("Hurt.wav");
  if(!killEffect)
    {
      cout << "Could not load Hurt.wav with error " << Mix_GetError() << endl;
      exit(-1);
    }
  hurtSound = Mix_LoadWAV("Jav.wav");
  if(!hurtSound)
    {
      cout << "Could not load Jav.wav with error " << Mix_GetError() << endl;
      exit(-1);
    }
  deathEffect = Mix_LoadWAV("deathEffect.wav");
  
  // Load bitmaps
  hello = SDL_LoadBMP("grass.bmp");
  player = SDL_LoadBMP("player.bmp");
  heart = SDL_LoadBMP("heart.bmp");
  splashSprite = SDL_LoadBMP("splashSprite.bmp");
  deathSprite = SDL_LoadBMP("deathSprite.bmp");
  waveIndicator = SDL_LoadBMP("waveIndicator.bmp");
  if(splashSprite == NULL)
    {
      cout << "Failed to load splash sprite." << endl;
      exit(-1);
    }
  spawnInterval = spawnIntervalDist(eng);
  
  return success;
}

bool frameHandler()
{
  // Init success flag
  bool success = true;
  
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
  for(int i=0; i<enemies.size(); i++)
    {
      enemies[i]->Move();
    }
  for(int i=0; i<arrows.size(); i++)
    {
      arrows[i]->Move();
    }
  vector<Arrow*>::iterator it;
  for(it=arrows.begin(); it!=arrows.end(); it++)
    {
      Arrow* arrow = *it;
      if(arrow->exist == false)
	{
	  delete arrow;
	  arrows.erase(it--);
	}
    }
  if(playerInvincibilityFrames <= 0)
    {
      vector<Enemy*>::iterator it2;
      for(it2=enemies.begin(); it2!=enemies.end(); it2++)
	{
	  Enemy* enemy = *it2;
	  if(playerOffsetX > enemy->enemyOffsetX-tileSize && playerOffsetX < enemy->enemyOffsetX+tileSize && playerOffsetY > enemy->enemyOffsetY-tileSize && playerOffsetY < enemy->enemyOffsetY+tileSize)
	    {
	      playerHit();
	    }
	}
    }
  playerInvincibilityFrames--;

  SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_Rect sidebarRect = {sidebarX, sidebarY, sidebarWidth, sidebarHeight};
  SDL_RenderFillRect(gRenderer, &sidebarRect);
  if(playerHealth >= 1)
    {
      SDL_Rect heartDstRect = {sidebarX, sidebarY, sidebarX+tileSize, sidebarY+tileSize};
      SDL_BlitSurface(heart, NULL, surface, &heartDstRect);
      if(playerHealth >= 2)
	{
	  SDL_Rect heartDstRect2 = {heartDstRect.x+tileSize, heartDstRect.y, heartDstRect.w, heartDstRect.h};
	  SDL_BlitSurface(heart, NULL, surface, &heartDstRect2);
	  if(playerHealth >= 3)
	    {
	      SDL_Rect heartDstRect3 = {heartDstRect2.x+tileSize, heartDstRect2.y, heartDstRect2.w, heartDstRect2.h};
	      SDL_BlitSurface(heart, NULL, surface, &heartDstRect3);
	    }
	}
    }
  if(wave)
    {
      SDL_Rect waveIndicatorDest = {sidebarX+tileSize/2, sidebarY+(sidebarHeight/2-32), sidebarWidth-tileSize, 64};
      SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
      SDL_RenderFillRect(gRenderer, &waveIndicatorDest);
    }
  else
    {
      SDL_Rect waveIndicatorDest = {sidebarX+tileSize/2, sidebarY+(sidebarHeight/2-32), sidebarWidth-tileSize, 64};
      SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
      SDL_RenderFillRect(gRenderer, &waveIndicatorDest);
    }
  return success;
}

void cleanup()
{
  cout << "Your final score is " << score << endl;
  // Deallocate surfaces
  SDL_FreeSurface(hello);
  hello = NULL;

  // Delete window
  SDL_DestroyWindow(window);
  window = NULL;

  SDL_Quit();
  Mix_Quit();
}

int main(int argc, char* argv[])
{
 START:
  if(!init())
    {
      cout << "Failed to initialize." << endl;
    }
  splashScreen();
  while(gameRunning)
    {
      bool validShoot = true;
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
      if(wave)
	{
	  if(sinceSpawn > spawnInterval)
	    {
	      spawnEnemy();
	      spawnInterval = spawnIntervalDist(eng)-spawnDecrease;
	      if(spawnInterval < 0) {spawnInterval = 0;}
	      waveCount++;
	    }
	}
      if(waveCount == 20)
	{
	  waveCount = 0;
	  wave = false;
	  spawnDecrease += 750;
	}
      if(wave == false)
	{
	  SDL_Rect waveIndicatorDest = {sidebarX, sidebarY+(sidebarHeight/3), sidebarWidth, 64};
	  SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
	  SDL_RenderFillRect(gRenderer, &waveIndicatorDest);
	  waveWait++;
	}
      if(waveWait >= 1000)
	{
	  wave = true;
	  waveWait = 0;
	}
      sinceSpawn = time-lastSpawn;

      sinceShoot = time-lastShoot;
      if(sinceShoot <= shootInterval)
	{
	  validShoot = false;
	}
      // Input handler
      SDL_PumpEvents();
      const Uint8* keys = SDL_GetKeyboardState(NULL);
      wKeyPress = keys[SDL_SCANCODE_W];
      aKeyPress = keys[SDL_SCANCODE_A];
      sKeyPress = keys[SDL_SCANCODE_S];
      dKeyPress = keys[SDL_SCANCODE_D];
      upKeyPress = keys[SDL_SCANCODE_UP];
      leftKeyPress = keys[SDL_SCANCODE_LEFT];
      downKeyPress = keys[SDL_SCANCODE_DOWN];
      rightKeyPress = keys[SDL_SCANCODE_RIGHT];
      if(keys[SDL_SCANCODE_ESCAPE]) {gameRunning = false;}

      // Collision
      bool validPos = true;
      if (playerOffsetY < mapTop+(tileSize/2)+1)
	{
	  validPos = false;
	  playerOffsetY++;
	}
      if (playerOffsetX < mapLeft+(tileSize/2)+1)
	{
	  validPos = false;
	  playerOffsetX++;
	}
      if (playerOffsetY > mapBottom-(tileSize/2)-1)
	{
	  validPos = false;
	  playerOffsetY--;
	}
      if (playerOffsetX > mapRight-(tileSize/2)-1)
	{
	  validPos = false;
	  playerOffsetX--;
	}
      
      // Keypress bool handler
      if(wKeyPress && validPos) {playerOffsetY--;}
      if(aKeyPress && validPos) {playerOffsetX--;}
      if(sKeyPress && validPos) {playerOffsetY++;}
      if(dKeyPress && validPos) {playerOffsetX++;}
      if(upKeyPress && validShoot) {spawnArrow(0);}
      if(leftKeyPress && validShoot) {spawnArrow(1);}
      if(downKeyPress && validShoot) {spawnArrow(2);}
      if(rightKeyPress && validShoot) {spawnArrow(3);}
      frameHandler();
      musicHandler();
      drawPlayer();
      drawEnemies();
      drawArrows();
      SDL_UpdateWindowSurface(window);
      SDL_blit(surface);

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
