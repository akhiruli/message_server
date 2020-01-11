#include <MessageServer.h>
#include <Config.h>

int main(){
    Config *cfg = Config::getInstance();
    MessageServer *server = new MessageServer();

    EventImpl *ev_obj = EventImpl::getInstance();
    if(ev_obj && ev_obj->initEv())
    {
        server->run();
        ev_obj->dispatchEvloop();
    }

    return 0;
}
