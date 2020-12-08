all: serverA.cpp serverB.cpp servermain.cpp client.cpp
	g++ -o serverA serverA.cpp -std=c++14
	g++ -o serverB serverB.cpp -std=c++14
	g++ -o servermain servermain.cpp -std=c++14
	g++ -o client client.cpp -std=c++14
	
	
.PHONY: serverA serverB mainserver


serverA: serverA
	./serverA

serverB: serverB
	./serverB

mainserver: servermain
	./servermain

client1: client
	./client


clean: 
	$(RM) serverA
	$(RM) serverB
	$(RM) servermain
	$(RM) clien
	$(RM) serverA.o
	$(RM) serverB.o
	$(RM) servermain.o
	$(RM) client.o
	
	