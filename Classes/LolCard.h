
#ifndef __LolCard__
#define __LolCard__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "cocosbuilder/CocosBuilder.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocosbuilder;

class LolCard : public Node,  public CCBSelectorResolver, public CCBMemberVariableAssigner
{
    //variables come here.
    Sprite * imgFrame;
    Label * champName;
    Label * statKills;
    Label * statDeaths;
    Label * statAssists;
    Label * statResult;
    Label * statTurrets;
    Label * statTime;
    Label * cardPlayerName;
    
    std::function<void(int _id)> callback;

    public:
    
    void setStatClickedCallback(std::function<void(int _id)> cb){ callback = cb;}
    void setPlayerName(std::string pname);
    void setCardData(const rapidjson::Value &card);
    
    bool init();
    virtual void onEnter();
    virtual void onExit();

    virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::Ref * pTarget, const char* pSelectorName);
    virtual Control::Handler onResolveCCBCCControlSelector(cocos2d::Ref * pTarget, const char* pSelectorName);
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref* target, const char* memberVariableName, cocos2d::Node* node);

    //member functions for callbacks.
    void statClicked(cocos2d::Ref *pSender, Control::EventType pControlEvent);

    ~LolCard();
    CREATE_FUNC(LolCard);

    static LolCard* getNewInstance();
};

class LolCardLoader : public NodeLoader {
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(LolCardLoader, loader);

        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(LolCard);
};

#endif /* defined(__LolCard__) */ 
