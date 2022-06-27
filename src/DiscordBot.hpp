#ifndef Bot
#define Bot


#include "dpp/cluster.h"
#include "dpp/dispatcher.h"
#include "dpp/presence.h"
class DBot{
public:
    dpp::cluster *bot;
    DBot(const std::string token){
        bot = new dpp::cluster(token);
    


    
        bot->on_ready([](const dpp::ready_t & im){
            std::cout << im.raw_event << '\n';
        });
    
    
    
    
    
        
    
    
    
    
    
    
    
    
    
    
    
    }

    void start(){
        bot->start(false);
    }

    ~DBot(){
        delete bot;
    }
};


#endif