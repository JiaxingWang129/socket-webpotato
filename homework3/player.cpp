#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>

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
  recv(socket_fd,buffer,10,0);
  //get the id number
  int idnum=atoi(buffer);
  //std::cout<<idnum<<std::endl;
  //std::cout<<"player "<<atoi(buffer)<<" is ready to play"<<std::endl;
  string message="player "+std::to_string(idnum)+" is ready to play";
  int length=message.length();
  send(socket_fd,message.c_str(),length,0);
  //std::cout<<message<<std::endl;
}
