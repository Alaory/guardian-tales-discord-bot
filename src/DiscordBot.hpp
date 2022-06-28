#ifndef Bot
#define Bot
#include "DataBase.hpp"
#include "dpp/message.h"
#include "scraper.hpp"
#include <dpp/dpp.h>
#include <vector>
class DBot{
public:
    dpp::cluster* bot;
    DBot(const std::string token){
    bot = new dpp::cluster(token);
    bot->on_slashcommand([](const dpp::slashcommand_t & sl){
        if(sl.command.get_command_name() == "GetCodes" || true){
            dpp::embed em;
            em.set_title("Coupon Codes");
            std::vector<Coupon> code = DataUp::CodeStroage;
            for(int i=0;i<code.size() && i < 5;i++){
                em.add_field(code[i].code, code[i].des);
            }
            sl.reply(dpp::message().add_embed(em));
        }
    });
}

    void start(){
    printf("starting bot\n");
    bot->start(false);
    }

    void GetCodes(){}
    ~DBot(){
    delete bot;
}
};


#endif
