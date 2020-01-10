
GPP = g++

CFLAGS+= -Wfatal-errors -g -Wall -Wformat-nonliteral -Wformat-security -I ./include

CPPFLAGS+= -g -std=c++11 -std=gnu++14 -Wfatal-errors -O1

LDFLAGS +=   -Wl,-rpath-link -lstdc++ -levent -std=c++11 -std=gnu++14 -lpthread

msgserver_sources = \
		src/EventImpl.cpp	\
        src/MessageServer.cpp   \
        src/MessageService.cpp  \
        src/MessageServerTest.cpp   \
        src/FileWriteWorker.cpp

APPS = msgserver 

all: $(APPS)

msgserver: ${msgserver_sources}
	$(GPP) $(CFLAGS) -fPIC $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(GPP) $(CFLAGS)  -fPIC -c $< -o $@	

clean:
	rm -f $(APPS)
	rm -f *.o
