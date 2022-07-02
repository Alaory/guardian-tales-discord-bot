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
#include <string>
#include <variant>
#include <vector>

class DBot{
public:
    dpp::cluster* bot;
    DBot(const std::string token){
    bot = new dpp::cluster(token);
    bot->on_slashcommand([&](const dpp::slashcommand_t & sl){
        std::cout << "Command named "<<sl.command.get_command_name() << " used by "<<sl.command.usr.username << '\n';
        if(sl.command.get_command_name() == "getcodes"){
            dpp::embed em;
            em.set_title("Coupon Codes");
            std::vector<Coupon> code = DataUp::CodeStroage;
            for(int i=0;i<code.size() ;i++){
                em.add_field(code[i].code, code[i].des);
            }
            if(code.size() ==0){
                sl.reply(dpp::message("Couldn't find any redeem codes"));
            }else{
                sl.reply(dpp::message().add_embed(em));
            }
        }else if (sl.command.get_command_name() == "userid") {
            sl.reply(dpp::message(std::get<std::string>(sl.get_parameter("userid"))));
        }
        else {
            sl.reply(dpp::message("Command Not Found"));
        }
    });

    bot->on_ready([&](const dpp::ready_t & event){
        if (dpp::run_once<struct register_bot_commands>()) {
            std::cout << "Bot Started \n";
            
            dpp::slashcommand sl = dpp::slashcommand("getcodes","Check and get the last Coupon Codes",bot->me.id);
            dpp::slashcommand CR = dpp::slashcommand("register","register your user number to get into the auto redeem code list",bot->me.id)
            .add_option(dpp::command_option(dpp::command_option_type::co_string,"userid","enter your user id from the game",true));
            
            std::vector<dpp::slashcommand> commands;



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
