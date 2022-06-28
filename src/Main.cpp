#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "scraper.hpp"
#include "DataBase.hpp"
#include "DiscordBot.hpp"



int main(){
    website web("https://ucngame.com","/codes/guardian-tales-coupon-codes/","wp-block-table",MyHTML_TAG_FIGURE);
    firebase::App *app = firebase::App::Create(firebase::AppOptions());
    DataUp data(app);
    UpdateCodeFromJson();
    DBot bot(TOKEN);
    
    bot.start();
    return 0;
}