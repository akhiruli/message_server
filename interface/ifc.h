#ifndef _IFC_H_
#define _IFC_H_
#include<stdint.h>
#define PAYLOAD_SIZE 1024
#define NAME_SIZE 128

typedef unsigned char  Byte;
struct Payload{
    uint32_t    client_id;
    uint32_t    msg_id;
    char        name[NAME_SIZE];
    Byte        buffer[PAYLOAD_SIZE]; 
}__attribute__((packed));

struct Message{
    uint32_t    len;
    Payload     payload;
}__attribute__((packed));

#endif
