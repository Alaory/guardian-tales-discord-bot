#ifndef Bot
#define Bot

#include "DataBase.hpp"
#include "dpp/appcommand.h"
#include "dpp/channel.h"
#include "dpp/dispatcher.h"
#include "dpp/message.h"
#include "scraper.hpp"
#include "redeemer.hpp"
#include <dpp/dpp.h>
#include <future>
#include <string>
#include <variant>
#include <vector>


/*
a simple wrap around dpp::bot
just to init the commands
*/
class DBot{
public:
    dpp::cluster* bot;
    DBot(const std::string token){
    bot = new dpp::cluster(token);
    bot->on_slashcommand([&](const dpp::slashcommand_t & sl){
        std::cout << "Command named "<<sl.command.get_command_name() << " used by "<<sl.command.usr.username << '\n';
        //TODO USE A switch statement
        if(sl.command.get_command_name() == "getcodes")
        {


            dpp::embed em;

            em.set_title("Coupon Codes");

            std::vector<Coupon> code = DataUp::CodeStroage;

            for(int i=0;i<code.size() && i < 6 ;i++){

                em.add_field(code[i].code, code[i].des);
            }
            if(code.size() ==0){

                sl.reply(dpp::message("Couldn't find any redeem codes"));
            }else{

                sl.reply(dpp::message().add_embed(em));
            }


        }


        else if (sl.command.get_command_name() == "register") 
        {
            sl.reply(dpp::message(std::get<std::string>(sl.get_parameter("userid"))));
        }
        
        else if (sl.command.get_command_name() == "redeemme") {
            redeemInfo user = {"6969","user","EU"};
            redeem::local_RedeemInfo.push_back(user);
            redeem::Redeem();
            sl.reply(dpp::message("yeet"));
        }

        else 
        {
            sl.reply(dpp::message("Command Not Found"));
        }

        std::cout << "guild_id: "<<sl.command.guild_id << '\n';
    });

    bot->on_ready([&](const dpp::ready_t & event){
        if (dpp::run_once<struct register_bot_commands>()) {
            std::cout << "Bot Started \n";
            
            dpp::slashcommand sl = dpp::slashcommand("getcodes","Check and get the last Coupon Codes",bot->me.id);
            dpp::slashcommand CR = dpp::slashcommand("register","register your user number to get into the auto redeem code list",bot->me.id)
            .add_option(dpp::command_option(dpp::command_option_type::co_string,"userid","enter your user id from the game",true))
            .add_option(dpp::command_option(dpp::command_option_type::co_string,"username","enter your ingame name",true));
            
            std::vector<dpp::slashcommand> commands;



            commands.push_back(sl);
            commands.push_back(CR);
            bot->global_bulk_command_create(commands);









            bot->guild_command_create(dpp::slashcommand("redeemme","noting",bot->me.id), 739248968600387595);
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
