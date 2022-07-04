#include <chrono>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "scraper.hpp"
#include "DataBase.hpp"
#include "DiscordBot.hpp"
#include <Scheduler/Scheduler.h>

int main(){
    website web("https://ucngame.com","/codes/guardian-tales-coupon-codes/","wp-block-table",MyHTML_TAG_FIGURE);
    firebase::App *app = firebase::App::Create(firebase::AppOptions());
    DataUp data(app);
    UpdateCodeFromJson();
    DBot bot(TOKEN);
    

    Bosma::Scheduler sch(1);
    sch.every(std::chrono::seconds(16), [&](){
        std::cout << "updateing database\n";
        SaveCouponCodes_toCloud(web.scrap_codes());
        UpdateCodeFromJson();
        //temp 778219240188149770
        dpp::embed em = dpp::embed().set_title("new Coupon codes");
        int numcode = 0;
        for(int i=0;i<DataUp::CodeStroage.size();i++){
            if(DataUp::CodeStroage[i].isNew){
                numcode++;
                em.add_field(DataUp::CodeStroage[i].code, DataUp::CodeStroage[i].des);
            }
        }
        if(numcode > 0){
            std::cout << "found " << numcode << " new codes\n";
            bot.bot->message_create(dpp::message(778219240188149770,em));
        }
        
    });
    
    bot.start();
    return 0;
}