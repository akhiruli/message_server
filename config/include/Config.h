#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <string>
#include <fstream>
#include <map>
#include <iostream>
#include <algorithm>
#include <cstdlib>

struct ConfigParams{
    std::string ip;
    uint16_t    port;
    std::string filename;
    uint16_t    msg_per_write;
    uint16_t    time_interval_btn_msg;

    ConfigParams(){
        ip = "127.0.0.1";
        port = 9000;
        filename = "message_info.txt";
        msg_per_write = 1;
        time_interval_btn_msg = 0;
    }
};

class Config{
    public:
        static Config* getInstance();
        bool loadConfig();
        void fillConfigParams( std::map<std::string, std::string>&);
        void getConfigFileName();
        ConfigParams    m_cfg;
    private:
        Config();
        Config(const Config&) = delete;
        static Config   *m_obj;
        std::string     m_filename;
};
#endif
