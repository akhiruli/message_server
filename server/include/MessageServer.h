#ifndef _UDP_SERVER_H
#define _UDP_SERVER_H
#include <EventImpl.h>
//#include <ioctl.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include<functional>
#include <MessageService.h>
#include <ifc.h>
#include<thread>
#include<ConcurrentQueue.h>
#include <FileWriteWorker.h>

typedef unsigned char  Byte;

class MessageServer {
    public:
        MessageServer();
        MessageServer(const MessageServer&) = delete;
        MessageServer& operator=(const MessageServer& ) = delete;
        ~MessageServer();

        void run();
        int prepareTcpSvr();
        int setnonblock(int fd);
        void spawnWorkers();
        void loadbalance_fd(int fd);
        static void on_accept(int fd, short ev, void *arg);
    private:
        int             m_index;
        struct event    m_evread;
        std::vector<std::pair<std::thread*, ConcurrentQueue<int> *>>     m_threadpool;
        std::pair<std::thread*, ConcurrentQueue<Payload> *>     m_fileWriterthread;
}; 
#endif
