#ifndef database
#define database
#include "dpp/nlohmann/json_fwd.hpp"
#include "scraper.hpp"
#include <iostream>
#include <vector>
#include <functional>
#include <firebase/app.h>
#include <firebase/storage.h>
#include <dpp/nlohmann/json.hpp>
using firebase::storage::Storage;
using firebase::storage::StorageReference;


class DataUp{
public:
    firebase::App * app = nullptr;
    Storage *data = nullptr;
    StorageReference  couponfile;
    std::string fileData_tosave = "";

    DataUp(firebase::App *ap){
        app = ap;
        data = firebase::storage::Storage::GetInstance(app);
        couponfile = data->GetReferenceFromUrl("gs://discord-app-bot.appspot.com/hello.txt");
        std::cout << "construction done\n";
    }


    void saveData(std::vector<Coupon>& codes){
        std::cout << "data ready\n";
        fileData_tosave.clear();
        



        nlohmann::json jf;
        jf["size"] = codes.size();
        
        
        nlohmann::json datacode;
        for(int i=0;i<codes.size();i++){
            datacode["Code"] = codes[i].code;
            datacode["Dis"] = codes[i].des;
            jf["list"].push_back(datacode);
        }
       
        
        fileData_tosave = jf.dump();

        //std::cout << fileData_tosave << '\n';
        couponfile.PutBytes(fileData_tosave.c_str(),fileData_tosave.size()).OnCompletion([](const firebase::Future<firebase::storage::Metadata> & metadata){
            if(metadata.result()->size_bytes() < 0){
                std::cout << "FAILED TO SAVE FILE\n";
            }else {
                std::cout << "SAVE FILE COMPLETE\n";
            }
        });
    }



    void  Getdata(std::vector<std::string> & codedata){
        size_t bufsize = 1024 * 1024 * 1;
        char buf[bufsize];
        couponfile.GetBytes(buf, bufsize).OnCompletion([&](const  firebase::Future<ulong> fsize){
            if(*fsize.result() == 0){
                std::cout << "no data was found in the file";
            }
            std::string temp = "";
            for(int i=0;i<*fsize.result();i++){
                if(buf[i] == '\n'){
                    codedata.push_back(temp);
                    temp.clear();
                }else {
                    temp += buf[i];
                }
            }
            
        });
        std::cout << "Done collecting codes\n";
    }

    
    ~DataUp(){

    }
};

#endif