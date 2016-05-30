#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "network/SocketIO.h"
#include "LolWars.h"

#define ROUND_WON "ROUND_WIN"
#define ROUND_LOST "ROUND_LOST"
#define ROUND_DRAW "ROUND_DRAW"
#define GAME_START "GAME_START"
#define ROUND_RESULT "ROUND_RESULT"
#define OPPONENT_DC "OPPONENT_DC"

USING_NS_CC;
using namespace network;

class HelloWorld : public cocos2d::Layer, public cocos2d::network::SocketIO::SIODelegate
{
    LolWars *lolWars;
    
public:
    
    //gameplay related stuff. This data will be picked by renderer.
    int roundIndx;
    bool myTurn;
    int myIndex;
    int enemyIndex;
    rapidjson::Document matchData;
    rapidjson::Document userObj;
    int roundResults[10];
    
    cocos2d::network::SIOClient *_sioClient, *_sioEndpoint;

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    virtual void onConnect(SIOClient* client);
    virtual void onMessage(SIOClient* client, const std::string& data);
    virtual void onClose(SIOClient* client);
    virtual void onError(SIOClient* client, const std::string& data);
    
    void sendEvent(Ref *pSender);
    void setkey(SIOClient *client, const std::string& data);
    void matchmade(SIOClient *client, const std::string& data);
    void moveEvent(SIOClient *client, const std::string& data);
    void opponentDC(SIOClient *client, const std::string& data);

    //ui callback.
    void statClicked(int _id);
    void notifyRoundResult(int _id);
    const rapidjson::Value & getCurrentCard(bool forEnemy = false);
    
    //this function will give you game result and will also trigger game over.
    int getGameResult();
    
    void askForMatchMake();

    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
};

#endif // __HELLOWORLD_SCENE_H__
