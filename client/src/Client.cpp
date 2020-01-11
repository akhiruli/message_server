#include <Client.h>

Client::Client(std::string ip, uint32_t port): m_ip(ip), m_port(port){

}

Client::~Client(){

}

bool Client::startClient(){
    struct sockaddr_in servaddr, cli;
    int connfd;

    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_fd == -1) {
        printf("socket creation failed... error= %s\n", strerror(errno));
        return false;
    }

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(m_ip.c_str());
    servaddr.sin_port = htons(m_port);

    if (connect(m_fd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed... error= %s\n", strerror(errno));
        return false;
    }
    else
        printf("connected to the server..\n");

    return true;
}


int Client::sendData(std::vector<std::string> messages, uint32_t clientId){
    int write_len = 0;
    for(int i=0; i < messages.size(); i++){

        struct Payload pload;
        pload.client_id = htonl(clientId);
        pload.msg_id = htonl(i+1);
        memset(pload.buffer, '\0', PAYLOAD_SIZE);
        memcpy(pload.buffer, messages[i].c_str(), messages[i].size());

        std::string msgname("client");
        msgname = msgname + "_" +std::to_string(clientId);
        memset(pload.name, '\0', NAME_SIZE);
        strcpy(pload.name, msgname.c_str());
 
        int len =  write(m_fd, (Byte *)&pload, sizeof(pload));

        if(len < 0){
            printf("Failed to send data to server with error %s\n", strerror(errno));
            if(write_len == 0)
                return -1;
            else
                return write_len;
        }

        write_len += len;

        char buff[MAX] = {'\0'};
        int read_len = read(m_fd, buff, sizeof(buff));
        if(read_len < 0){
            printf("Failed to read the response from server\n");
            return write_len;
        }

        printf("Received ack: %s\n", buff);
    }
    return write_len;
}

#if 0
        memset(pload.name, '\0', 512);
        std::string msgname("client");
        msgname = msgname + "_" +std::to_string(clientId);
        strcpy(pload.name, msgname.c_str());

        size_t msglen = messages[i].size() + 1;
        pload.buffer = (Byte *) malloc(msglen);
        memset(pload.buffer, '\0', msglen);
        memcpy(pload.buffer, messages[i].c_str(), msglen-1);

        uint32_t payload_len = htonl(520 + msglen);

        Byte buffer_to_send[payload_len] = {'\0'};

        uint32_t curr_len = 0;
        memcpy(buffer_to_send, &payload_len, 4);
        curr_len += 4;

        memcpy(buffer_to_send + curr_len, &pload.client_id, 4);
        curr_len += 4;

        memcpy(buffer_to_send + curr_len, &pload.msg_id, 4);
        curr_len += 4;

        memcpy(buffer_to_send + curr_len, &pload.name, 512);
        curr_len += 512;

        memcpy(buffer_to_send + curr_len, &pload.buffer, msglen);
        curr_len += msglen;


#endif
