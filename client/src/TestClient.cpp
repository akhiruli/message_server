#include <Client.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <Config.h>

std::vector<std::string> parseMessage(std::string& msg){
    std::vector <std::string> messages; 
    std::stringstream check(msg);
    std::string temp;

    while(getline(check, temp, ',')){
        messages.push_back(temp);
    }

    return messages;
}

int main(int argc, char** argv){
    if (argc != 3){
        printf("run as below:\n ./msgclient <client_id(int type)> <message in comma separted format>\n like: ./msgclient 1 hi,hello\n");
        return 0;
    }

    uint32_t client_id = std::stoi(argv[1]);
    std::string msg = const_cast<char *>(argv[2]);
    std::vector<std::string> messages = parseMessage(msg);

    Config *cfg = Config::getInstance();
    Client cli((cfg->m_cfg).ip, (cfg->m_cfg).port);
    cli.startClient();
    cli.sendData(messages, client_id);
    return 0;
}
