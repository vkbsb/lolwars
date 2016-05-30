
#ifndef __LolWars__
#define __LolWars__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "cocosbuilder/CocosBuilder.h"
#include "LolCard.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocosbuilder;

class LolWars : public Layer,  public CCBSelectorResolver, public CCBMemberVariableAssigner, public CCBAnimationManagerDelegate
{
    //variables come here.
    Layer * startScreen;
    LayerColor * loadingScreen;
    Layer * gameplayScreen;
    LolCard *enemyCard;
    LolCard *myCard;
    Node *uiElements;
    ui::EditBox *m_EditBox;
    Label *gameResultLabel;
    
    CCBAnimationManager *ccbAnimManager;
    
    public:
    ControlButton *playButton;

    void setAnimManager(CCBAnimationManager *ptr){ ccbAnimManager = ptr;}
    
    bool init();
    virtual void onEnter();
    virtual void onExit();

    virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::Ref * pTarget, const char* pSelectorName);
    virtual Control::Handler onResolveCCBCCControlSelector(cocos2d::Ref * pTarget, const char* pSelectorName);
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref* target, const char* memberVariableName, cocos2d::Node* node);

    virtual void completedAnimationSequenceNamed(const char *name);
    
    //member functions for callbacks.
    void playClicked(cocos2d::Ref *pSender, Control::EventType pControlEvent);
    void replayClicked(cocos2d::Ref *pSender, Control::EventType pControlEvent);
    
    void statClicked(int _id);


    void showGameResult();
    //gameEngine related callbacks.
    void gameStart(Ref *ptr);
    void roundResult(Ref *res);
    void opponentDC(Ref *ptr);
    
    ~LolWars();
    CREATE_FUNC(LolWars);

    static LolWars* getNewInstance();
};

class LolWarsLoader : public LayerLoader {
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(LolWarsLoader, loader);

        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(LolWars);
};

#endif /* defined(__LolWars__) */ 
