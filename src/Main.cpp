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
    sch.every(std::chrono::seconds(15), [&](){
        std::cout << "updateing database\n";
        UpdateSaveCodeToJson(web.scrap_codes());
    });
    sch.every(std::chrono::hours(1),[&](){
        std::cout << "retreving database\n";
        UpdateCodeFromJson();
    });

    
    bot.start();
    return 0;
}