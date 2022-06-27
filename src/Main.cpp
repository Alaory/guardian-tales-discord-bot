#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "scraper.hpp"
#include "DataBase.hpp"
#include <dpp/dpp.h>
#include "DiscordBot.hpp"


//TODO add discord bot 
//make it open source under the GPT licence
//bug fix
int main(){
    website web("https://ucngame.com","/codes/guardian-tales-coupon-codes/","wp-block-table",MyHTML_TAG_FIGURE);
    firebase::App *app = firebase::App::Create(firebase::AppOptions());

    //std::string toke = TOKEN;
    //DBot botme(toke);
    std::vector<Coupon> cod = web.scrap_codes();std::vector<std::string> codes;
    DataUp dp(app);
    

    dp.saveData(cod);
    std::cin.get();
    //botme.start();
    return 0;
}