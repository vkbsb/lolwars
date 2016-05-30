#include "HelloWorldScene.h"
#include "RiotAPI.hpp"

USING_NS_CC;

char *CardKeys[] = {"k", "d", "a", "w", "tu", "ti"};

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

void HelloWorld::onConnect(SIOClient* client)
{
    log("onConnect");
}

void HelloWorld::onMessage(SIOClient* client, const std::string& data)
{
    log("onMessage: %s", data.c_str());
}

void HelloWorld::onClose(SIOClient* client)
{
    log("onClose");
}

void HelloWorld::onError(SIOClient* client, const std::string& data)
{
    log("onError: %s", data.c_str());
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    
    lolWars = LolWars::getNewInstance();
    addChild(lolWars);
    
    
//    //create a client by using this static method, url does not need to contain the protocol
//    _sioClient = SocketIO::connect("http://127.0.0.1:8080", *this);
    _sioClient = SocketIO::connect("http://ec2-54-254-232-38.ap-southeast-1.compute.amazonaws.com:8080", *this);
    
   
//    //you may set a tag for the client for reference in callbacks
    _sioClient->setTag("lol-wars");

//register event callbacks using the CC_CALLBACK_2() macro and passing the instance of the target class
    _sioClient->on("move", CC_CALLBACK_2(HelloWorld::moveEvent, this));
    _sioClient->on("matchmade", CC_CALLBACK_2(HelloWorld::matchmade, this));
    _sioClient->on("setkey", CC_CALLBACK_2(HelloWorld::setkey, this));
    _sioClient->on("dc", CC_CALLBACK_2(HelloWorld::opponentDC, this));
    
    RiotAPI::getInstance()->initUrls("euw");
    
    RiotAPI::getInstance()->setCallback([=](std::string _id, std::string _data){
        if(_id == SUMMONER_ID_FETCHED){
            RiotAPI::getInstance()->fetchGames();
        }else if(_id == SUMMONER_GAMES_FETCHED){
            rapidjson::Document doc;
            doc.Parse<0>(_data.c_str());
//            log("%s", _data.c_str());
            
            if(doc.HasMember("games")){
                //create the user object to send for matchmake.
                userObj.Parse<0>(RiotAPI::getSummonerObject().c_str());
                
                rapidjson::Value cards(rapidjson::kArrayType);
                rapidjson::Document::AllocatorType& allocator = userObj.GetAllocator();

                //iterrate through all the games.
                rapidjson::Value &games = doc["games"];
                
                for(rapidjson::SizeType i = 0; i < games.Size(); i++){
                    rapidjson::Value &game = games[i];
                    int champid = game[CHAMP_KEY].GetInt();
                    
                    rapidjson::Value &stats = game[STATS_KEY];
                    int k = 0, d = 0, a = 0, win = 0, tu = 0, ti=10;
                    
                    if(stats.HasMember(RESULT_KEY) && stats[RESULT_KEY].GetBool()){
                        win = 1;
                    }
                    
                    if(stats.HasMember(KILLS_KEY)){
                        k = stats[KILLS_KEY].GetInt();
                    }
                    
                    if(stats.HasMember(DEATHS_KEY)){
                        d = stats[DEATHS_KEY].GetInt();
                    }
                    
                    if(stats.HasMember(ASSISTS_KEY)){
                        a = stats[ASSISTS_KEY].GetInt();
                    }
                    
                    if(stats.HasMember(TURRENTS_KEY)){
                        tu = stats[TURRENTS_KEY].GetInt();
                    }
                    
                    //create a card json with the stats.
                    rapidjson::Value card(rapidjson::kObjectType);
                    card.AddMember("k", k, allocator);
                    card.AddMember("d", d, allocator);
                    card.AddMember("a", a, allocator);
                    card.AddMember("c", champid, allocator);
                    card.AddMember("w", win, allocator);
                    card.AddMember("tu", tu, allocator);
                    card.AddMember("ti", tu, allocator);
                    
                    //add the card to the list of cards.
                    cards.PushBack(card, allocator);
                }
                
                userObj.AddMember("cards", cards, allocator);
                
                //ask for matchmake.
                askForMatchMake();
            }
        }
    });

    return true;
}

void HelloWorld::askForMatchMake()
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    userObj.Accept(writer);
    const char* json = buffer.GetString();
    //                log("%s", json);
    
    log("Asking for matchMake");
    
    memset(roundResults, 0, 10 * sizeof(int));
    
    //we got the cards now send the cards across and establish stuff.
    _sioClient->emit("matchmake", json);
}

void HelloWorld::sendEvent(Ref *pSender)
{
    _sioClient->emit("msg", "{test: 10}");
}

void HelloWorld::opponentDC(SIOClient *client, const std::string& data)
{
    CCNotificationCenter::getInstance()->postNotification(OPPONENT_DC);
}

void HelloWorld::setkey(SIOClient *client, const std::string& data)
{
    rapidjson::Document doc;
    doc.Parse<0>(data.c_str());
    std::string key = doc["ak"].GetString();
    
    RiotAPI::getInstance()->setApiKey(key);
    
    //RiotAPI::getInstance()->fetchID("gizmoforever");
    lolWars->playButton->setEnabled(true);
}

void HelloWorld::matchmade(SIOClient *client, const std::string& data)
{
    log("onMatchMade: %s", data.c_str());
    
    this->matchData.Parse<0>(data.c_str());
    
    this->roundIndx = 0;
    
    //determine who's turn it is.
    const rapidjson::Value &players = matchData["p"];
    
    if(players.IsArray()){
        const rapidjson::Value &firstPlayer = players[rapidjson::SizeType(0)];
        
        if(firstPlayer["id"].GetInt64() == userObj["id"].GetInt64()){
            log("You are player One");
            myTurn = true;
            myIndex = 0;
            enemyIndex = 1;
        }else{
            log("You are player Two");
            myTurn = false;
            myIndex = 1;
            enemyIndex = 0;
        }
    }
    
    CCNotificationCenter::getInstance()->postNotification(GAME_START, CCString::create(myTurn? "MINE" : "THEIR"));
}

const rapidjson::Value & HelloWorld::getCurrentCard(bool forEnemy)
{
    const rapidjson::Value &players = matchData["p"];
    const rapidjson::Value &cardOrder = matchData["s"];
    
    const rapidjson::Value &myCards = players[rapidjson::SizeType(myIndex)]["cards"];
    const rapidjson::Value &enemyCards = players[rapidjson::SizeType(enemyIndex)]["cards"];
    
    int index = cardOrder[rapidjson::SizeType(roundIndx)].GetInt();
    
    const rapidjson::Value &myCard = myCards[rapidjson::SizeType(index)];
    const rapidjson::Value &enemyCard = enemyCards[rapidjson::SizeType(index)];
    
    if(forEnemy){
        return enemyCard;
    }else{
        return myCard;
    }
}

void HelloWorld::notifyRoundResult(int id)
{
    const rapidjson::Value &players = matchData["p"];
    const rapidjson::Value &cardOrder = matchData["s"];
    
    const rapidjson::Value &myCards = players[rapidjson::SizeType(myIndex)]["cards"];
    const rapidjson::Value &enemyCards = players[rapidjson::SizeType(enemyIndex)]["cards"];
    
    int index = cardOrder[rapidjson::SizeType(roundIndx)].GetInt();
    
    const rapidjson::Value &myCard = myCards[rapidjson::SizeType(index)];
    const rapidjson::Value &enemyCard = enemyCards[rapidjson::SizeType(index)];
    
    log("Checking: %s", CardKeys[id]);
    
    if(myCard[CardKeys[id]].GetInt() > enemyCard[CardKeys[id]].GetInt()){
        CCNotificationCenter::getInstance()->postNotification(ROUND_RESULT, CCString::create(ROUND_WON));
        roundResults[roundIndx] = 1;
    }else if(myCard[CardKeys[id]].GetInt() < enemyCard[CardKeys[id]].GetInt()){
        CCNotificationCenter::getInstance()->postNotification(ROUND_RESULT, CCString::create(ROUND_LOST));
        roundResults[roundIndx] = -1;
    }else{
        CCNotificationCenter::getInstance()->postNotification(ROUND_RESULT, CCString::create(ROUND_DRAW));
        roundResults[roundIndx] = 0;
    }
}

int HelloWorld::getGameResult()
{
    int result = 0;
    for(int i = 0; i < 10; i++){
        result += roundResults[i];
    }
    
    //send the result to server.
     _sioClient->emit("gameover", StringUtils::format("{\"result\":%d}", result));
    
    return result;
}

void HelloWorld::statClicked(int id)
{
    if(myTurn == false){
        log("Not your turn yet!");
        return;
    }
    
    //send data to the other player.
    _sioClient->emit("move", StringUtils::format("{\"stat\": %d}", id));
    
    log("Clicked: %d", id);
    
    //next is opponents' turn, this will be switched when i get a message back.
    myTurn = false;
    
    //notify the result of this round for this client.
    notifyRoundResult(id);
    
    roundIndx++;
}

void HelloWorld::moveEvent(SIOClient *client, const std::string& data)
{
    log("onMoveEvent: %s", data.c_str());
    
    myTurn = true;
    rapidjson::Document move;
    
    move.Parse<0>(data.c_str());
    
    notifyRoundResult(move["stat"].GetInt());
    
    roundIndx++;
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
