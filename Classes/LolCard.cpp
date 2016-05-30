
#include "LolCard.h"
LolCard* LolCard::getNewInstance() {
    /* Create an autorelease NodeLoaderLibrary. */
    NodeLoaderLibrary * ccNodeLoaderLibrary = NodeLoaderLibrary::newDefaultNodeLoaderLibrary();

    ccNodeLoaderLibrary->registerNodeLoader("LolCard", LolCardLoader::loader());

    /* Create an autorelease CCBReader. */
    cocosbuilder::CCBReader * ccbReader = new cocosbuilder::CCBReader(ccNodeLoaderLibrary);
    ccbReader->setCCBRootPath("res/");
    auto node = ccbReader->readNodeGraphFromFile("res/LolCard.ccbi");
    node->setPosition(Director::getInstance()->getWinSize()/2);

    node->setUserObject(NULL);
    ccbReader->autorelease();

    return dynamic_cast<LolCard*>(node);
}

cocos2d::SEL_MenuHandler LolCard::onResolveCCBCCMenuItemSelector(cocos2d::Ref * pTarget, const char* pSelectorName)
{

    return nullptr;
}

Control::Handler LolCard::onResolveCCBCCControlSelector(cocos2d::Ref * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(this, "statClicked", LolCard::statClicked);

    return nullptr;
}

bool LolCard::init()
{
    if(!Node::init()){
        return false;
    }
    imgFrame = NULL;
    champName = NULL;
    statKills = NULL;
    statDeaths = NULL;
    statAssists = NULL;
    statResult = NULL;
    statTurrets = NULL;
    statTime = NULL;
    cardPlayerName = NULL;    
    callback = nullptr;
    return true;
}

void LolCard::onEnter()
{
    Node::onEnter();
}

void LolCard::onExit()
{
    Node::onExit();
}

bool LolCard::onAssignCCBMemberVariable(cocos2d::Ref* pTarget, const char* pMemberVariableName, cocos2d::Node* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "imgFrame", Sprite *, imgFrame);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "champName", Label *, champName);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "statKills", Label *, statKills);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "statDeaths", Label *, statDeaths);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "statAssists", Label *, statAssists);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "statResult", Label *, statResult);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "statTurrets", Label *, statTurrets);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "statTime", Label *, statTime);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "cardPlayerName", Label *, cardPlayerName);

    return false;
}

void LolCard::statClicked(cocos2d::Ref *pSender, Control::EventType pControlEvent){
	log("LolCard::statClicked");
    if(callback){
        Node *ptr = dynamic_cast<Node*>(pSender);
        callback(ptr->getTag());
    }
}

void LolCard::setPlayerName(std::string pname)
{
    cardPlayerName->setString(pname);
}

void LolCard::setCardData(const rapidjson::Value &card)
{
    std::string champImgfile = StringUtils::format("res/ccbResources/champs/%d.png", card["c"].GetInt());
    Texture2D *tex = TextureCache::getInstance()->addImage(champImgfile);
    if(tex){
        imgFrame->setTexture(tex);
    }else{
        imgFrame->setTexture(TextureCache::getInstance()->addImage("res/ccbResources/ui/box.png"));
    }
    
    statKills->setString(StringUtils::format("%d", card["k"].GetInt()));
    statDeaths->setString(StringUtils::format("%d", card["d"].GetInt()));
    statAssists->setString(StringUtils::format("%d", card["a"].GetInt()));
    statTurrets->setString(StringUtils::format("%d", card["tu"].GetInt()));
    
    if(card["w"].GetInt() == 1){
        statResult->setString("WIN");
    }else{
        statResult->setString("LOSS");
    }
}

LolCard::~LolCard()
{
    CC_SAFE_RELEASE(imgFrame);
    CC_SAFE_RELEASE(champName);
    CC_SAFE_RELEASE(statKills);
    CC_SAFE_RELEASE(statDeaths);
    CC_SAFE_RELEASE(statAssists);
    CC_SAFE_RELEASE(statResult);
    CC_SAFE_RELEASE(statTurrets);
    CC_SAFE_RELEASE(statTime);
    CC_SAFE_RELEASE(cardPlayerName);

}
