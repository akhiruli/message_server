#ifndef _MSG_SVC_H_
#define _MSG_SVC_H_

#include <stdio.h>                                                              
#include <stdlib.h>                                                             
#include <string.h> 
#include <functional>
#include <ConcurrentQueue.h>
#include <EventImpl.h>
#include <thread>
#include <ifc.h>
#include <list>
#define NUM_READ_TRY 3

typedef unsigned char  Byte;
class MessageService{
    public:
        MessageService();
        virtual ~MessageService();
        void entrypoint(ConcurrentQueue<int>&);
        void processEvent(int fd);
        //static void on_read(int fd, short ev, void *arg);
        void setFileWriterQueue(ConcurrentQueue<Payload *> *queue){
            this->m_file_writer_queue = queue;
        }
    private:
        ConcurrentQueue<Payload*>   *m_file_writer_queue;
};

class Client{
    public:
        Client(ConcurrentQueue<Payload*>   *);
        ~Client();
        Client(const Client& ) = delete;
        Client& operator=(const Client& ) = delete;
        void parseMessage();
        void sendAck();
        static void on_read(int fd, short ev, void *arg);
        void setFd(int fd){ m_fd = fd;}
    public:
        struct event  m_evread;

    private:
        struct TempMessage{
            Byte   *buffer;
            int    len;
            TempMessage(){
                buffer = NULL;
                len = 0;
            }
            ~TempMessage(){
                //if(buffer)
                  //  free(buffer);
                len = 0;
            }
        };
        ConcurrentQueue<Payload*>   *m_file_writer_queue;
        std::list<TempMessage>     m_msg_list;
        int     m_fd;
        bool    m_firstByte;
        Byte*   m_currbuffer;
        int     m_currbufflen;
};
#endif
