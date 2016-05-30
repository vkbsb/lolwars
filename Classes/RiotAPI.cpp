//
//  RiotAPI.cpp
//  NetTest
//
//  Created by vamsi krishna on 29/10/15.
//
//

#include "RiotAPI.hpp"

RiotAPI* RiotAPI::_instance = nullptr;

std::string RiotAPI::getSummonerObject()
{
    std::string summonerInfo = UserDefault::getInstance()->getStringForKey(SUMMONER_ID_FETCHED, "");
    if(summonerInfo.empty()){
        log("Summoner info not present");
        return "";
    }
    
    rapidjson::Document doc;
    doc.Parse<0>(summonerInfo.c_str());
    rapidjson::Value::MemberIterator M;
    M = doc.MemberonBegin();
    
    const char* summonerName = M->name.GetString();
    rapidjson::Value &summonerObject = doc[summonerName];
    
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    summonerObject.Accept(writer);
    const char* json = buffer.GetString();
    return json;
}

RiotAPI* RiotAPI::getInstance(){
    if(_instance == nullptr){
        _instance = new RiotAPI();
    }
    return _instance;
}

bool RiotAPI::initUrls( std::string server)
{
    api_key = "9a08a263-25a2-4d97-b2a6-fe4eb6021fcb";
    base_url = "https://" + server + ".api.pvp.net";
    summoner_by_name = base_url + "/api/lol/" + server + "/v1.4/summoner/by-name/";
    summoner_by_id = base_url +"/api/lol/"+ server + "/v1.4/summoner/"; //{summonerIds};
    summoner_recent_games = base_url + "/api/lol/"+ server + "/v1.3/game/by-summoner/";  //{summonerId}/rec
    
    return true;
}


void RiotAPI::fetchID(std::string summonerName)
{
    log("+RiotAPI::fetchID");
    
    if(api_key.empty()){
        log("Plese set api key");
        return;
    }
    
    std::string summonerInfo = UserDefault::getInstance()->getStringForKey(SUMMONER_ID_FETCHED, "");
    if(summonerInfo.empty() == false){
        rapidjson::Document doc;
        doc.Parse<0>(summonerInfo.c_str());
        
        if(doc.HasParseError()){
            log("Parse Error");
            goto FETCH_SUMMONERINFO;
        }
        
        if (doc.HasMember(summonerName.c_str())) {
            if(callback){
                callback(SUMMONER_ID_FETCHED, summonerInfo);
            }
            log("-RiotAPI::fetchID");
            return;
        }
    }
    
FETCH_SUMMONERINFO:
    HttpRequest* request = new (std::nothrow) HttpRequest();
    
    // required fields
    request->setUrl((summoner_by_name + summonerName + "?api_key=" + api_key).c_str());
    request->setRequestType(HttpRequest::Type::GET);
    request->setResponseCallback(CC_CALLBACK_2(RiotAPI::responseHandler, this));
    request->setTag(SUMMONER_ID_FETCHED);
    HttpClient::getInstance()->send(request);
    
    log("-RiotAPI::fetchID");
}

void RiotAPI::fetchGames(int limit)
{
    log("+RiotAPI::fetchGames");
    
    if(api_key.empty()){
        log("Plese set api key");
        return;
    }
    
    HttpRequest* request = new (std::nothrow) HttpRequest();
    
    std::string summonerId = "";
    
    std::string summonerInfo = UserDefault::getInstance()->getStringForKey(SUMMONER_ID_FETCHED, "");
    if(summonerInfo.empty() == false){
        rapidjson::Document doc;
        doc.Parse<0>(summonerInfo.c_str());
        rapidjson::Value::MemberIterator M;
        M = doc.MemberonBegin();
        
        const char* summonerName = M->name.GetString();
        rapidjson::Value &summonerInfo = doc[summonerName];
        log("%s", summonerName);

        if(summonerInfo.HasMember("id")){
            summonerId = StringUtils::format("%lld", summonerInfo["id"].GetInt64());
        }else{
            log("Couldn't find id");
        }

    }else{
        log("No summoner info found");
        return;
    }
    
    // required fields
    request->setUrl((summoner_recent_games + summonerId + "/recent?api_key=" + api_key).c_str());
    request->setRequestType(HttpRequest::Type::GET);
    request->setResponseCallback(CC_CALLBACK_2(RiotAPI::responseHandler, this));
    request->setTag(SUMMONER_GAMES_FETCHED);
    HttpClient::getInstance()->send(request);
    
    log("-RiotAPI::fetchGames");
}

void RiotAPI::responseHandler(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    if (!response->isSucceed())
    {
        log("response failed");
        log("error buffer: %s", response->getErrorBuffer());
        return;
    }
    
    // dump data
    std::vector<char> *buffer = response->getResponseData();
    char *ptr = new char[buffer->size()+1];
    
    memcpy(ptr, buffer->data(), buffer->size());
    
    ptr[buffer->size()] = '\0';
    
    std::string reqTag = response->getHttpRequest()->getTag();
    
    if(reqTag ==  SUMMONER_ID_FETCHED){
        UserDefault::getInstance()->setStringForKey(SUMMONER_ID_FETCHED, ptr);
        UserDefault::getInstance()->flush();
    }else if(reqTag == SUMMONER_GAMES_FETCHED){
        
    }
    
    //callback for requests finished.
    if(callback != nullptr){
        callback(reqTag, ptr);
    }
    
    delete [] ptr;
}