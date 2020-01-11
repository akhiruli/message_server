#include <MessageService.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFLEN 1200

MessageService::MessageService() {
}

MessageService::~MessageService(){
}
void MessageService::entrypoint(ConcurrentQueue<int>& queue){
    printf("created a thread with threadId %d\n", std::this_thread::get_id());
    while(1){
        auto fd = queue.pop();
        printf("Received a fd %d\n", fd);
        processEvent(fd);
    }

    printf("exiting thread hreadId %d\n", std::this_thread::get_id());
}

void MessageService::processEvent(int fd){
    printf("MessageService::processEvent: fd %d\n", fd);
    EventImpl *ev_obj = EventImpl::getInstance();
    Client *client = new Client(m_file_writer_queue);
    ev_obj->attachFdtoEv(&client->m_evread, fd, Client::on_read, client);
    ev_obj->addToEv(&client->m_evread);
}

Client::Client(ConcurrentQueue<Payload>   *file_writer_queue):m_file_writer_queue(file_writer_queue),
        m_firstByte(true), m_currbuffer(NULL), m_currbufflen(0)
{

}

Client::~Client(){
    
}

void Client::on_read(int fd, short ev, void *arg){
    printf("MessageService::on_read:Got read event\n");
    Client *client = reinterpret_cast<Client *>(arg);
    EventImpl *ev_obj = EventImpl::getInstance();
    for(int i=0; i < NUM_READ_TRY; i++){
        int len = -1;                                                               
        Byte *buffer = (Byte *)malloc(BUFLEN);
        memset(buffer, '\0', BUFLEN);
        len = read(fd, buffer, BUFLEN);

        if(len == 0){
            printf("Client disconnected\n");
            close(fd);
            ev_obj->removeFromEv(&client->m_evread);
            delete client;
            return;
        }
        else if(len < 0){
            if(errno != EAGAIN){
                printf("Socket failure, disconnecting client: %s\n", strerror(errno));
                close(fd);
                ev_obj->removeFromEv(&client->m_evread);
                delete client;
                return;
            }
            else if(errno == EAGAIN){
                break;
            }
        }

        printf("Received len: %d\n", len);

        TempMessage tmsg;
        tmsg.buffer = buffer;
        tmsg.len = len;

        client->m_msg_list.push_back(tmsg);
        //struct Payload *p_load = reinterpret_cast<struct Payload *>(buffer);
        //client->m_file_writer_queue->push(*p_load);
    }
    client->setFd(fd); 
    client->parseMessage();
    client->sendAck();
}


void Client::sendAck(){
    int len = write(m_fd, "ack", 3);
    if (len == -1){
        printf("Client::sendAck:Failed to send the Ack to client\n");
    }
}

void Client::parseMessage(){
    int req_size = sizeof(struct Payload);
    struct Payload *p_load = NULL;
    while(!m_msg_list.empty()){
        TempMessage tmpMsg = m_msg_list.front();
        m_msg_list.pop_front();
        printf("%d:%d\n", tmpMsg.len, req_size);
        if(tmpMsg.len == req_size){
            p_load = reinterpret_cast<struct Payload *>(tmpMsg.buffer);
            m_file_writer_queue->push(*p_load);
            printf("Client::parseMessage:pushing\n");
        }
        else if(tmpMsg.len > req_size){
            p_load = reinterpret_cast<struct Payload *>(tmpMsg.buffer);
            m_file_writer_queue->push(*p_load);
            TempMessage tmp;
            int leftByte = tmpMsg.len -req_size;
            tmp.buffer = (Byte *) malloc(leftByte);
            memset(tmp.buffer, '\0', leftByte);
            memcpy(tmp.buffer, tmpMsg.buffer + req_size, leftByte);
            tmp.len = leftByte;
            m_msg_list.push_front(tmp);
        }
        else{
            if(!m_msg_list.empty()){
                TempMessage mergeMsg;
                TempMessage tmp = m_msg_list.front();
                m_msg_list.pop_front();
                mergeMsg.buffer = (Byte *)malloc(tmpMsg.len + tmp.len);
                memset(mergeMsg.buffer, '\0', tmpMsg.len + tmp.len);
                memcpy(mergeMsg.buffer, tmpMsg.buffer, tmpMsg.len);
                mergeMsg.buffer += tmpMsg.len;
                memcpy(mergeMsg.buffer, tmp.buffer, tmp.len);

                m_msg_list.push_front(mergeMsg);
            }
            else{
                m_msg_list.push_front(tmpMsg);
                break;
            } 
        }
    }
} 



