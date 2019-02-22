#include <vector>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
class Potato{
	private:
		int hops;
		int playernum;
		bool sign;
	public:
		int trace[513]={8};
		//if we use char x[] we could transfer the structure together
		Potato();
		Potato(int a, int b,bool c):hops(a),playernum(b),sign(c){}
		void sethop(int num){
			hops=num;
		}
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
		bool getsign(){
			return sign;
		}
		void setsign(bool num){
			sign=num;
		}
};
