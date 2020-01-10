#ifndef _F_WRITER_H_
#define _F_WRITER_H_

#include <thread>
#include<ConcurrentQueue.h>
#include <ifc.h>

class FileWriteWorker{
    public:
        FileWriteWorker();
        ~FileWriteWorker();
        void entrypoint(ConcurrentQueue<Payload> &queue);
        void processMsg(Payload& msg);
    private:
};

#endif
