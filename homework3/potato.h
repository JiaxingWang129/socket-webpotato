#include <vector>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
class Potato{
	private:
		int hops;
		int playernum;
		std::vector<int> traceid;
	public:
		Potato();
		Potato(int a, int b):hops(a),playernum(b){}
		int gethopnum(){
		  return hops;
		}
		int getplayernum(){
		  return playernum;
		}
		int getrand(int playernum){
			srand(time(NULL));
			int random=rand()%playernum;
			return random;
		}
};
