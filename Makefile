all:	netprotolib.o
	gcc -shared -fPIC -olibnetproto.so netprotolib.o
netprotolib.o:	netprotolib.c netprotolib.h
	gcc -c -fPIC -onetprotolib.o netprotolib.c -lpthread
install:	
	cp ./libnetproto.so /usr/lib/libnetproto.so
	cp ./netprotolib.h /usr/include/netprotolib.h
	
