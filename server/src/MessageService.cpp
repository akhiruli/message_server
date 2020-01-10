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

Client::Client(ConcurrentQueue<Payload>   *file_writer_queue):m_file_writer_queue(file_writer_queue){

}

Client::~Client(){

}

void Client::on_read(int fd, short ev, void *arg){
    printf("MessageService::on_read:Got read event\n");
    Client *client = reinterpret_cast<Client *>(arg);
    int len = -1;                                                               
    Byte buffer[BUFLEN] = {'\0'};
    len = read(fd, buffer, BUFLEN);

    EventImpl *ev_obj = EventImpl::getInstance();
    if(len == 0){
        printf("Client disconnected\n");
        close(fd);
        ev_obj->removeFromEv(&client->m_evread);
        delete client;
        return;
    }
    else if(len < 0){
        printf("Socket failure, disconnecting client: %s\n", strerror(errno));
        close(fd);
        ev_obj->removeFromEv(&client->m_evread);
        delete client;
        return;
    }
   
    client->setFd(fd); 
    printf("Received buffer : %s\n", buffer);
    client->parseMessage(buffer, len);
}


void Client::sendAck(Payload& msg){
    int len = write(m_fd, "ack", 3);
    if (len == -1){
        printf("Client::sendAck:Failed to send the Ack to client\n");
    }
}

void Client::parseMessage(Byte  *buffer, int len){

    Payload pld;
    pld.buffer = (Byte *)malloc(len);
    memset(pld.buffer, 0, len);
    memcpy(pld.buffer, buffer, len);

    sendAck(pld);
    m_file_writer_queue->push(pld);
}
