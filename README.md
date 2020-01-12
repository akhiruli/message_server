messaging server:
A TCP server wich accepts connections and messages from TCP client. Server send back acknowledgment to client and writes the messages to a file.

High level requirement:
1. An application/client sends data to server.
2. Server acknowledged the message received from client.
3. Server writes the informations received from client to a file.
4. Need to ensure concurrency and integrity of data while writing to file.

High level Design:           
----------                  |----------
|        |                  |          |        
| client | <--------------> |server    |------>|--------
|_ _ _ _ |                  |_ _ _ _ _ |       |  Disk  |
                                               |        |
                                               ----------
                                               
server Design:

                    |-------------------               |-----------------|
                    | Concurrent Queue |---------------| Service thread  | \
                  / |_ _ _ _ _ _ _ _ _ |               |_ _ _ _ _ _ _ _ _|   \
|---------- -----/                                                              \
| listener thrd |   |_ _ _ _ _ _ _ _ _                 |------------------|        \       
|_ _ _ _  _ _ _ |---| Concurrent Queue |---------------| service thread   |           \  |----------------- |  
         \     \    |_ _ _ _ _ _ _ _ _ |               |_ _ _ _ _ _ _ _ _ | -------------|                  |    |-------------------|
          \     \                                                                        | Concurrent Queue |----| filewriter thread |
           \     \  |-------------------               |------------------|--------------|_ _ _ _ _ _ _ _ _ |    |_ _ _ _ _ _ _ _ _  |
            \       | Concurrent Queue |---------------| service thread   |            /                                 |
             \      |_ _ _ _ _ _ _ _ _ |               |_ _ _ _ _ _ _ _ _ |          /                                   |
              \                                                                    /                                 |-----------|
                \  |-------------------                |------------------|     /                                    | Disk      |
                  \| Concurrent Queue |----------------| service thread   |   /                                      |_ _ _ _ _  |
                   |_ _ _ _ _ _ _ _ _ |                |_ _ _ _ _ _ _ _ _ |/

Description Of each Component:

Listener thread: This thread is responsible for listening on a TCP socket (IP + Port). Whenever it receives a client connection it just accepts and send the client file descriptor to one of the service thread in a round robin fashion. The communication channel belween listener and service thread is Concurrent Queue (Thread safe async queue).

Service Thread: It reads data from client FD and parse it and then push to a Concurrent Queue. In the meantime it sends an ACK to client. 
Note: All service thread uses the same Queue.

Concurrent Queue: It is a thread safe async queue implemented on top of standard queue.

Filewriter Thread: It pull data from Concurrent Queue and frame the message and write it to disk(It may buffer some message based on the configuration and then write. This is done for performance improvement).

IMPLEMENTATION DETAILS:
  Language: C++03, C++11
  Script: Shell
  Protocol: TCP
  Socket: Non blocking
  OS: Ubuntu 16.04
  FD handler: Used libevent (epoll library has been used for this project)
  
Compilation Steps:
  prerequisite:
    OS: Ubuntu 16.04 (though it should compile in any other linux falvour)
    compiler: g++ compiler (c++11 and c++14)
    Compilation Tool: Make
    Thirdparty Library: libevent
  steps:
    1. inside message_server folder run the following command.
        ./build.sh
        Above command will create two binary msgserver and msgclient under bin directory.
 Steps to run:
 Server:
    1. Before starting the server please run the following script in sudo mode.
        sudo ./tcp_tuning_params.sh
    2. Please change the configuration in the config file app.conf as per the need
       eg. MSG_PER_WRITE=5 , means filewriter thread will buffer 5 messages before writing to file.
    3. Now start the server using the following command:
        bin/msgserver
 Client:
    1. Please change the configuration in the config file app.conf as per the need.
       e.g if you want a sleep of belween two message the use the following configuration.
          TIME_INTERVAL_BTN_MSG=5
    2. run the client using the following command:
        bin/msgclient <client_id(int type)> <messages in comma separted format>
        e.g: bin/msgclient 1 hi,hello
  
        It will send two messages to server (hi and hello) with client_id 1 and sequential messageIds.
        
  Filewriter thread writes messages to configured file mentioned in app.conf in the following format:
  
  <client_id> <message_id> <message_name> <message>
  
  TESTS:
  
  1. Number of concurrent TCP connections handles by server:
  2.
 
  
