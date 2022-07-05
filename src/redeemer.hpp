#ifndef redm
#define redm

#include "DataBase.hpp"
#include <httplib.h>
#include <string>
#include <vector>

struct redeemInfo{
    std::string UserId,userName,region="EU";
};

/*
redeemer a class that redeems the coupon codes 
from the game official site from it's LOCAL storage 
need to be updated so often
*/
class redeem{
public:
    inline static std::vector<redeemInfo> local_RedeemInfo;
    static void Redeem(){
        httplib::Client cli("https://guardiantales.com");
        httplib::Headers head ={
            {"Accept","*/*"},
            {"Accept-Encoding",R"(gzip, deflate, br)"},
            {"Content-Type","application/x-www-form-urlencoded; charset=UTF-8"}
        };
        
        for(int i=0;i<local_RedeemInfo.size();i++){            
            for (int j=0; j< DataUp::CodeStroage.size()&& j < 6; j++) {
                httplib::Params parm;
                parm.emplace("region",local_RedeemInfo[i].region);
                parm.emplace("userId",local_RedeemInfo[i].UserId);//i know its bad but too lazy to fix it :( 
                parm.emplace("code",DataUp::CodeStroage[j].code);
                std::cout << cli.Post("/coupon/redeem",head,parm)->body << '\n' << "container size " << parm.size() << '\n';
            }
        }


        
    }

};



#endif