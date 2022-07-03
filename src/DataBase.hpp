#ifndef database
#define database
#include "dpp/nlohmann/json.hpp"
#include "scraper.hpp"
#include <chrono>
#include <exception>
#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <firebase/app.h>
#include <firebase/storage.h>

using firebase::storage::Storage;
using firebase::storage::StorageReference;


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
        std::cout << "construction done for database\n";
    }


    static void saveData(std::string toSave,StorageReference & SaveTo){
        SaveTo.PutBytes(toSave.c_str(),toSave.size()).OnCompletion([](const firebase::Future<firebase::storage::Metadata> & metadata){
            if(metadata.result()->size_bytes() < 0){
                std::cout << "FAILED TO SAVE FILE\n";
            }else {
                std::cout << "SAVE FILE COMPLETE\n";
            }
        });
    }



    static void Getdata(std::string & SaveTo,StorageReference & Getfrom){
        size_t bufsize = 1024 * 1024 * 1;
        char buf[bufsize];
        Getfrom.GetBytes(buf, bufsize).OnCompletion([&](const  firebase::Future<ulong> fsize){
            if(*fsize.result() < 0){
                std::cout << "failed to get data\n";
            }else{
                std::cout << "Done collecting codes\n";
                SaveTo = buf;
            }
        });   

    }




    ~DataUp(){

    }
};



/*
takes a coupon vector and
turn it into a json file 
then uploads it via the 
DataUp::savedata function
*/
inline void SaveCouponCodes_toCloud(std::vector<Coupon> & codes){
    std::cout << "updataing json file\n";
    nlohmann::json jf;
    std::string fileData_tosave;
    nlohmann::json datacode;
    std::vector<nlohmann::json> temp;

    for (int i=0; i<DataUp::CodeStroage.size(); i++) {
        datacode["Code"] = DataUp::CodeStroage[i].code;
        datacode["Des"] = DataUp::CodeStroage[i].des;
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

        
    fileData_tosave = jf.dump();
    
    DataUp::saveData(fileData_tosave,DataUp::couponfile);
}




/*

downloads the Coupon file from
the database and pasre it into 
a coupon vector

*/
inline void UpdateCodeFromJson(){
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
            code.des = j["list"][i]["Dis"];
            DataUp::CodeStroage.push_back(code);
        }

        break;

    }catch(std::exception & e){
        
        std::cout << e.what() << '\n';

    }}
}

#endif
