#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__
#include"Roadmap.h"
#include "cocos2d.h"
USING_NS_CC;
class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
	void addNewAgent(Vec2 p,int i);
	void addObstacle();
	void addObstacle2();
	bool thinkcollison(int i,int j);
	bool thinkdirection(int i,int j);
	int rangefield(int i,int j);
	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	virtual void onTouchMoved(Touch *touch, Event *unused_event);
	virtual void onTouchEnded(Touch *touch, Event *unused_event);
	void menuCallback(CCObject *pSender);
private:
	void update(float dt);
	Size s;
	RVO::RVOSimulator *sim;
	int agentno;
	SpriteBatchNode*  BatchNode;
	bool started;
	FILE *fp1;
};

#endif // __HELLOWORLD_SCENE_H__
