//
//  RiotAPI.hpp
//  NetTest
//
//  Created by vamsi krishna on 29/10/15.
//
//

#ifndef RiotAPI_hpp
#define RiotAPI_hpp

#include <stdio.h>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "network/HttpClient.h"
#include "json/Document.h"
#include "json/PrettyWriter.h"
#include "json/Stringbuffer.h"

USING_NS_CC;
USING_NS_CC_EXT;

#define CHAMP_KEY   "championId"
#define STATS_KEY   "stats"
#define DEATHS_KEY  "numDeaths"
#define KILLS_KEY   "championsKilled"
#define ASSISTS_KEY "assists"
#define RESULT_KEY  "win"
#define TIME_KEY    "timePlayed"
#define TURRENTS_KEY    "turretsKilled"


#define SUMMONER_ID_FETCHED "SUMMONER_ID"
#define SUMMONER_GAMES_FETCHED "SUMMONER_GAMES"

using namespace cocos2d::network;

class RiotAPI
{
    
    static RiotAPI *_instance;

    std::string base_url;
    std::string summoner_by_name;
    std::string summoner_by_id;
    std::string summoner_recent_games;
    std::string api_key;
    
    std::function<void(std::string _id, std::string _data)> callback;
    
    RiotAPI(){
        callback = nullptr;
        api_key = "";
    }
    
public:
    void setCallback( std::function<void(std::string _id, std::string _data)> cb) {callback = cb;}
    void setApiKey(std::string key) { api_key = key; }
    
    bool initUrls(std::string server);
    
    void fetchID(std::string summonerName);
    void fetchGames(int limit = 10);
    
    void responseHandler(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    
    static std::string getSummonerObject();
    static RiotAPI * getInstance();
};

#endif /* RiotAPI_hpp */
