#include <chrono>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "scraper.hpp"
#include "DataBase.hpp"
#include "DiscordBot.hpp"
#include <Scheduler/Scheduler.h>


/*
TODO 

fix database saving and downloading 
both for the coupon code and guild data
*/
int main(){
    website web("https://ucngame.com","/codes/guardian-tales-coupon-codes/","wp-block-table",MyHTML_TAG_FIGURE);
    firebase::App *app = firebase::App::Create(firebase::AppOptions());
    DataUp data(app);
    
    Update_cache_Storage();
    DBot bot(TOKEN);
    

    Bosma::Scheduler sch(1);
    sch.every(std::chrono::seconds(16), [&](){
        std::cout << "[ Scheduler ] updateing database\n";
        Save_cache_to_cloud(web.scrap_codes());
        Update_cache_Storage();
        dpp::embed em = dpp::embed().set_title("new Coupon codes");
        int numcode = 0;
        for(int i=0;i<DataUp::CodeStroage.size();i++){
            if(DataUp::CodeStroage[i].isNew){
                numcode++;
                em.add_field(DataUp::CodeStroage[i].code, DataUp::CodeStroage[i].des);
                DataUp::CodeStroage[i].isNew = false;
            }
        }
        if(numcode > 0){
            std::cout << "[ Scheduler ] found " << numcode << " new codes\n";
            for (int i=0; i< DataUp::GuildStorage.size() ; i++)
                bot.bot->message_create(dpp::message(DataUp::GuildStorage[i].channel_id,em));
        }
        
    });
    
    bot.start();
    return 0;
}