
#include "LolWars.h"
#include "RiotAPI.hpp"
#include "HelloWorldScene.h"

LolWars* LolWars::getNewInstance() {
    /* Create an autorelease NodeLoaderLibrary. */
    NodeLoaderLibrary * ccNodeLoaderLibrary = NodeLoaderLibrary::newDefaultNodeLoaderLibrary();

    ccNodeLoaderLibrary->registerNodeLoader("LolWars", LolWarsLoader::loader());
    ccNodeLoaderLibrary->registerNodeLoader("LolCard", LolCardLoader::loader());
    
    /* Create an autorelease CCBReader. */
    cocosbuilder::CCBReader * ccbReader = new cocosbuilder::CCBReader(ccNodeLoaderLibrary);
    ccbReader->setCCBRootPath("res/");
    auto node = ccbReader->readNodeGraphFromFile("res/LolWars.ccbi");
//    node->setPosition(Director::getInstance()->getWinSize()/2);

    CCBAnimationManager *ptr = dynamic_cast<CCBAnimationManager*>(node->getUserObject());
    LolWars *lolWars = dynamic_cast<LolWars*>(node);
    lolWars->setAnimManager(ptr);

    ptr->setDelegate(lolWars);
    
    //node->setUserObject(NULL);
    ccbReader->autorelease();

    return lolWars;
}

cocos2d::SEL_MenuHandler LolWars::onResolveCCBCCMenuItemSelector(cocos2d::Ref * pTarget, const char* pSelectorName)
{

    return nullptr;
}

Control::Handler LolWars::onResolveCCBCCControlSelector(cocos2d::Ref * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(this, "playClicked", LolWars::playClicked);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(this, "replayClicked", LolWars::replayClicked);

    return nullptr;
}

bool LolWars::init()
{
    if(!Layer::init()){
        return false;
    }
    
    startScreen = NULL;
    loadingScreen = NULL;
    gameplayScreen = NULL;
    enemyCard = NULL;
    myCard = NULL;
    ccbAnimManager = NULL;
    playButton = nullptr;
    uiElements = nullptr;
    gameResultLabel = nullptr;
    
    //subscribe to the notifications of the game.
    CCNotificationCenter::getInstance()->addObserver(this, callfuncO_selector(LolWars::gameStart), GAME_START, NULL);
    CCNotificationCenter::getInstance()->addObserver(this, callfuncO_selector(LolWars::roundResult), ROUND_RESULT, NULL);
    CCNotificationCenter::getInstance()->addObserver(this, callfuncO_selector(LolWars::roundResult), ROUND_RESULT, NULL);
    CCNotificationCenter::getInstance()->addObserver(this, callfuncO_selector(LolWars::roundResult), ROUND_RESULT, NULL);
    CCNotificationCenter::getInstance()->addObserver(this, callfuncO_selector(LolWars::opponentDC), OPPONENT_DC, NULL);
    return true;
}

void LolWars::showGameResult()
{
    HelloWorld *hw = dynamic_cast<HelloWorld*>(this->getParent());

    //game finished consolidate.
    int result = hw->getGameResult();
    if(result > 0){
        //show win result
        gameResultLabel->setString("VICTORY");
    }else if(result < 0){
        //show loss reslut.
        gameResultLabel->setString("DEFEAT");
    }else{
        //show draw result.
        gameResultLabel->setString("It's a TIE");
    }
    ccbAnimManager->runAnimationsForSequenceNamed("showGameResult");

}

void LolWars::opponentDC(Ref *ptr)
{
    showGameResult();
}

void LolWars::completedAnimationSequenceNamed(const char* seq)
{
    log("%s", seq);
    //first round setup.
    HelloWorld *hw = dynamic_cast<HelloWorld*>(this->getParent());

    if(strcmp(seq, "hideLoadingScreen") == 0){
        if(hw){
            rapidjson::Value&players = hw->matchData["p"];
            rapidjson::Value&me = players[hw->myIndex];
            rapidjson::Value&enemy = players[hw->enemyIndex];
            
            myCard->setPlayerName(me["name"].GetString());
            enemyCard->setPlayerName(enemy["name"].GetString());
            
            //update the card data so we dont' show random crap.
            enemyCard->setCardData(hw->getCurrentCard(true));
            myCard->setCardData(hw->getCurrentCard());
            
            if(hw->myTurn){
                ccbAnimManager->runAnimationsForSequenceNamed("showYourTurn");
            }else{
                ccbAnimManager->runAnimationsForSequenceNamed("showEnemyTurn");
            }
        }else{
            log("Couldn't find the parent ??. WTF!!");
        }
    }else if(strcmp(seq, "showYourTurnResult") == 0 || strcmp(seq, "showEnemyTurnResult") == 0){
        if(hw){
            if(hw->roundIndx < 10){
                //update the card data so we dont' show random crap.
                enemyCard->setCardData(hw->getCurrentCard(true));
                myCard->setCardData(hw->getCurrentCard());
            
                runAction(Sequence::create(DelayTime::create(0.1), CallFunc::create([=]{
                    if(hw->myTurn){
                        ccbAnimManager->runAnimationsForSequenceNamed("showYourTurn");
                    }else{
                        ccbAnimManager->runAnimationsForSequenceNamed("showEnemyTurn");
                    }
                }), NULL));
            }else{
                showGameResult();
            }
        }
    }
    
    //TODO: result animation completes, then initialize for the next  round.
}

void LolWars::replayClicked(cocos2d::Ref *pSender, Control::EventType pControlEvent)
{
    ccbAnimManager->runAnimationsForSequenceNamed("hideGameResult");
    HelloWorld *hw = dynamic_cast<HelloWorld*>(this->getParent());
    hw->askForMatchMake();
}

void LolWars::roundResult(Ref *res)
{
    CCString *ptr = dynamic_cast<CCString*>(res);
    if(ptr){
        log("%s ", ptr->getCString());
        
        if(ptr->compare("") == 0){
            //execute animations for corresponding result.
            //after the result animation. set the card data and set the state.
        }
        //the values of turn are already decided by the time the result comes in..
        //we should do inverse of the turn variable.
        HelloWorld *hw = dynamic_cast<HelloWorld*>(this->getParent());

        if(hw->myTurn == true){
            ccbAnimManager->runAnimationsForSequenceNamed("showEnemyTurnResult");
        }else{
            ccbAnimManager->runAnimationsForSequenceNamed("showYourTurnResult");
        }

    }else{
        log("CCString didn't come trhough");
    }
}

void LolWars::gameStart(Ref *res)
{
    ccbAnimManager->runAnimationsForSequenceNamed("hideLoadingScreen");
}

void LolWars::onEnter()
{
    Layer::onEnter();
    playButton->setEnabled(false);
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    
    auto editBoxSize = Size(visibleSize.width - 100, 60);

    m_EditBox = ui::EditBox::create(editBoxSize,
                                          cocos2d::ui::Scale9Sprite::createWithSpriteFrame(Sprite::create("res/ccbResources/ui/box.png")->getSpriteFrame()));

//    m_EditBox->setPosition(Vec2(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2));
    m_EditBox->setFontName("Arial");
    m_EditBox->setFontSize(25);
    m_EditBox->setFontColor(Color3B::BLACK);
    
    std::string sname = UserDefault::getInstance()->getStringForKey("summonername", "");
    if(sname.empty()){
        m_EditBox->setPlaceHolder("Summoner Name");
    }else{
        m_EditBox->setText(sname.c_str());
    }
    
    m_EditBox->setPlaceholderFontColor(Color3B::WHITE);
    m_EditBox->setMaxLength(30);
    m_EditBox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    m_EditBox->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
    uiElements->addChild(m_EditBox);
    
    //get a callback when a card id is pressed.
    myCard->setStatClickedCallback(CC_CALLBACK_1(LolWars::statClicked, this));
}

void LolWars::statClicked(int _id)
{
    HelloWorld *hw = dynamic_cast<HelloWorld*>(this->getParent());
    if(hw->myTurn){
        //show opponent card.
        ccbAnimManager->runAnimationsForSequenceNamed("showEnemyTurn");
    }
    
    hw->statClicked(_id);
}

void LolWars::onExit()
{
    Layer::onExit();
}

bool LolWars::onAssignCCBMemberVariable(cocos2d::Ref* pTarget, const char* pMemberVariableName, cocos2d::Node* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "startScreen", Layer *, startScreen);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "loadingScreen", LayerColor *, loadingScreen);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "gameplayScreen", Layer *, gameplayScreen);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "enemyCard", LolCard *, enemyCard);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "myCard", LolCard *, myCard);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "uiElements", Node *, uiElements);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "playButton", ControlButton *, playButton);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "gameResultLabel", Label *, gameResultLabel);
    
    return false;
}

void LolWars::playClicked(cocos2d::Ref *pSender, Control::EventType pControlEvent){
	log("LolWars::playClicked");
    ccbAnimManager->runAnimationsForSequenceNamed("showLoadingScreen");
    playButton->setEnabled(false);
    m_EditBox->setVisible(false);
    m_EditBox->setEnabled(false);
    RiotAPI::getInstance()->fetchID(m_EditBox->getText());
    
    UserDefault::getInstance()->setStringForKey("summonername", m_EditBox->getText());
    UserDefault::getInstance()->flush();
}

LolWars::~LolWars()
{
    CC_SAFE_RELEASE(startScreen);
    CC_SAFE_RELEASE(loadingScreen);
    CC_SAFE_RELEASE(gameplayScreen);
    CC_SAFE_RELEASE(enemyCard);
    CC_SAFE_RELEASE(myCard);
    CC_SAFE_RELEASE(uiElements);
    CC_SAFE_RELEASE(playButton);
    CC_SAFE_RELEASE(gameResultLabel);
}
