#include <FileWriteWorker.h>

FileWriteWorker::FileWriteWorker(){
    m_filename = "message_info.txt";

    //m_ofs.open(m_filename.c_str(), std::ios::out | std::ios::app);
}

FileWriteWorker::~FileWriteWorker(){
    if(m_ofs.is_open()){
        m_ofs.close();
    }
}

void FileWriteWorker::entrypoint(ConcurrentQueue<Payload> &queue){
    printf("FileWriteWorker::entrypoint: thread_id %d\n", std::this_thread::get_id());

    while(1){
        auto msg = queue.pop();
        processMsg(msg);
    }
}

void FileWriteWorker::processMsg(Payload& msg){
    printf("FileWriteWorker::processMsg: nessage %s\n", msg.buffer);

    uint32_t client_id = ntohl(msg.client_id);
    uint32_t msg_id = ntohl(msg.msg_id);
    std::string message((char *)msg.buffer);

    std::string str("");

    str = std::to_string(client_id) + " " + std::to_string(msg_id) + " " + std::string((char *)msg.name) + " " +  message + "\n";

    printf("framed message: %s\n", str.c_str()); 
    
    std::fstream fs;
    fs.open(m_filename.c_str(), std::ios::out | std::ios::app);
    fs <<str.c_str();
    fs.close();
    /*if(m_ofs.is_open()){
        printf("open\n");
        m_ofs <<str.c_str();
    }
    else{
        m_ofs.open(m_filename, std::ofstream::out | std::ofstream::app);
        m_ofs <<str.c_str();
    }*/
}
