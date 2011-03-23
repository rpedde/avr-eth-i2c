CFLAGS = -I/opt/local/include -I.. -O0 -g -DDEBUG=1
LDFLAGS = -L/opt/local/lib -lpcap -ldnet

OBJECTS = main.o compat.o ../rnet.o ../dhcp.o ../udp.o ../eth.o enc-compat.o pq.o ../arp.o ../icmp.o

all: eth-test

eth-test: $(OBJECTS)
	  $(CC) $(LDFLAGS) $(OBJECTS) -o eth-test

clean:
	rm -f *.o 
	rm -f ../*.o 
	rm -f eth-test