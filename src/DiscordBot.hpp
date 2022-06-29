#ifndef Bot
#define Bot
#include "DataBase.hpp"
#include "dpp/appcommand.h"
#include "dpp/channel.h"
#include "dpp/dispatcher.h"
#include "dpp/message.h"
#include "scraper.hpp"
#include <dpp/dpp.h>
#include <future>
#include <vector>
class DBot{
public:
    dpp::cluster* bot;
    DBot(const std::string token){
    bot = new dpp::cluster(token);
    bot->on_slashcommand([&](const dpp::slashcommand_t & sl){
        std::cout << "command naamed "<<sl.command.get_command_name() << " used by "<<sl.command.usr.username << '\n';
        if(sl.command.get_command_name() == "getcodes"){
            dpp::embed em;
            em.set_title("Coupon Codes");
            std::vector<Coupon> code = DataUp::CodeStroage;
            for(int i=0;i<code.size() ;i++){
                em.add_field(code[i].code, code[i].des);
            }
            sl.reply(dpp::message().add_embed(em));
        }
    });

    bot->on_ready([&](const dpp::ready_t & event){
        if (dpp::run_once<struct register_bot_commands>()) {
            std::cout << "Bot Started \n";
            dpp::slashcommand sl = dpp::slashcommand("getcodes","Check and get the last Coupon Codes",bot->me.id);
            dpp::slashcommand dsl = dpp::slashcommand("see_coupon_list","view the old coupon codes",bot->me.id);
            std::vector<dpp::slashcommand> commands;
            commands.push_back(dsl);
            commands.push_back(sl);
            bot->global_bulk_command_create(commands);
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
