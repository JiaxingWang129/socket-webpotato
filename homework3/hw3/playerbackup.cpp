#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>
#include "potato.h"
#include <algorithm>
#include <string>
#include <stdlib.h>
#include <stdio.h>
using namespace std;


/*------------------------------------give potato to neighbor function-----------------------------------------------*/
int give(int idnum,int total){
   int next=rand()%2;
   int left;
   int right;
   //int len=str.length();
   if(idnum==0){
      left=total-1;
      right=idnum+1;
   }
   else if(idnum==total-1){
      left=idnum-1;
      right=0;
   }
   else{
      left=idnum-1;
      right=idnum+1;
   }
   //std::cout<<"the next is"<<next<<std::endl;
   //give potato to left neighbor
   if(next==0){
    std::cout<<"sending potato to <"<< left <<" >"<<std::endl;
   }
   //give potato to right neighbor
   if(next==1){
    std::cout<<"sending potato to <"<< right <<" >"<<std::endl;
   }
   return next;
}


int main(int argc, char *argv[])
{
	int status;
	int socket_fd;
  int r_socket_fd;
  int l_connection_fd;
  int l_socket_fd;
	struct addrinfo host_info;
	struct addrinfo *host_info_list;
  int r_status;
  struct addrinfo right_info;
  struct addrinfo *right_info_list;
  int l_status;
  struct addrinfo left_info;
  struct addrinfo *left_info_list;
  char buffer[512];
	if(argc<3){
		std::cout<<"input is wrong\n"<<std::endl;
		return 1;
	}
	const char *hostname =argv[1];
	//each player has its own port number to connect with the neighbor
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
  
  //cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if
  //receving from the server its id number
  
  recv(socket_fd,buffer,sizeof(buffer),0);
  //get the id number
  int idnum=atoi(buffer);

  //receving the total hopnum---sizeof problem
  recv(socket_fd,buffer,sizeof(buffer),0);
  int total_hop=atoi(buffer);
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
  if(idnum!=total_playernew-1){
   right_port=(idnum+1)*2+35000;
  }
  else{
    right_port=35000;
  }
  std::string transfer1=to_string(right_port);
  const char * rightport=transfer1.c_str();
/*-------------- being as client--------------------------------------------*/
 

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
  //cout << "Waiting for connection on port " << portserver << endl;
  struct sockaddr_storage l_socket_addr;
  socklen_t l_socket_addr_len = sizeof(l_socket_addr);
  //std::cout<<"I am the server and my port number is " << portserver<<std::endl; 
  
 
  l_connection_fd = accept(l_socket_fd, (struct sockaddr *)&l_socket_addr, &l_socket_addr_len);
  if (l_connection_fd == -1) {
    cerr << "Error: cannot accept connection on socket" << endl;
    return -1;
  } //if
 //char transfernew[1000];
 //recv(l_connection_fd,transfernew,sizeof(transfernew),0);
 //std::cout<<"player"<<transfernew<<" has sending me message"<<std::endl;

}
// Then connect
  //cout << "Connecting to " << hostname << " on port " << rightport << "..." << endl;
  
  r_status = connect(r_socket_fd, right_info_list->ai_addr, right_info_list->ai_addrlen);
  if (r_status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << rightport << ")" << endl;
    return -1;
  } //if
 // std::cout<<"I am connecting with my right server and my right server port number is " << right_port<<std::endl; 
  /* testing for transfer from one client to another client */
 // char clientbuffer[1000];
 // string clientmessage="this is the message to test"+std::to_string(idnum);
 // int clientlength=clientmessage.length();
 // send(r_socket_fd,clientmessage.c_str(),clientlength,0);
 // std::cout<<"I am the client"<< idnum <<" i have send the message to"<< idnum+1 <<std::endl;

if(idnum==0){
//  accpet 
  //cout << "Waiting for connection on port " << portserver << endl;
  struct sockaddr_storage l_socket_addr;
  socklen_t l_socket_addr_len = sizeof(l_socket_addr);
  //std::cout<<"I am the server and my port number is " << portserver<<std::endl; 
  
  l_connection_fd = accept(l_socket_fd, (struct sockaddr *)&l_socket_addr, &l_socket_addr_len);
  if (l_connection_fd == -1) {
    cerr << "Error: cannot accept connection on socket" << endl;
    return -1;
  } //if
  //testing for clients connect with each other
  //char transfernew[1000];
  //recv(l_connection_fd,transfernew,sizeof(transfernew),0);
  //std::cout<<"player"<<transfernew<<" has sending me message"<<std::endl;
 }

 //the frist receving player would send the potato all the player need to listen to three channel
    fd_set master;
    Potato start(0,0,false);
    int fdmax=0;
 while(1){
    FD_ZERO(&master);
    //with ringmaster
    FD_SET(socket_fd, &master);
    //accept left
    FD_SET(l_connection_fd, &master);
    //connect right server
    FD_SET(r_socket_fd, &master);
    fdmax=max(socket_fd,l_connection_fd);
    //std::cout<<fdmax<<std::endl;
    fdmax=max(fdmax,r_socket_fd);
    //std::cout<<fdmax<<std::endl;
    //std::cout<<"execute before select here"<<std::endl;
    // some times other player can only execute here????
    int ret = select(fdmax + 1, &master, NULL, NULL, NULL);
    //std::cout<<"after the select is"<<ret<<std::endl;
    if(ret < 0){
      cerr << "select is wrong" << endl;
      return -1;
    }
    //see which channel get the message
    //message from the ringmaster (end or the first time)
    if(FD_ISSET(socket_fd, &master)){
        //receiving from the ringmaster the potato
        recv(socket_fd, &start, sizeof(start), 0);
        //first time receive from the ringmaster
        if(start.getsign()==false){
         //set hopnum
        start.sethop(start.gethopnum()-1);
        //std::cout<<"now the hopnumber is"<<start.gethopnum()<<std::endl;
        //set trace
        int index=total_hop-start.gethopnum()-1;
        //std::cout<<"the trace index is "<<index<<std::endl;
        start.trace[index]=idnum;
       // std::cout<<"the trace is"<<start.trace[index]<<std::endl;
       // std::cout<<"receiving from the ringmaster and the sign is"<<start.getsign()<<std::endl;
        if(start.gethopnum()==0){
          //sendback to ringmaster to change the sign to true 
            std::cout<<"I am it"<<std::endl; 
            send(socket_fd,&start,sizeof(start),0);
            //std::cout<<"I have send back the potato to ringmaster"<<std::endl;
        }
        else{
           int check=give(idnum,total_playernew);
          //give potato to left neighbor
          if(check==0){
          send(l_connection_fd,&start,sizeof(start),0);
           }
           //give potato to right neighbor
         else if(check==1){
           send(r_socket_fd,&start,sizeof(start),0);
           }
        }
       }
       //receive from the ringmaster to tell end
       else{
          //std::cout<<idnum<<"receive from the ringmaster to end"<<std::endl;
          break;
       }
     }
   //receive message from the right neighbor

    if(FD_ISSET(r_socket_fd, &master)){
      if(recv(r_socket_fd, &start, sizeof(start), 0)==0){
          break;
      }
        int num=start.gethopnum()-1;
        start.sethop(num);
       // std::cout<<"right the hopnumber is"<<start.gethopnum()<<std::endl;
         //set trace
        int index=total_hop-start.gethopnum()-1;
        //std::cout<<"the trace index is "<<index<<std::endl;
        start.trace[index]=idnum;
        //std::cout<<"the trace is"<<start.trace[index]<<std::endl;
        if(start.gethopnum()==0){
          std::cout<<"I am it"<<std::endl;
          //send back the trace string
          send(socket_fd,&start,sizeof(start),0);
         // std::cout<<"I have send back the potato to ringmaster"<<std::endl;
          close(l_connection_fd);
          close(r_socket_fd);
          break;
        }
        else if(start.gethopnum()<0){
          close(l_connection_fd);
          close(r_socket_fd);
          break;
        }
        else{
          int check=give(idnum,total_playernew);
          //give potato to left neighbor
           if(check==0){
            send(l_connection_fd,&start,sizeof(start),0);
            }
          //give potato to right neighbor
            else if(check==1){
            send(r_socket_fd,&start,sizeof(start),0);
            }
          }
        }

    if(FD_ISSET(l_connection_fd, &master)){
      if(recv(l_connection_fd, &start, sizeof(start), 0)==0){
        break;
      }
      int num=start.gethopnum()-1;
      start.sethop(num);
      //std::cout<<"left now the hopnumber is"<<start.gethopnum()<<std::endl;
      //set trace
      int index=total_hop-start.gethopnum()-1;
     // std::cout<<"the trace index is "<<index<<std::endl;
      start.trace[index]=idnum;
      //std::cout<<"the trace is"<<start.trace[index]<<std::endl;  
      
      if(start.gethopnum()==0){
          std::cout<<"I am it"<<std::endl;
          //send back the trace string
          send(socket_fd,&start,sizeof(start),0);
          //std::cout<<"I have send back the potato to ringmaster"<<std::endl;
          close(l_connection_fd);
          close(r_socket_fd);
          break;
      }
      else if(start.gethopnum()<0){
        close(l_connection_fd);
        close(r_socket_fd);
        break;
      }
      else{
         //std::cout<<idnum<<"execute here to give"<<std::endl;
         int check=give(idnum,total_playernew);
        //give potato to left neighbor
        if(check==0){
          send(l_connection_fd,&start,sizeof(start),0);
        }
        //give potato to right neighbor
        else if(check==1){
          send(r_socket_fd,&start,sizeof(start),0);
        }     
      }     
    }
  }
  //close 
  freeaddrinfo(host_info_list);
  freeaddrinfo(left_info_list);
  freeaddrinfo(right_info_list);
  close(socket_fd);
  close(l_connection_fd);
  close(r_socket_fd);
  return 0;

}
