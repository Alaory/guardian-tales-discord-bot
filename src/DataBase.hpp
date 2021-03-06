#ifndef database
#define database
#include "dpp/nlohmann/json.hpp"
#include "dpp/snowflake.h"
#include "scraper.hpp"
#include <cstdint>
#include <iostream>
#include <vector>
#include <functional>
#include <firebase/app.h>
#include <firebase/storage.h>

using firebase::storage::Storage;
using firebase::storage::StorageReference;


/*
DataUp
a class that need to be initialized once 
its responsable to save && download the files from
the firebase storage
storagerefrence need to be changed to your specifaction
*/

struct Guild{
    std::string Guild_Name,Channel_name;
    dpp::snowflake guild_id,channel_id;
};

struct redeemInfo{
    std::string UserId,userName="user",region="EU";
};

class DataUp{
public:
    DataUp(const DataUp&) = delete;//no copy contructor. bad for me 

    inline static firebase::App * app = nullptr;
    inline static Storage *data = nullptr;
    inline static StorageReference couponfile;
    inline static std::vector<Coupon> CodeStroage;
    inline static std::vector<Guild> GuildStorage;
    inline static std::vector<redeemInfo> local_RedeemInfo;
    DataUp(firebase::App *ap){
        app = ap;
        data = firebase::storage::Storage::GetInstance(app);
        couponfile = data->GetReferenceFromUrl("gs://discord-app-bot.appspot.com/Coupon.json");
        std::cout << "[ DataBase ] construction done for database\n";
    }
    /*
    Download a file and turn it into a string from firebase
    */
    static void saveData(std::shared_ptr<std::string> toSave,StorageReference & SaveTo,std::function<void()> callback = [](){}){
        std::cout << "[ DataBase ] Saving...\n";
        SaveTo.PutBytes(toSave->c_str(),toSave->size()).OnCompletion([toSave,callback](const firebase::Future<firebase::storage::Metadata> & metadata){
            if(metadata.result()->size_bytes() < 0){
                std::cout << "[ DataBase ] FAILED TO SAVE FILE\n";
            }else {
                std::cout << "[ DataBase ] SAVE FILE COMPLETE\n"  << '\n';
                callback();
            }
        });
    }

    /*
    Save string to firebase
    */
    static inline std::string Json_ToView;
    static void Getdata(StorageReference  &Getfrom,std::function<void(std::string*)> callback){
        std::cout << "[ DataBase ] Downloading\n";
        const size_t  bufsize = 1024 * 1024 * 1;
        char buf[bufsize];
        memset(buf, 0, bufsize);
        Getfrom.GetBytes(buf, bufsize).OnCompletion([callback,&buf,&Getfrom](const  firebase::Future<ulong> fsize){
            if(*fsize.result() < 0){
                std::cout << "[ DataBase ] failed to download data\n";
            }else{
                try{
                    std::cout << "[ DataBase ] Downloaded file "<< Getfrom.name() << '\n';
                    std::shared_ptr<std::string> Buffer = std::make_shared<std::string>(std::string(buf));
                    Json_ToView = buf;
                    callback(Buffer.get());
                }catch (std::exception e){
                    std::cout << e.what() << '\n';
                }

            }
            
        });

    }
};


/*
TODO 
Seprate all saveing and downloading
into their function and only modifi one json string

*/

/*
takes a coupon vector and
turn it into a json file 
then uploads it via the 
DataUp::savedata function
*/

inline void Save_cache_to_cloud(std::vector<Coupon>  codes = {} ,std::vector<Guild> guilddata = {},std::vector<redeemInfo> red = {} ){
    std::cout << "[ DataBase ] updataing json file\n";
    nlohmann::json jf;
    
    nlohmann::json datacode;
    std::vector<nlohmann::json> temp;

    for (int i=0; i<DataUp::CodeStroage.size(); i++) {
        datacode["Code"] = DataUp::CodeStroage[i].code;
        datacode["Des"] = DataUp::CodeStroage[i].des;
        datacode["IsNew"] = false;
        temp.push_back(datacode);
    }
    
    for (int i=0; i<codes.size(); i++) {
        bool found = false;

        for(int j=0;j<DataUp::CodeStroage.size();j++){
            if(codes[i].code == DataUp::CodeStroage[j].code){
                found = true;
            }
        }
        if(!found){
            try {
            
                datacode["Code"] = codes[i].code;
                datacode["Des"]  = codes[i].des;
                datacode["IsNew"] = true;
                temp.push_back(datacode);
                
            } catch (std::exception e) {
                
                std::cout << e.what() << '\n';
            }
            
        }
    }
    /*
    turn the guild vector into a json file
    */
    std::cout << "[ DataBase ] Saving GuildData\n";
    guilddata.insert(guilddata.end(),DataUp::GuildStorage.begin(),DataUp::GuildStorage.end());
    std::vector<nlohmann::json> guildjson_list;
    
    for(int i=0;i<guilddata.size();i++){
        nlohmann::json jd;
        jd["id"] = guilddata[i].guild_id;
        jd["name"] = guilddata[i].Guild_Name;
        jd["coupon_channel_id"] = guilddata[i].channel_id;
        jd["coupon_channel_name"] = guilddata[i].Channel_name;
        guildjson_list.push_back(jd);
    }
    


    std::cout << "[ DataBase ] Saving Redeem_Info\n";
    std::vector<nlohmann::json> redeem_info;
    red.insert(red.end(),DataUp::local_RedeemInfo.begin(),DataUp::local_RedeemInfo.end());
    for(int i=0;i<red.size();i++){
        nlohmann::json jd;
        jd["UserId"] = red[i].UserId;
        jd["UserName"] = red[i].userName;
        jd["Region"] = red[i].region;
        redeem_info.push_back(jd);
    }



    try{
        jf["list_code"] = temp;
        jf["size_codes"] = codes.size();
        jf["guilds"] = guildjson_list;
        jf["size_guild"] = guilddata.size();
        jf["redeem_info_list"] = redeem_info;
        jf["redeem_info_size"] = redeem_info.size();
    }catch(std::exception e){
        std::cout << e.what() << '\n';
    }

    std::shared_ptr<std::string> fileData_tosave = std::make_shared<std::string>(jf.dump());
    
    DataUp::saveData(fileData_tosave,DataUp::couponfile);
}




/*

downloads the Coupon file from
the database and pasre it into 
a coupon vector
*/
inline void Update_cache_Storage(std::function<void()> _callback = [](){}){
    std::cout << "[ DataBase ] Updateing local cache\n";    
    std::function<void(std::string*)> callback = [_callback](std::string* CodeJson){
        std::cout << "[ DataBase ] Parsing...\n";
        try {
            nlohmann::json j = nlohmann::json::parse(*CodeJson);
            

            try {
            DataUp::CodeStroage.clear();
            for(int i=0;i<j["list_code"].size();i++){
                Coupon code;
                code.code = j["list_code"][i]["Code"];
                code.des = j["list_code"][i]["Des"];
                code.isNew = j["list_code"][i]["IsNew"];
                DataUp::CodeStroage.push_back(code);
            }
            }catch(std::exception & e){
                std::cout << "[ DataBase ] Code Caching: "<< e.what() << '\n';
                std::cout << "[ DataBase ] Parsing: " << *CodeJson << '\n';
            }


            DataUp::GuildStorage.clear();
            try {
                for(int i=0;i<j["guilds"].size();i++){
                    Guild temp;
                    temp.guild_id = j["guilds"][i]["id"];
                    temp.Guild_Name = j["guilds"][i]["name"];
                    temp.channel_id = j["guilds"][i]["coupon_channel_id"];
                    temp.Channel_name = j["guilds"][i]["coupon_channel_name"];
                    DataUp::GuildStorage.push_back(temp);
                }
            } catch (std::exception e) {
                std::cout << "[ DataBase ] Guild Caching: " << e.what() << '\n';
                std::cout << "[ DataBase ] Parsing: " << *CodeJson << '\n';
            }



            DataUp::local_RedeemInfo.clear();
            try {
                for(int i=0;i<j["redeem_info_list"].size();i++){
                    redeemInfo temp;
                    temp.UserId = j["redeem_info_list"][i]["UserId"];
                    temp.userName = j["redeem_info_list"][i]["UserName"];
                    temp.region = j["redeem_info_list"][i]["Region"];
                    DataUp::local_RedeemInfo.push_back(temp);
                }
            } catch (std::exception e) {
                std::cout << "[ DataBase ] Redeem Info Caching: " << e.what() << '\n';
                std::cout << "[ DataBase ] Parsing: " << *CodeJson << '\n';
            }

        }
        catch (std::exception e) {
            std::cout << e.what() << '\n';
        }

        _callback();
    };
    
    DataUp::Getdata(DataUp::couponfile,callback);
}


/*
update local cache and update database
*/



#endif
