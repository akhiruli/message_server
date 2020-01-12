#ifndef _F_WRITER_H_
#define _F_WRITER_H_

#include <thread>
#include <ConcurrentQueue.h>
#include <ifc.h>
#include <string>
#include <fstream>
#include <arpa/inet.h> 

class FileWriteWorker{
    public:
        FileWriteWorker();
        ~FileWriteWorker();
        void entrypoint(ConcurrentQueue<Payload*> &queue);
        void processMsg(Payload *msg);
    private:
        std::fstream    m_ofs;
        std::string     m_filename;
        std::string     m_bufferMsg;
        uint16_t        m_numbufferedmsg;
        uint16_t        m_maxbufmsg; 
};

#endif
