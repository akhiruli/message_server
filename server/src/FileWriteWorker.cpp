#include <FileWriteWorker.h>
#include <Config.h>

FileWriteWorker::FileWriteWorker(): m_bufferMsg(""), m_numbufferedmsg(0){
    Config *cfg = Config::getInstance();
    if((cfg->m_cfg).filename.empty())
        m_filename = "message_info.txt";
    else
        m_filename = (cfg->m_cfg).filename;

    m_maxbufmsg = (cfg->m_cfg).msg_per_write;
    printf("FileWriteWorker::FileWriteWorker: filename=%s, maxbuf= %d\n", m_filename.c_str(), m_maxbufmsg);
}

FileWriteWorker::~FileWriteWorker(){
    if(m_ofs.is_open()){
        m_ofs.close();
    }
}

void FileWriteWorker::entrypoint(ConcurrentQueue<Payload* > &queue){
    printf("FileWriteWorker::entrypoint: thread_id %d\n", std::this_thread::get_id());

    while(1){
        auto msg = queue.pop();
        processMsg(msg);
    }
}

void FileWriteWorker::processMsg(Payload *msg){

    uint32_t client_id = ntohl(msg->client_id);
    uint32_t msg_id = ntohl(msg->msg_id);
    std::string message((char *)msg->buffer);

    std::string str("");

    str = std::to_string(client_id) + " " + std::to_string(msg_id) + " " + std::string((char *)msg->name) + " " +  message + "\n";

    free(msg);

    m_bufferMsg.append(str);
    ++m_numbufferedmsg;

    if(m_numbufferedmsg < m_maxbufmsg){
        printf("Buffering the message for future write as number of message haven't" 
            " reached the minimum configured number, framed msg: %s\n", str.c_str());
        return;
    } 

    printf("Writing to file now\n");
        
    std::fstream fs;
    fs.open(m_filename.c_str(), std::ios::out | std::ios::app);
    fs <<m_bufferMsg.c_str();
    fs.close();

    m_numbufferedmsg = 0;
    m_bufferMsg = "";
}
