#include <MessageService.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFLEN 1024

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
        int len = 0;                                                               
        Byte buffer[BUFLEN] = {'\0'};
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

        printf("Received buffer : %s\n", buffer);

        TempMessage tmsg;
        tmsg.buffer = (Byte *)malloc(len);
        memset(tmsg.buffer, 0, len);
        memcpy(tmsg.buffer, buffer, len);
        tmsg.len = len;

        client->m_msg_list.push_back(tmsg);
    }
    client->setFd(fd); 
    client->parseMessage();
}


void Client::sendAck(Payload& msg){
    int len = write(m_fd, "ack", 3);
    if (len == -1){
        printf("Client::sendAck:Failed to send the Ack to client\n");
    }
}

void Client::parseMessage(){
    while(!m_msg_list.empty()){
        TempMessage tmsg= m_msg_list.front();
        m_msg_list.pop_front();
        if(tmsg.buffer){
            uint32_t payload_len = 0;
            if(m_currbufflen != 0){
                //some buffered mesages to handle
                if(m_firstByte && tmsg.len >= 4){                
                    payload_len = ntohl(*((uint32_t *)tmsg.buffer));
                    tmsg.buffer += 4;
                    m_firstByte = false;
                    tmsg.len -= 4;
                }
            }
            if(m_firstByte && tmsg.len >= 4){
                payload_len = ntohl(*((uint32_t *)tmsg.buffer));
                tmsg.buffer += 4;
                m_firstByte = false;
                tmsg.len -= 4;
            }

            if(tmsg.len >= payload_len){
                Payload *pld = reinterpret_cast<Payload *>(tmsg.buffer);
                m_file_writer_queue->push(*pld);
                tmsg.len -= payload_len;
                tmsg.buffer += payload_len;
                m_firstByte = true;
                if(tmsg.len != 0){
                    m_currbuffer = (Byte *)malloc(tmsg.len);
                    memset(m_currbuffer, 0, tmsg.len);
                    memcpy(m_currbuffer, tmsg.buffer, tmsg.len);
                    m_currbufflen = tmsg.len;
                    m_firstByte = false;
                }
            }
        }
    } 
}
