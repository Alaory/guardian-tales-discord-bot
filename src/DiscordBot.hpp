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
        std::cout << "[ DiscordBot ] Command named "<<sl.command.get_command_name() << " used by "<<sl.command.usr.username << '\n';
        
        //TODO USE A switch statement
        
        if(sl.command.get_command_name() == "getcodes")
        {

            dpp::embed em;

            em.set_title("Coupon Codes");
            em.set_color(dpp::colors::cadmium_green);
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

        else if (sl.command.get_command_name() == "setupdate") {
            sl.reply(dpp::message("this channel will receive the latest Coupon codes\n"));
            Guild gld;
            gld.channel_id = sl.command.channel_id;
            gld.guild_id = sl.command.guild_id;
            gld.Channel_name = "unknown";
            gld.Guild_Name = "unknown";
            std::vector<Guild> temp_guild = {gld};
            bool found = false;
            for (int i=0; i<DataUp::GuildStorage.size(); i++) {
                if(gld.guild_id == DataUp::GuildStorage[i].guild_id){
                    DataUp::GuildStorage[i].channel_id = gld.channel_id;
                    found = true;
                    break;
                }
            }
            if(!found){
                Save_cache_to_cloud(DataUp::CodeStroage,temp_guild);
            }else{
                Save_cache_to_cloud();
            }
        }


        else if (sl.command.get_command_name() == "register") 
        {
            redeemInfo user;
            user.UserId = std::get<std::string>(sl.get_parameter("userid"));
            user.userName = std::get<std::string>(sl.get_parameter("username"));
            if(user.UserId.length() != 12){
                sl.reply(dpp::message("please enter your user id"));
            }else{
                DataUp::local_RedeemInfo.push_back(user);
                sl.reply(dpp::message("done :)"));
                Save_cache_to_cloud();
            }
        }
        
        else if (sl.command.get_command_name() == "redeemme") {
            sl.reply(dpp::message("yeet"));
            redeem::Redeem();
        }

        else 
        {
            sl.reply(dpp::message("Command Not Found"));
        }

        std::cout << "[ DiscordBot ] guild_id: "<<sl.command.guild_id << '\n';

    });



    bot->on_ready([&](const dpp::ready_t & event){
        if (dpp::run_once<struct register_bot_commands>()) {
            std::cout << "[ DiscordBot ] Bot Started \n";
            
            dpp::slashcommand sl = dpp::slashcommand("getcodes","Check and get the last Coupon Codes",bot->me.id);
            dpp::slashcommand Upch = dpp::slashcommand("SetUpdate","Set this channel to receive the news coupon codes",bot->me.id);
            dpp::slashcommand CR = dpp::slashcommand("register","register your user number to get into the auto redeem code list",bot->me.id)
            .add_option(dpp::command_option(dpp::command_option_type::co_string,"userid","enter your user id from the game",true))
            .add_option(dpp::command_option(dpp::command_option_type::co_string,"username","enter your ingame name",true));
            
            std::vector<dpp::slashcommand> commands;



            commands.push_back(sl);
            commands.push_back(Upch);
            commands.push_back(CR);
            bot->global_bulk_command_create(commands);








            // for debug stuff
            //bot->guild_command_create(dpp::slashcommand("redeemme","noting",bot->me.id), 739248968600387595);
        }
    });



}

    void start(){
    printf("[ DiscordBot ] starting bot\n");
    bot->start(false);
    }

    void GetCodes(){}
    ~DBot(){
    delete bot;
}
};


#endif
