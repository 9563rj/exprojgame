#include <iostream>
#include <random>
#include <fstream>

int main()
{
  // seed rng
  std::random_device rd;
  std::mt19937 eng(rd());
  std::uniform_int_distribution<> tileDist(0,2);

  // filestream init
  std::ofstream worldFile;
  worldFile.open("world.txt");
  
  for(int i=0; i<551;i++)
    {
      switch(tileDist(eng))
	{
	case 0 :
	  worldFile << "G";
	  break;
	  
	case 1 :
	  worldFile << "W";
	  break;
	  
	case 2:
	  worldFile << "R";
	  break;
	  
	default:
	  std::cout << "Invalid number." << std::endl;
	  break;
	}
    }
}
