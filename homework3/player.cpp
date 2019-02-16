#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>
#include "potato.h"
#include <algorithm>
using namespace std;

int main(int argc, char *argv[])
{
	int status;
	int socket_fd;
	struct addrinfo host_info;
	struct addrinfo *host_info_list;
	if(argc<3){
		std::cout<<"input is wrong\n"<<std::endl;
		return 1;
	}
	const char *hostname =argv[1];
	//each player has its own port number to connect with the neighbor, and give it to the server in the first time 
	const char *port=argv[2];
    
	memset(&host_info,0,sizeof(host_info));
	host_info.ai_family=AF_UNSPEC;
	host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if
  
  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if
  //receving from the server its id number
  char buffer[512];
  recv(socket_fd,buffer,sizeof(buffer),0);
  //get the id number
  int idnum=atoi(buffer);

  //receving the total number of player---sizeof problem
  recv(socket_fd,buffer,sizeof(buffer),0);
  int total_player=atoi(buffer);
  //std::cout<<"the total player is "<<total_player<< std::endl;
  
  //return the ready message to the ringmaster
  string message="player "+std::to_string(idnum)+" is ready to play";
  int length=message.length();
  send(socket_fd,message.c_str(),length,0);

  //std::cout<<total_player<<std::endl;

//this is the port number itself open for its left neighbor to connecting on 
  /*---------------------------------------being as server-------------------------------------*/
 int connect_port;
    
    connect_port=(idnum)*2+35000;

  std::string transfer=to_string(connect_port);
  const char * portserver=transfer.c_str();
/* -----------------   being server for the left player-----------------------------------------------------------*/
    int l_status;
    int l_socket_fd;
    struct addrinfo left_info;
    struct addrinfo *left_info_list;
    memset(&left_info ,0, sizeof(left_info));

    left_info.ai_family   = AF_UNSPEC;
    left_info.ai_socktype = SOCK_STREAM;
    left_info.ai_flags    = AI_PASSIVE;
 
    l_status = getaddrinfo(hostname, portserver, &left_info, &left_info_list);
    if (l_status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << portserver << ")" << endl;
    return -1;
  } //if

   l_socket_fd = socket(left_info_list->ai_family, 
         left_info_list->ai_socktype, 
         left_info_list->ai_protocol);
   if (l_socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << portserver << ")" << endl;
    return -1;
  } //if

  int yes = 1;
  l_status = setsockopt(l_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  l_status = bind(l_socket_fd, left_info_list->ai_addr, left_info_list->ai_addrlen);
  if (l_status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    cerr << "  (" << hostname << "," << portserver << ")" << endl;
    return -1;
  } //if

  status = listen(l_socket_fd, 100);
  if (l_status == -1) {
    cerr << "Error: cannot listen on socket" << endl; 
    cerr << "  (" << hostname << "," << portserver << ")" << endl;
    return -1;
  } //if
  
  /* receive from the ringmaster again to make sure all the port has open */
  recv(socket_fd,buffer,sizeof(buffer),0);
  int total_playernew=atoi(buffer);
  std::cout<<"connected as player <" << idnum<< " > out of <" <<total_playernew<< " > total player" << std::endl;
 
//---------------just for testing need to be abstracted with listen to three port---------------//
  /*
  testing for receiving the potato (should only one player receive the first potato)
  Potato start(0,0);
  //receive the random first potato player
  recv(socket_fd,buffer,sizeof(buffer),0);
  //std::cout<<"the random buffer is"<<atoi(buffer)<<std::endl;
  if(idnum==atoi(buffer)){
  recv(socket_fd,&start,sizeof(&start),0);
  std::cout<<"my id number is"<<idnum<<"and I receive the first potato"<<std::endl;
  start.sethop(start.gethopnum()-1);
  }
  */
  //testing for send back to the ringmaster the end of the potato (why the trace would be random number????)
  

/*-------------------being client for the right server ---------------------*/
   //the right neighbor's port is (idnum+1)*2+35000 
  int right_port;
  if(idnum!=total_player-1){
   right_port=(idnum+1)*2+35000;
  }
  else{
    right_port=35000;
  }
  std::string transfer1=to_string(right_port);
  const char * rightport=transfer1.c_str();
/*-------------- being as client--------------------------------------------*/
  int r_status;
  int r_socket_fd;
  struct addrinfo right_info;
  struct addrinfo *right_info_list;

  memset(&right_info,0,sizeof(right_info));
  right_info.ai_family=AF_UNSPEC;
  right_info.ai_socktype = SOCK_STREAM;

  r_status = getaddrinfo(hostname, rightport, &right_info, &right_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << rightport << ")" << endl;
    return -1;
  } //if

  r_socket_fd = socket(right_info_list->ai_family, 
         right_info_list->ai_socktype, 
         right_info_list->ai_protocol);
  if (r_socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << rightport << ")" << endl;
    return -1;
  } //if
  
  //connect one to one, if the idnum is 0. it connect first then accept first, other id num accpet first then connect

 if(idnum!=0){
//-----------------accpet----------------//
  cout << "Waiting for connection on port " << portserver << endl;
  struct sockaddr_storage l_socket_addr;
  socklen_t l_socket_addr_len = sizeof(l_socket_addr);
  std::cout<<"I am the server and my port number is " << portserver<<std::endl; 
  
  int l_connection_fd; 
  l_connection_fd = accept(l_socket_fd, (struct sockaddr *)&l_socket_addr, &l_socket_addr_len);
  if (l_connection_fd == -1) {
    cerr << "Error: cannot accept connection on socket" << endl;
    return -1;
  } //if
  char transfernew[1000];
  recv(l_connection_fd,transfernew,sizeof(transfernew),0);
  std::cout<<"player"<<transfernew<<" has sending me message"<<std::endl;

}
// Then connect
  cout << "Connecting to " << hostname << " on port " << rightport << "..." << endl;
  
  r_status = connect(r_socket_fd, right_info_list->ai_addr, right_info_list->ai_addrlen);
  if (r_status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << rightport << ")" << endl;
    return -1;
  } //if
  std::cout<<"I am connecting with my right server and my right server port number is " << right_port<<std::endl; 
  /* testing */
  char clientbuffer[1000];
  string clientmessage=std::to_string(idnum);
  int clientlength=clientmessage.length();
  send(r_socket_fd,clientmessage.c_str(),clientlength,0);
  std::cout<<"I am the client"<< idnum <<" i have send the message to"<< idnum+1 <<std::endl;

int l_connection_fd;
if(idnum==0){
//  accpet 
  cout << "Waiting for connection on port " << portserver << endl;
  struct sockaddr_storage l_socket_addr;
  socklen_t l_socket_addr_len = sizeof(l_socket_addr);
  std::cout<<"I am the server and my port number is " << portserver<<std::endl; 
  
  l_connection_fd = accept(l_socket_fd, (struct sockaddr *)&l_socket_addr, &l_socket_addr_len);
  if (l_connection_fd == -1) {
    cerr << "Error: cannot accept connection on socket" << endl;
    return -1;
  } //if
  char transfernew[1000];
  recv(l_connection_fd,transfernew,sizeof(transfernew),0);
  std::cout<<"player"<<transfernew<<" has sending me message"<<std::endl;
 }

 //the frist receving player would send the potato all the player need to listen to two channel
   fd_set master;
   int fdmax;
 while(1){
    FD_ZERO(&master);
    //with ringmaster
    FD_SET(socket_fd, &master);
    //accept left
    FD_SET(l_connection_fd, &master);
    //connect right server
    FD_SET(r_socket_fd, &master);
    fdmax = max(socket_fd, l_connection_fd);
    fdmax = max(fdmax, r_socket_fd);
    if(select(fdmax + 1, &master, NULL, NULL, NULL)==-1){
      std::cout<<"select wrong"<<std::endl;
      return 0;
    }
    Potato start(0,0);
    //see which channel get the message
    if(FD_ISSET(socket_fd, &master)){
      std::cout<<"I have received potato from the ringmaster"<<std::endl;
       recv(socket_fd,&start,sizeof(&start),0);
       int hop=start.gethopnum()-1;
       start.sethop(hop);
       if(start.gethopnum()==0){
        send(socket_fd,&start,sizeof(&start),0);
        break;
       } 
    }
    if(FD_ISSET(r_socket_fd, &master)){
      
    }
    if(FD_ISSET(l_connection_fd, &master)){
      
    }
  }
}
