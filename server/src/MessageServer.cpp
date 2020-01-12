#include<MessageServer.h>
#include <Config.h>

MessageServer::MessageServer(): m_index(0){
}

MessageServer::~MessageServer(){
}

void MessageServer::run()                                                                              
{
    spawnWorkers();
                                                                                                     
    int fd = prepareTcpSvr();
    if(fd < 0)
    {    
        printf("MessageServer::run: failed to start the server\n");
        return;
    }    

    EventImpl *ev_obj = EventImpl::getInstance();                                                      
    ev_obj->attachFdtoEv(&m_evread, fd, on_accept, this);                                                
    ev_obj->addToEv(&m_evread);
} 

void MessageServer::spawnWorkers(){
    ConcurrentQueue<Payload *> *queue_file_writer = new ConcurrentQueue<Payload *>();

    for(auto i = 0; i <4; i++){
        MessageService  *m_obj = new MessageService();
        m_obj->setFileWriterQueue(queue_file_writer);
        ConcurrentQueue<int> *queue = new ConcurrentQueue<int>();
        std::thread *th_obj = new std::thread(&MessageService::entrypoint, m_obj, std::ref(*queue));
        th_obj->detach();
        m_threadpool.push_back(std::make_pair(th_obj, queue));
    }

    FileWriteWorker *f_writer = new FileWriteWorker(); 
    std::thread *th_obj = new std::thread(&FileWriteWorker::entrypoint, f_writer, std::ref(*queue_file_writer));

    m_fileWriterthread = std::make_pair(th_obj, queue_file_writer);
}

int MessageServer::prepareTcpSvr()
{       
    int listen_fd = -1;
    struct sockaddr_in listen_addr;                                                                    
    int reuseaddr_on = 1;          

    listen_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)                                                                                 
    {                                                                                                  
        printf("MessageServer::prepareTcpSvr: listen failed\n");
        return listen_fd;
    }

    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on,
                sizeof(reuseaddr_on)) == -1)
    {
        printf("MessageServer::prepareTcpSvr: setsockopt failed\n");
        return -1;
    }

    Config *cfg = Config::getInstance();

    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_addr.s_addr = INADDR_ANY;
    listen_addr.sin_port = htons((cfg->m_cfg).port);

    if (bind(listen_fd, (struct sockaddr *)&listen_addr,
                sizeof(listen_addr)) < 0)
    {
        printf("MessageServer::prepareTcpSvr: bind failed\n");
        return -1;
    }

    if (listen(listen_fd, 10) < 0){
        printf("MessageServer::prepareTcpSvr: failed to listen\n");
        return -1;
    }
    /* Set the socket to non-blocking, this is essential in event
     * based programming with libevent. */
    if (setnonblock(listen_fd) < 0)
    {
        printf("MessageServer::prepareTcpSvr: failed to set server socket to non-blocking\n");
        return -1;
    }

    return listen_fd;
}

int MessageServer::setnonblock(int fd)
{
    int flags;

    flags = fcntl(fd, F_GETFL);
    if (flags < 0)
        return flags;
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0)
        return -1;

    return 0;
}

void MessageServer::on_accept(int fd, short ev, void *arg)
{
    MessageServer *server = reinterpret_cast<MessageServer *>(arg);

    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    /* Accept the new connection. */
    client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd == -1) {
        printf("MessageServer::on_accept:accept failed\n");
        return;
    }

    /* Set the client socket to non-blocking mode. */
    if (server->setnonblock(client_fd) < 0)
        printf("MessageServer::on_accept:failed to set client socket non-blocking\n");
    printf("Accepted sokcet with fd %d\n", client_fd);

    server->loadbalance_fd(client_fd);

}

void MessageServer::loadbalance_fd(int fd){
    if(m_index >= m_threadpool.size())
        m_index = 0;

    ConcurrentQueue<int> *queue = m_threadpool[m_index].second;

    queue->push(fd);

    m_index++;
}
