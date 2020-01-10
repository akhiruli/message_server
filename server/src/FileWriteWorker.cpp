#include <FileWriteWorker.h>

FileWriteWorker::FileWriteWorker(){

}

FileWriteWorker::~FileWriteWorker(){

}

void FileWriteWorker::entrypoint(ConcurrentQueue<Payload> &queue){
    printf("FileWriteWorker::entrypoint: thread_id %d\n", std::this_thread::get_id());

    while(1){
        auto msg = queue.pop();
        processMsg(msg);
    }
}

void FileWriteWorker::processMsg(Payload& msg){
    printf("FileWriteWorker::processMsg: nessage %s", msg.buffer);
}
