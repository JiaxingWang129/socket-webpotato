all:ringmaster player
CFLAGS= -ggdb3
ringmaster: ringmaster.cpp
	g++ $(CFLAGS) -o ringmaster ringmaster.cpp
player:	player.cpp
	g++ $(CFLAGS) -o player player.cpp

.PHONY:
clean:
	rm -f *.o *~ player ringmaster
