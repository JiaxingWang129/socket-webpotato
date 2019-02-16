#include <vector>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
class Potato{
	private:
		int hops;
		int playernum;
	public:
		int traceid[512]={0};
		//if we use char x[] we could transfer the structure together
		Potato();
		Potato(int a, int b):hops(a),playernum(b){}
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
		/*
		void printtrace(int playernum,char &trace[512]){
			for(int j=0;j<playernum;j++){
                std::cout<<"<"<<trace[j]<<"> ,";
               }
		}
		*/
};
