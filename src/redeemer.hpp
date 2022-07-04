#ifndef redm
#define redm

#include <string>
#include <vector>

struct redeemInfo{
    std::string UserId,userName;
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
        //redeemcodes
    }

};



#endif