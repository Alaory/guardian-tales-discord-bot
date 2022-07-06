#ifndef database
#define database
#include "dpp/nlohmann/json.hpp"
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

class DataUp{
public:
    DataUp(const DataUp&) = delete;//no copy contructor. bad for me 

    inline static firebase::App * app = nullptr;
    inline static Storage *data = nullptr;
    inline static StorageReference couponfile;
    inline static StorageReference userfile;
    inline static std::vector<Coupon> CodeStroage;

    DataUp(firebase::App *ap){
        app = ap;
        data = firebase::storage::Storage::GetInstance(app);
        couponfile = data->GetReferenceFromUrl("gs://discord-app-bot.appspot.com/Coupon.json");
        userfile = data->GetReferenceFromUrl("gs://discord-app-bot.appspot.com/User.json");
        std::cout << "[ DataBase ] construction done for database\n";
    }


    static void saveData(std::shared_ptr<std::string> &toSave,StorageReference & SaveTo){
        std::cout << "[ DataBase ] Saving...\n";
        SaveTo.PutBytes(toSave->c_str(),toSave->size()).OnCompletion([toSave](const firebase::Future<firebase::storage::Metadata> & metadata){
            std::weak_ptr<std::string> weakme = toSave;
            if(metadata.result()->size_bytes() < 0){
                std::cout << "[ DataBase ] FAILED TO SAVE FILE\n";
            }else {
                std::cout << "[ DataBase ] SAVE FILE COMPLETE\n";
            }
        });
    }



    static void Getdata(std::string & SaveFrom,StorageReference & Getfrom){
        size_t bufsize = 1024 * 1024 * 1;
        char buf[bufsize];
        Getfrom.GetBytes(buf, bufsize).OnCompletion([&](const  firebase::Future<ulong> fsize){
            if(*fsize.result() < 0){
                std::cout << "[ DataBase ] failed to get data\n";
            }else{
                std::cout << "[ DataBase ] Downloaded file "<< Getfrom.name() << " completed\n";
                SaveFrom = buf;
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
    std::string CodeJson;

    DataUp::Getdata(CodeJson,DataUp::couponfile);

    DataUp::CodeStroage.clear();
    
    for(int i=0;i<3;i++){ 

    if(CodeJson.length() <=0){
        std::this_thread::sleep_for(std::chrono::seconds(4));//this is bad very bad. i got no other idea of how to solve it.if you do well... TELL ME. i am NOT asking 
    }

    try {
        nlohmann::json j = nlohmann::json::parse(CodeJson);
        int size = j["size"];
        
        for(int i=0;i<size;i++){
            Coupon code;
            code.code = j["list"][i]["Code"];
            code.des = j["list"][i]["Des"];
            code.isNew = j["list"][i]["IsNew"];
            DataUp::CodeStroage.push_back(code);
        }

        break;

    }catch(std::exception & e){
        
        std::cout << e.what() << '\n';

    }}
}

#endif
