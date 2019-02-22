#include "potato.h"
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include<string>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

int main(int argc, char *argv[]){
	/*initialize*/
  Potato Ringmaster(atoi(argv[3]),atoi(argv[2]),false);
  Potato result(0,0,false);
  int totalhop=Ringmaster.gethopnum();
  if(totalhop==0){
    std::cout<<"hops cannot be zero"<<std::endl;
    exit(EXIT_FAILURE);
  }
    std::cout<<"Potato Ringmaster"<<std::endl;
	std::cout<<"Players = <"<<Ringmaster.getplayernum()<<">"<<std::endl;
	std::cout<<"Hops = <"<<totalhop<<">"<<std::endl;
	//std::cout<<"Portnum = <"<<portnum<<">"<<std::endl;
	//int rnd=Ringmaster.getrand(3);
	/* conneting with each client  */
	int status;
	int socket_fd;
	struct addrinfo host_info;
	struct addrinfo *host_info_list;
	const char *hostname=NULL;
	//port number to connect with clients
	const char * portnum=argv[1];
	
	//for record the port number of each client, we use 35000+id*2 for each clinet listen port
	std::string *p_host=new std::string[Ringmaster.getplayernum()];
	std::vector<int> port_client(Ringmaster.getplayernum(),0);
	for (int z=0;z<Ringmaster.getplayernum();z++){
		port_client[z]=35000+z*2;
	}

	
	/* using socket to connect citing from the example */
	memset(&host_info ,0, sizeof(host_info));

	host_info.ai_family   = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags    = AI_PASSIVE;
 
    status = getaddrinfo(hostname, portnum, &host_info, &host_info_list);
    if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << portnum << ")" << endl;
    return -1;
  } //if

   socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
   if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << portnum << ")" << endl;
    return -1;
  } //if

  int yes = 1;
  status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    cerr << "  (" << hostname << "," << portnum << ")" << endl;
    return -1;
  } //if

  status = listen(socket_fd, 100);
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl; 
    cerr << "  (" << hostname << "," << portnum << ")" << endl;
    return -1;
  } //if

  //cout << "Waiting for connection on port " << portnum << endl;
  struct sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  
  //creating connection with each player 
  vector<int> player_fd(Ringmaster.getplayernum(),0);
  int i=0;
  while(i<Ringmaster.getplayernum()){
  player_fd[i] = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
  if (player_fd[i] == -1) {
    cerr << "Error: cannot accept connection on socket" << endl;
    return -1;
  } //if
  //ready for send id to each player
 // const int * message = &i;
 // std::cout<<*message<<std::endl;
  // try to print the client ip 
char hoststr[NI_MAXHOST];
char portstr[NI_MAXSERV];

int rc = getnameinfo((struct sockaddr *)&socket_addr, 
    socket_addr_len, hoststr, sizeof(hoststr), portstr, sizeof(portstr), 
    NI_NUMERICHOST | NI_NUMERICSERV);

if (rc == 0) {
    std::cout<<"player"<<i<<"ip is"<<hoststr<<"port number is"<<portstr<<std::endl;
  }
 


  char buffer[512];
  sprintf(buffer, "%d", i);
  send(player_fd[i],buffer,sizeof(buffer),0);
  int totalnum=Ringmaster.gethopnum();
  sprintf(buffer,"%d",totalnum);
  send(player_fd[i],buffer,sizeof(buffer),0);
  //get the ready information from each player
  recv(player_fd[i],buffer,sizeof(buffer),0);
  std::cout<<buffer<<std::endl;  
  i++;
  }

  //send the player's total number to make sure that they all open their port
  int z=Ringmaster.getplayernum()-1;
  while(z>=0){
  //send the total player number to each player
  int totalnew=Ringmaster.getplayernum();
  char buffer[512];
  sprintf(buffer,"%d",totalnew);
  send(player_fd[z],buffer,sizeof(buffer),0);
  //std::cout<<"I have send the total number player to "<<z<<std::endl;
  z--;
  }
  
  //sending the potato and using select to listen to all the player
  int rnd=Ringmaster.getrand(Ringmaster.getplayernum());
  /*
  int k=Ringmaster.getplayernum()-1;
  //to see who receive the first potato
  while(k>=0){
  //send the randome first chosen potato number
  char buffer[512];
  sprintf(buffer,"%d",rnd);
  send(player_fd[k],buffer,sizeof(buffer),0);
  //std::cout<<"I have send the total number player to "<<z<<std::endl;
  k--;
  }
  */
  //std::cout<<Ringmaster.trace<<std::endl;
  send(player_fd[rnd],&Ringmaster,sizeof(&Ringmaster),0);
  std::cout<<"Ready to start the game, sending potato to player < "<< rnd << " >" <<std::endl;
  ///for selecting when the potato would come back 
  fd_set readfds;
  FD_ZERO(&readfds);
  char buffer[512];//for trace string
  while(1){
  int maxfd=player_fd[0];
  //put all the player into set
  for(int j=0;j<Ringmaster.getplayernum();j++){
    FD_SET(player_fd[j],&readfds);
    //choose the max;
    if(player_fd[j]>maxfd){
      maxfd=player_fd[j];
    }
  }
  //select
  if(select(maxfd+1,&readfds,NULL,NULL,NULL)==-1){
    std::cout<<"select error"<<std::endl;
  }
  //run through the existing connections looking for the last time potato to read
 
  int j;
  for(j=0;j<Ringmaster.getplayernum();j++){
    //we got a message from the player
  if(FD_ISSET(player_fd[j],&readfds)){
    if(recv(player_fd[j],&result,sizeof(result),0)==0){
      break;
    }
    //std::cout<<result.gethopnum()<<std::endl;
    std::cout<<"I have receive the potato back from the player"<<j<<std::endl;
    break;
    }
   }
  //get the potato and judge whether the hop number is zero close all
  if(result.gethopnum()<=0){
      result.setsign(true);
      for(int i = 0; i < Ringmaster.getplayernum(); i++){
      send(player_fd[i],&result,sizeof(result), 0);
      close(player_fd[i]);
    }
  }
    break;
  }
  //print the trace string buffer
  std::cout<<"Trace of Potato:"<<std::endl;
   //receive the trace
  for (int i=0;i<totalhop-1;i++){
    std::cout<<"< ";
    std::cout<<result.trace[i];
    std::cout<<" >,";
  }
  std::cout<<"< "<<result.trace[i]<<" >"<<std::endl;
  //std::cout<<"< "<<result.trace[in]<<" >"<<std::endl;
  freeaddrinfo(host_info_list);
  close(socket_fd);
}





