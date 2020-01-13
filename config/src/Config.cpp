#include <Config.h>
#include <mutex>

Config* Config::m_obj = NULL;
static std::mutex config_mutex;

Config::Config(): m_filename("app.conf"){
    getConfigFileName();
    loadConfig();
}

Config* Config::getInstance(){
    if(!m_obj){
        std::lock_guard<std::mutex> lock(config_mutex);
        if(!m_obj)
            m_obj = new Config();
    }

    return m_obj;
}

void Config::getConfigFileName(){
    const char* ms_home = getenv( "MS_HOME" );
    if(ms_home){
        std::string str(ms_home);
        str.erase(std::remove_if(str.begin(), str.end(), isspace), str.end());
        size_t len = str.size();
        std::string key("/");
        if(str[len-1] == '/')
            str.replace (str.size()-1, key.length(),"");

        m_filename = str.append("/app.conf");
    }
    else{
        printf("Failed to load config file\n");
    }
}

bool Config::loadConfig(){
    std::ifstream ifs(m_filename.c_str());
    std::string line;

    std::map<std::string, std::string> tempMap;
    if(ifs.is_open()){
        while(std::getline(ifs, line)){
            line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
            if(line[0] == '#' || line.empty())
                continue;

            auto delmiterPos = line.find("=");
            auto name = line.substr(0, delmiterPos);
            auto value = line.substr(delmiterPos + 1);

            tempMap.insert(std::make_pair(name, value));
        }
    }
    else
        return false;

    fillConfigParams(tempMap);

    return true;
}

void Config::fillConfigParams( std::map<std::string, std::string>& tempMap){
    std::map<std::string, std::string>::iterator itr = tempMap.find("IP");
    if(itr != tempMap.end()){
        m_cfg.ip = itr->second; 
    }

    itr = tempMap.find("PORT");
    if(itr != tempMap.end()){
        m_cfg.port = std::stoi(itr->second);
    }
    
    itr = tempMap.find("FILE_NAME");
    if(itr != tempMap.end()){
        m_cfg.filename = itr->second;
    }

    itr = tempMap.find("MSG_PER_WRITE");
    if(itr != tempMap.end()){
        m_cfg.msg_per_write = std::stoi(itr->second);
    }

    itr = tempMap.find("TIME_INTERVAL_BTN_MSG");
    if(itr != tempMap.end()){
        m_cfg.time_interval_btn_msg = std::stoi(itr->second);
    }
    printf("ip=%s, port=%d, filename=%s, msg_per_write=%d\n", m_cfg.ip.c_str(), m_cfg.port, m_cfg.filename.c_str(), m_cfg.msg_per_write);
}
