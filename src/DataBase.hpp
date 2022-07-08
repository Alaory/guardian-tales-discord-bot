#ifndef database
#define database
#include "dpp/nlohmann/json.hpp"
#include "dpp/snowflake.h"
#include "scraper.hpp"
#include <chrono>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
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


class DataUp{
public:
    DataUp(const DataUp&) = delete;//no copy contructor. bad for me 

    inline static firebase::App * app = nullptr;
    inline static Storage *data = nullptr;
    inline static StorageReference couponfile;
    inline static StorageReference userfile;
    inline static StorageReference guildfile;
    inline static std::vector<Coupon> CodeStroage;
    inline static std::vector<Guild> GuildStorage;
    inline static size_t bufsize = 1024 * 1024 * 1;
    inline static char *buf = new char[bufsize];
    DataUp(firebase::App *ap){
        app = ap;
        data = firebase::storage::Storage::GetInstance(app);
        couponfile = data->GetReferenceFromUrl("gs://discord-app-bot.appspot.com/Coupon.json");
        userfile = data->GetReferenceFromUrl("gs://discord-app-bot.appspot.com/User.json");
        guildfile = data->GetReferenceFromUrl("gs://discord-app-bot.appspot.com/Guild.json");
        std::cout << "[ DataBase ] construction done for database\n";
    }

    static void saveData(std::shared_ptr<std::string> &toSave,StorageReference & SaveTo,std::function<void()> callback = [](){}){
        std::cout << "[ DataBase ] Saving...\n";
        SaveTo.PutBytes(toSave->c_str(),toSave->size()).OnCompletion([toSave,callback](const firebase::Future<firebase::storage::Metadata> & metadata){
            if(metadata.result()->size_bytes() < 0){
                std::cout << "[ DataBase ] FAILED TO SAVE FILE\n";
            }else {
                std::cout << "[ DataBase ] SAVE FILE COMPLETE\n";
                callback();
            }
        });
    }



    static void Getdata(StorageReference & Getfrom,std::function<void(std::string)> callback){
        std::cout << "[ DataBase ] Downloading\n";
        Getfrom.GetBytes(buf, bufsize).OnCompletion([&](const  firebase::Future<ulong> fsize){
            if(*fsize.result() < 0){
                std::cout << "[ DataBase ] failed to download data\n";
            }else{
                try{
                    std::cout << "[ DataBase ] Downloaded file "<< Getfrom.name() << '\n';
                    std::string Buffer(buf);
                    callback(Buffer);
                }catch (std::exception e){
                    std::cout << e.what() << '\n';
                }
            }
        });   

    }
};



/*
takes a coupon vector and
turn it into a json file 
then uploads it via the 
DataUp::savedata function
*/

inline void SaveCouponCodes_toCloud(std::vector<Coupon> & codes){
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

    try{
        jf["list"] = temp;
        jf["size"] = codes.size();
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
inline void UpdateCodeFromJson(){
    std::cout << "[ DataBase ] Updateing local cache\n";    
    std::function<void(std::string)> callback = [](std::string CodeJson){
        try {
        DataUp::CodeStroage.clear();
        nlohmann::json j = nlohmann::json::parse(CodeJson);
        int size = j["size"];
        for(int i=0;i<size;i++){
            Coupon code;
            code.code = j["list"][i]["Code"];
            code.des = j["list"][i]["Des"];
            code.isNew = j["list"][i]["IsNew"];
            DataUp::CodeStroage.push_back(code);
        }
        }catch(std::exception & e){
            std::cout << e.what() << '\n';
        }
    };
    DataUp::Getdata(DataUp::couponfile,callback);
}

inline void SaveGuildData(std::vector<Guild> guilddata){
    std::cout << "[ DataBase ] Saving GuildData\n";
    nlohmann::json j;
    guilddata.insert(guilddata.end(),DataUp::GuildStorage.begin(),DataUp::GuildStorage.end());
    j["size"] = guilddata.size();
    std::vector<nlohmann::json> guildjson_list;
    for(int i=0;i<guilddata.size();i++){
        nlohmann::json jd;
        jd["id"] = guilddata[i].guild_id;
        jd["name"] = guilddata[i].Guild_Name;
        jd["coupon_channel_id"] = guilddata[i].channel_id;
        jd["coupon_channel_name"] = guilddata[i].Channel_name;
        guildjson_list.push_back(jd);
    }
    j["guilds"] = guildjson_list;
    std::shared_ptr<std::string> data = std::make_shared<std::string>(j.dump());
    DataUp::saveData(data,DataUp::guildfile);//might lose data if we call them together
}

inline void GetGuildData_toLocal(){
    std::cout << "[ DataBase ] caching GuildData\n";
    std::function<void(std::string)> callback = [](std::string GuildJson){
        std::cout << "[ DataBase ] CallBack for GuildData fun\n";
        DataUp::GuildStorage.clear();
        nlohmann::json gjs = nlohmann::json::parse(GuildJson);
        try {
            for(int i=0;i<gjs["guilds"].size();i++){
                Guild temp;
                temp.guild_id = gjs["guilds"][i]["id"];
                temp.Guild_Name = gjs["guilds"][i]["name"];
                temp.channel_id = gjs["guilds"][i]["coupon_channel_id"];
                temp.Channel_name = gjs["guilds"][i]["coupon_channel_name"];
                DataUp::GuildStorage.push_back(temp);
            }
        } catch (std::exception e) {
            std::cout << e.what() << '\n';
        }
    };

    DataUp::Getdata(DataUp::guildfile,callback);
}




#endif
