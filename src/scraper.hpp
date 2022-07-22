#ifndef LOG
#define LOG(STR)  std::cout << STR
#endif
#ifndef scraper
#define scraper

#include "dpp/utility.h"
#include <dpp/nlohmann/json.hpp>
#include <chrono>
#include <memory>
#include <string>
#include <httplib.h>
#include <myhtml/api.h>
#include <vector>

struct Coupon{
    std::string code,des;
    bool isNew = false;
};

/*
add table scraper
add list scraper
*/

namespace ScrapeTag{
    /*
        @breif parse a ul tag into a vector string list
    */
    typedef std::vector<std::string> UL;
    typedef std::vector<UL> TABLE;





    inline UL UL_scraper(myhtml_tree_node_t * ul){
        myhtml_tree_node_t * IndexNode = myhtml_node_child(ul);
        myhtml_tree_node_t * TempIndexNode = IndexNode;
        UL ul_Text = {};
        while (IndexNode) {
            const char * Text = myhtml_node_text(TempIndexNode, nullptr);
            if(!Text){
                TempIndexNode = myhtml_node_child(TempIndexNode);
                continue;
            }
            ul_Text.push_back(std::string(Text));
            std::cout << "[ Scraper::UL_scraper ] I got: " << Text << '\n';
            IndexNode = myhtml_node_next(IndexNode);
            if(!IndexNode)
                break;
            TempIndexNode = IndexNode;
        }
        return ul_Text;
    }








    /*
    Parse a table tag from html into a vector list
    */
    inline TABLE Table_scraper(myhtml_tree_node_t * table){
        LOG("[ Scraper::Table_scraper ] Tag numbered: " << myhtml_node_tag_id(table) << '\n');
        if(myhtml_node_tag_id(table) != MyHTML_TAG_TABLE){
            std::cout << "[ Scraper::Table_scraper ] We Got: " << myhtml_node_tag_id(table) << " we need table: " << MyHTML_TAG_TABLE << '\n';
            return {};
        }

        myhtml_tree_node_t * IndexNode_P = myhtml_node_child(table);
        myhtml_tree_node_t * IndexNode_C = myhtml_node_child(IndexNode_P);
        myhtml_tree_node_t * IndexNode_T = IndexNode_C;
        

        TABLE Table;
        
        while (IndexNode_P) {
            std::vector<std::string> Row;
            while (IndexNode_C){
                LOG("[ Scraper::Table_scraper ] Parent:" <<
                    myhtml_node_tag_id(IndexNode_P) <<
                    " Child: " << myhtml_node_tag_id(IndexNode_C) <<
                    " Temp: " << myhtml_node_tag_id(IndexNode_T) << '\n');
                

                const char * Text = myhtml_node_text(IndexNode_C, nullptr);

                if(!Text){
                    IndexNode_C = myhtml_node_child(IndexNode_C);
                    continue;
                }
                
                if(strlen(Text) > 0){
                    Row.push_back(std::string(Text));
                    LOG("[ Scraper::Table_scraper ] I got: " << Row[Row.size()-1] << '\n');
                }


                if (myhtml_node_next(IndexNode_C))
                    IndexNode_C = myhtml_node_next(IndexNode_C);
                else if(myhtml_node_parent(IndexNode_C)!= IndexNode_P){
                    LOG("[ Scraper::Table_scraper ] Next Sibling: " << myhtml_node_tag_id(IndexNode_C) << '\n');
                    IndexNode_C = myhtml_node_next(myhtml_node_parent(IndexNode_C));
                    continue;
                }
                
            }

            
            Table.push_back(Row);
            //LOG("[ Scraper::Table_scraper ] Row Pushed\n");
            
                

            IndexNode_P = myhtml_node_next(IndexNode_P);
            
            if(!IndexNode_P)
                continue;
            IndexNode_C = myhtml_node_child(IndexNode_P);
            IndexNode_T = IndexNode_C;
            
        }
    
    LOG("[ Scraper::Table_scraper ] List Size:"<<Table.size()<< " and :" << Table[0].size() <<'\n');
    return Table;
    }



}














class website{
    std::string Url,UrlPath,TagName,Page;
    int TagId;
    myhtml_t *context;
    myhtml_tree_t * treeBody;
    std::vector<Coupon> Codes;
    httplib::Client * cli;
    public:
    website(const std::string &url,const std::string &WebPath,const std::string TagName,const int TagId){
        
        this->Url = url;
        this->UrlPath = WebPath;
        this->TagName = TagName;
        this->TagId = TagId;
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
        std::cout << "[ scraper ] Scarping  Codes\n";
        Codes.clear();
        myhtml_tree_clean(treeBody);
        Page = cli->Get(UrlPath.c_str())->body;
        myhtml_parse(treeBody, MyENCODING_UTF_8, Page.c_str() , Page.size());
        myhtml_collection_t * TagColl = myhtml_get_nodes_by_tag_id(treeBody, nullptr, TagId, nullptr);
        
        for (int i=0; i < TagColl->length; i++) {
            
            std::string IDname(myhtml_attribute_value(myhtml_node_attribute_first(TagColl->list[i]), nullptr));
            if(IDname == TagName){
                
                if (TagId == MyHTML_TAG_FIGURE) {
                    //access html tags
                    myhtml_tree_t * tree_figure = myhtml_node_tree(TagColl->list[i]);

                    // ScrapeTag::TABLE table = ScrapeTag::Table_scraper(myhtml_node_child(TagColl->list[i]));
                    // for (int i=0; i<table.size(); i++) {
                    //     std::cout << "[ Scraper ] Table: " << i << " first element: " << table[i][0] << '\n';
                    // }

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