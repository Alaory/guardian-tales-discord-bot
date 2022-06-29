#ifndef database
#define database
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
    inline static firebase::App * app = nullptr;
    inline static Storage *data = nullptr;
    inline static StorageReference couponfile;
    inline static StorageReference userfile;
    inline static std::vector<Coupon> CodeStroage;

    DataUp(firebase::App *ap){
        app = ap;
        data = firebase::storage::Storage::GetInstance(app);
        couponfile = data->GetReferenceFromUrl("gs://discord-app-bot.appspot.com/hello.txt");
        userfile = data->GetReferenceFromUrl("gs://discord-app-bot.appspot.com/User.json");
        std::cout << "construction done for database\n";
    }


    static void saveData(std::string & toSave,StorageReference & SaveTo){
        //std::cout << fileData_tosave << '\n';
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






inline void UpdateSaveCodeToJson(std::vector<Coupon> & codes){
    std::cout << "updataing json file\n";
    nlohmann::json jf;
    jf["size"] = codes.size();
    std::string fileData_tosave;
    
    nlohmann::json datacode;
    for(int i=0;i<codes.size();i++){
        bool found = false;
        datacode["Code"] = codes[i].code;
        datacode["Dis"] = codes[i].des;
        for(int i=0;i<DataUp::CodeStroage.size();i++){
            if(DataUp::CodeStroage[i].code == codes[i].code){
                found = true;
                break;
            }
        }
        if(!found)
            jf["list"].push_back(datacode);
    }
        
    fileData_tosave = jf.dump();
    DataUp::saveData(fileData_tosave,DataUp::couponfile);
}





inline void UpdateCodeFromJson(){
    std::string CodeJson;
    DataUp::Getdata(CodeJson,DataUp::couponfile);
    DataUp::CodeStroage.clear();
    for(int i=0;i<3;i++){ 
    if(CodeJson.length() <=0){
        std::this_thread::sleep_for(std::chrono::seconds(4));//this is bad very bad. i got no other idea of how to solve it.if you do well tell me. i am NOT asking 
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