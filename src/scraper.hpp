#ifndef scraper
#define scraper
#include "dpp/utility.h"
#include <dpp/nlohmann/json.hpp>
#include <chrono>
#include <string>
#include <httplib.h>
#include <myhtml/api.h>

struct Coupon{
    std::string code,des;
    bool isNew = false;
};

class website{
    std::string Url,UrlPath,TagName,Page;
    int TagNum;
    myhtml_t *context;
    myhtml_tree_t * treeBody;
    std::vector<Coupon> Codes;
    httplib::Client * cli;
    public:
    website(const std::string &url,const std::string &WebPath,const std::string TagName,const int TagNum){
        
        this->Url = url;
        this->UrlPath = WebPath;
        this->TagName = TagName;
        this->TagNum = TagNum;
        //init myhtml 
        context = myhtml_create(); 
        myhtml_init(context, MyHTML_OPTIONS_DEFAULT, 1, 0);
        treeBody = myhtml_tree_create();
        myhtml_tree_init(treeBody, context);

        //init httplib
        cli = new httplib::Client(url);
    }


    // TODO 
    // make function to search for the data INSTED of looking specificly for it 
    // optional 
    // add auto recive codes

    std::vector<Coupon>& scrap_codes(){
        Codes.clear();
        myhtml_tree_clean(treeBody);
        Page = cli->Get(UrlPath.c_str())->body;
        myhtml_parse(treeBody, MyENCODING_UTF_8, Page.c_str() , Page.size());
        myhtml_collection_t * TagColl = myhtml_get_nodes_by_tag_id(treeBody, nullptr, TagNum, nullptr);
        
        for (int i=0; i < TagColl->length; i++) {
            
            std::string tname(myhtml_attribute_value(myhtml_node_attribute_first(TagColl->list[i]), nullptr));
            if(tname == TagName){
                
                if (TagNum == MyHTML_TAG_FIGURE) {
                    //access html tags
                    myhtml_tree_t * tree_figure = myhtml_node_tree(TagColl->list[i]);
                    myhtml_collection_t * TagTr = myhtml_get_nodes_by_tag_id(tree_figure, nullptr, MyHTML_TAG_TR, nullptr);

                    for(int j=0;j<TagTr->length;j++){


                        myhtml_tree_node_t * node = TagTr->list[j];
                        const char * code = nullptr;

                        while (true) {
                            code = myhtml_node_text(node,nullptr);
                            if(code == nullptr){
                                node = myhtml_node_child(node);
                                continue;
                            }
                            break;
                        }

                        Coupon Code;
                        Code.code = code;
                        if(Code.code.find(' ') != std::string::npos){
                            continue;
                        }
                        Code.des = dpp::utility::current_date_time();
                        Codes.push_back(Code);
                    }

                    myhtml_collection_destroy(TagTr);
                    break;
                }
            }
        }

        myhtml_collection_destroy(TagColl);
        return Codes;
    }

    ~website(){
        delete cli;
        myhtml_tree_destroy(treeBody);
        myhtml_destroy(context);
    }

};
#endif