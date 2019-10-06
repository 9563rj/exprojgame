#include "main.h"
using namespace std;

int main(int argc, char* argv[])
{
  SDL_Window* window = NULL;
  SDL_Surface* surface = NULL;

  if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      // Debug for errors
      cout << "SDL failed to initialize. Error:" << SDL_GetError() << endl;
    }
  else
    {
      // Create window
      window = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winXMax, winYMax, SDL_WINDOW_SHOWN);
      
      // Debug for errors
      if(window == NULL)
	{
	  cout << "Window failed to create. Error:" << SDL_GetError();
	}
      else
	{
	  // Fill window with white and show
	  surface = SDL_GetWindowSurface(window);
	  SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
	  SDL_UpdateWindowSurface(window);
	  
	  // Wait ten seconds and then close and cleanup
	  SDL_Delay(10000);
	  SDL_DestroyWindow(window);
	  SDL_Quit();
	  return 0;
	}
    }
}
