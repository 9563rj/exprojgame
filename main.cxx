#include "main.h"
using namespace std;

SDL_Window* window = NULL;
SDL_Surface* surface = NULL;
SDL_Surface* hello = NULL;
int tileSize = 16;
int mapX = winXMax/tileSize/2-1;
int mapXPixels = mapX*tileSize;
int mapY = winYMax/tileSize-1;
int mapYPixels = mapY*tileSize;
int mapXZero = winXMax/2+tileSize/2;
int mapYZero = (winYMax-mapYPixels)/2;

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
  return success;
}

bool loadMedia()
{
  // Init success flag
  bool success = true;

  // Load splash screen
  hello = SDL_LoadBMP("grass.bmp");
  // Debug
  if(hello == NULL)
    {
      cout << "Couldn't load image. Error:" << SDL_GetError() << endl;
      success = false;
    }
  else
    {
      // Apply splash screen and update
      for(int i=0;i<mapX;i++)
	{
	  for(int j=0;j<mapY;j++)
	    {
	      SDL_Rect dstrect = {i*tileSize+mapXZero, j*tileSize+mapYZero, (i+1)*tileSize+mapXZero, (j+1)*tileSize+mapYZero};
	      SDL_BlitSurface(hello, NULL, surface, &dstrect);
	    }
	}
      SDL_UpdateWindowSurface(window);
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
      if(!loadMedia())
	{
	  cout << "Failed to load image." << endl;
	}
    }
  SDL_Delay(10000);
  cleanup();
  return 0;
}
