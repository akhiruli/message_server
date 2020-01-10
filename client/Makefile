
GPP = g++

CFLAGS+= -Wfatal-errors -g -Wall -Wformat-nonliteral -Wformat-security -I ./include

CPPFLAGS+= -g -std=c++11 -std=gnu++14 -Wfatal-errors -O1

LDFLAGS +=   -Wl,-rpath-link -lstdc++ -levent -std=c++11 -std=gnu++14 -lpthread

msgclient_sources = \
    src/Client.cpp  \
    src/TestClient.cpp

APPS = msgclient

all: $(APPS)

msgclient: ${msgclient_sources}
	$(GPP) $(CFLAGS) -fPIC $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(GPP) $(CFLAGS)  -fPIC -c $< -o $@	

clean:
	rm -f $(APPS)
	rm -f *.o
