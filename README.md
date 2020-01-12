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

                    |-------------------               |--------------
                    | Concurrent Queue |---------------| Service thread
                  / |_ _ _ _ _ _ _ _ _ |               |
|---------- -----/
| listener thrd |   |_ _ _ _ _ _ _ _ _               
|_ _ _ _  _ _ _ |---| Concurrent Queue |
         \     \    |_ _ _ _ _ _ _ _ _ |
          \     \ 
           \     \  |-------------------
            \       | Concurrent Queue |
             \      |_ _ _ _ _ _ _ _ _ |
              \        
                \      |-------------------
                 | Concurrent Queue |
                   |_ _ _ _ _ _ _ _ _ |
