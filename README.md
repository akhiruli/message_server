messaging server:
A highly scalable TCP server wich accepts connections and messages from TCP client. Server send back acknowledgment to client and writes the messages to a file.

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

    1. export environment variable MS_HOME to home directory of message_server
    eg. export MS_HOME=~/message_server

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

  Test Environment:
    OS: Ubuntu 16.04
    Memory: 16G
    Disk: 50G 
    both client and server is in same server
 
  case1: Concurrent connection-
    Total  Number of concurrent TCP connections (6 messages per client) handles by server: 10000 (didn't test beyond that)
    Note: Need to increase ulimit
        
  Case2: load test: Ran 10000 successfull request in 20 secs (using shell script ran in loop of 10000 and all request was running in background)

  Case3: Latency test:

    No Of Client | number of message | Total Time taken(micro seconds)| Averge Latency (microseconds|
-------------------------------------------------------------------------------------
      1          |   100             |      6375                        |        63.75                |
 
      100        |   100(per client) |  2989                            |   29.89
                                     | 17981                            |   179.81
                                     | 13633                            |   136.33
                                     | 18172                            |   181.72
                                     | 2883                             |   28.83
                                     | 4176                             |   41.76
                                    |   15955                           |   159.55
                                    |   3380                            |   33.80
                                    |   5246                            |   52.46
                                    |   9575                            |   95.75
                                    |   7609                            |   76.09
                                    |   8180                            |   81.80
                                    |   24349                           |   243.49
                                    |   4973                            |   49.73
                                    |   3425                            |   34.25
                                    |   9395                            |   93.95
                                    |   9872                            |   98.72
                                    |   3168                            |   31.68
                                    |   5063                            |   50.63
                                    |   23165                           |   231.65
                                    |   8585                            |   85.85
                                    |   12658                           |   126.58
                                    |   12722                           |   127.22
                                    |   12071                           |   120.71 
                                    |   10343                           |   103.43
                                    |   8689                            |   86.89
                                    |   2954                            |   29.54
                                    |   2904                            |   29.04
                                    |   3178                            |   31.78
                                    |   2957                            |   29.57
                                    |   2872                            |   28.72
                                    |   2980                            |   29.80
                                    |   2895                            |   28.95
                                    |   12803                           |   128.03
                                    |   10869                           |   108.69
                                    |   3627                            |   36.27
                                    |   15826                           |   158.26
                                    |   6403                            |   64.03
                                    |   5767                            |   57.67
                                    |   8376                            |   83.76
                                    |   2842                            |   28.42
                                    |   12572                           |   125.72
                                    |   8426                            |   84.26
                                    |   8517                            |   85.17
                                    |   8843                            |   88.43
                                    |   8248                            |   82.48
                                    |   10238                           |   102.38
                                    |   24530                           |   245.30
                                    |   22223                           |   222.23
                                    |   14675                           |   146.75
                                    |   3693                            |   36.93
                                    |   4070                            |   40.70
                                    |   20085                           |   200.85
                                    |   9852                            |   98.52
                                    |   9073                            |   90.73
                                    |   9181                            |   91.81
                                    |   8263                            |   82.63
                                    |   15078                           |   150.78
                                    |   10373                           |   103.73
                                    |   9398                            |   93.98
                                    |   11350                           |   113.50
                                    |   9507                            |   95.07
                                    |   11701                           |   117.01
                                    |   8203                            |   82.03
                                    |   7988                            |   79.88
                                    |   10874                           |   108.74
                                    |   4293                            |   42.93
                                    |   4118                            |   41.18
                                    |   16130                           |   161.30
                                    |   4009                            |   40.09
                                    |   3196                            |   31.96
                                    |   13903                           |   139.03
                                    |   7330                            |   73.30
                                    |   7321                            |   73.21
                                    |   12278                           |   122.78
                                    |   4381                            |   43.81
                                    |   7792                            |   77.92
                                    |   8050                            |   80.50
                                    |   7440                            |   74.40
                                    |   7351                            |   73.51
                                    |   8250                            |   82.50
                                    |   7979                            |   79.79
                                    |   7274                            |   72.74
                                    |   7451                            |   74.51
                                    |   7229                            |   72.29
                                    |   7404                            |   74.04
                                    |   6221                            |   62.21
                                    |   11111                           |   111.11
                                    |   4995                            |   49.95
                                    |   4930                            |   49.30
                                    |   10194                           |   101.94
                                    |   9809                            |   98.09
                                    |   9406                            |   94.06
                                    |   9888                            |   98.88
                                    |   8028                            |   80.28
                                    |   7682                            |   76.82
                                    |   8483                            |   84.83
                                    |   10511                           |   105.11
                                    |   12548                           |   125.48
                                    |   16774                           |   167.74
                                    |   8575                            |   85.75
 
