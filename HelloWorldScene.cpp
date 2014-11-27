#include "HelloWorldScene.h"

USING_NS_CC;
#define threshold 0.3
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

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    s = Director::getInstance()->getWinSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	
	if((fp1=fopen("tracking.txt","w"))==NULL)
	{
		printf("file open error1.\n");
		exit(0);
	}
	else
	{
		fprintf(fp1,"f i j field    Pos.x       Pos.y    PosLength  DeltaVec.x  DeltaVec.y  DeltaVeci.x DeltaVeci.y \n");
	}

	started=false;
	sim = new RVO::RVOSimulator();

	setupScenario3(sim);
	//setupScenario2(sim);
	//buildRoadmap(sim);

    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + s.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
	auto listener=EventListenerTouchOneByOne::create();
	//auto listener=EventListenerTouchAllAtOnce::create();

	CCSprite *resultButton = CCSprite::create("reset.png");	
	CCSprite *resultButtoned = CCSprite::create("reseted.png");

	CCSprite *startButton = CCSprite::create("r1.png");	
	CCSprite *startButtoned = CCSprite::create("r2.png");
	CCMenuItemSprite *startBtn =CCMenuItemSprite::create(startButton, startButtoned, startButton, this, menu_selector(HelloWorld::menuCallback));
	startBtn->setTag(302);
	CCMenuItemSprite *resetBtn=CCMenuItemSprite::create(resultButton,resultButtoned,resultButton,this,menu_selector(HelloWorld::menuCallback));
	resetBtn->setTag(303);
	resetBtn->setPositionY(s.height*0.1);
	CCMenu *menu2 = CCMenu::create(resetBtn,startBtn,NULL);
	menu2->setPosition(Vec2(s.width*0.8,s.height*0.1));
	this->addChild(menu2,1);


	listener->onTouchBegan=CC_CALLBACK_2(HelloWorld::onTouchBegan,this);
	listener->onTouchMoved=CC_CALLBACK_2(HelloWorld::onTouchMoved,this);
	listener->onTouchEnded=CC_CALLBACK_2(HelloWorld::onTouchEnded,this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener,this);
    
    //BatchNode=SpriteBatchNode::create("agent01.png");
	//addChild(BatchNode, 0, 1);
	
	addObstacle2();

	for (size_t i=0; i<sim->getNumAgents();++i)
	{
		addNewAgent(Vec2(sim->getAgentPosition(i).x(),sim->getAgentPosition(i).y()),i);
	}
    //DrawPrimitives::drawRect(Vec2(s.width/2,s.height/2),Vec2(15,15));
    CCLOG("%lf,%lf",s.width,s.height);
	scheduleUpdate();

	


    return true;
}


//判断方向
bool HelloWorld::thinkdirection(int i,int j)
{
	Vec2 vec1=Vec2(sim->getAgentVelocity(i).x_,sim->getAgentVelocity(i).y_);
	Vec2 vec2=Vec2(sim->getAgentVelocity(j).x_,sim->getAgentVelocity(j).y_);
	
  if (vec1.dot(vec2)>0)
  { //同向
	  return false;
  }

  //异向
  return true;
}


//判断j在i的范围分域中的哪一块
int HelloWorld::rangefield(int i,int j)
{
	Vec2 posvec=Vec2((sim->getAgentPosition(j)-sim->getAgentPosition(i)).x_,(sim->getAgentPosition(j)-sim->getAgentPosition(i)).y_);
    Vec2 vec=Vec2(sim->getAgentVelocity(i).x_,sim->getAgentVelocity(i).y_);
	float agentcos=posvec.dot(vec)/(posvec.length()*vec.length());
	float directnum=posvec.x*vec.y-vec.x*posvec.y;
	
		if (agentcos>0&&agentcos<=0.5)
		{
			return (directnum>0?3:-3);
		}
		else if(agentcos>0.5&&agentcos<=sqrt(3)/2)
		{
			return(directnum>0?2:-2);
		}
		else if(agentcos>sqrt(3)/2&&agentcos<1)
		{
			return(directnum>0?1:-1);
		}
		else if (agentcos=0)
		{
			return 4;
		}

		else if(agentcos=1)
		{
			 return 0;
		}
		else
		 return -7;

}

//计算j是否会与i碰撞
bool HelloWorld::thinkcollison(int i,int j)
{ 
	 float r=2.5;
	Vec2 vec=Vec2((sim->getAgentVelocity(i)-sim->getAgentVelocity(j)).x_,(sim->getAgentVelocity(i)-sim->getAgentVelocity(j)).y_);
	Vec2 cpos=Vec2((sim->getAgentPosition(i)-sim->getAgentPosition(j)).x_,(sim->getAgentPosition(i)-sim->getAgentPosition(j)).y_);
 
	if (cpos.length()<15)
 {
	 if (vec.dot(-cpos)>0)
	 {
		 if ((cpos.x*cpos.x*vec.y*vec.y+vec.x*vec.x*cpos.y*cpos.y-2*cpos.x*cpos.y*vec.x*vec.y-4*r*r*(vec.x*vec.x+vec.y*vec.y)<=0))
		 {
			 if (vec.length()>threshold)
			 {
				 return true;
			 }
			 else
			 return false;
		 }
		 
		else return false;
	 }
	 
	 else
		 return false;
 }
 else
		return false;
}


void HelloWorld::menuCallback(CCObject *pSender)
{
	CCMenuItemSprite *item = (CCMenuItemSprite*)pSender;
	if(item->getTag() == 303)
	{   fclose(fp1);
		CCScene *s=HelloWorld::createScene();
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, s));
	
	}
	else if(item->getTag()==302)
	{
		if (started==true)
		{
			started=false;
		}
		else
		started=true;

	}
}


void HelloWorld::addObstacle()
{
	auto ob1=CCSprite::create("ob1.png");
	ob1->setScale(9/8);
	ob1->setPosition(Vec2(-75+s.width/2,75+s.height/2));
	auto ob2=CCSprite::create("ob1.png");
	ob2->setScale(9/8);
	ob2->setPosition(Vec2(-75+s.width/2,-75+s.height/2));
	auto ob3=CCSprite::create("ob1.png");
	ob3->setScale(9/8);
	ob3->setPosition(Vec2(75+s.width/2,-75+s.height/2));
	auto ob4=CCSprite::create("ob1.png");
	ob4->setScale(9/8);
	ob4->setPosition(Vec2(75+s.width/2,75+s.height/2));
	addChild(ob1,0,2);
	addChild(ob2,0,3);
	addChild(ob3,0,4);
	addChild(ob4,0,5);

}


void HelloWorld::addObstacle2()
{
	auto ob1=CCSprite::create("ob2.png");
	auto ob2=CCSprite::create("ob2.png");
	ob1->setPosition(Vec2(s.width/2,s.height/2+80));
	ob2->setPosition(Vec2(s.width/2,s.height/2-80));
	addChild(ob1,0,300);
	addChild(ob2,0,301);

}



//void HelloWorld::addNewAgent(Vec2 p,int i)
//{
//	
//	auto BatchNode=static_cast<SpriteBatchNode*>( getChildByTag(1));
//	auto sprite = Sprite::createWithTexture(BatchNode->getTexture());
//    sprite->setColor(Color3B(rand()%255,rand()%255,rand()%255));
//	sprite->setScale(0.8);
//	BatchNode->addChild(sprite,0,i);
//	CCLOG("%lf,%lf",sprite->getContentSize().width,sprite->getContentSize().height);
//	sprite->setPosition( Vec2( p.x*3+s.width/2, p.y*3+s.height/2) );
//	agentno++;
//}

void HelloWorld::addNewAgent(Vec2 p,int i)
{  
	  char str[100] = {0};
	   int choice=i%4+1;  
      sprintf(str, "agent%02d.png",choice);
      auto sprite=Sprite::create(str);
	  
	  sprite->setScale(0.8);
	   sprite->setPosition( Vec2( p.x*3+s.width/2, p.y*3+s.height/2) );
	addChild(sprite,0,i);

}


bool HelloWorld::onTouchBegan(Touch *touch,Event *unused_event)
{
	return true;
}


void HelloWorld::onTouchMoved(Touch *touch, Event *unused_event)
{

}

void HelloWorld::onTouchEnded(Touch *touch, Event *unused_event)
{
  //addNewAgent(Vec2(touch->getLocation().x,touch->getLocation().y));
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::update(float dt)
{
	auto BatchNode=static_cast<SpriteBatchNode*>( getChildByTag(1));
  if(started)
  {
	if(!reachedGoal(sim))
	{	
	for (size_t i=0; i<sim->getNumAgents();i++)
	{
		//CCSprite* sp=(CCSprite*)BatchNode->getChildByTag(i);
		CCSprite* sp=(CCSprite*)this->getChildByTag(i);
		  sp->setPosition(Vec2(sim->getAgentPosition(i).x()*3+s.width/2,sim->getAgentPosition(i).y()*3+s.height/2));
		
		//addNewAgent(Vec2(sim->getAgentPosition(i).x(),sim->getAgentPosition(i).y()));
	   for(int j=0;j<sim->getNumAgents();j++)
		{ if(i!=j&&i<j)
	       if (thinkcollison(i,j))
		  {
			  Vec2 vecw=Vec2((sim->getAgentVelocity(i)-sim->getAgentVelocity(j)).x_,(sim->getAgentVelocity(i)-sim->getAgentVelocity(j)).y_);
			  Vec2 cposw=Vec2((sim->getAgentPosition(i)-sim->getAgentPosition(j)).x_,(sim->getAgentPosition(i)-sim->getAgentPosition(j)).y_);
			  float length=cposw.length();
			  Vec2 veci=Vec2(sim->getAgentVelocity(i).x_,sim->getAgentVelocity(i).y_);
			  float veclength=veci.length();
			  int flag=0,field=0;
			
			  //判断是否异向
			  if (thinkdirection(i,j))
			  {
				  flag=1;
			  }			  

			  //if ((-cposw).dot(veci)/(length*veclength)>0.5)
			  //{   //在120度视角范围内
				 // flag=1;
				 // //否则flag=0，不在120度视角范围内。
			  //}
			  
			 fprintf(fp1,"%d %d %d   %d  %.8f %.8f %.8f %.8f %.8f %.8f %.8f\n",flag,i,j, rangefield(i,j),cposw.x,cposw.y,length,vecw.x,vecw.y,vecw.x-veci.x,vecw.y-veci.y);
			
		  }

	   }

	}
	//CCLOG("agent 1 speed:%lf %lf",sim->getAgentVelocity(1).x(),sim->getAgentVelocity(1).y());
	//CCLOG("agent 1 position:%lf %lf",sim->getAgentPosition(1).x(),sim->getAgentPosition(1).y());
	setPreferredVelocities(sim);
	sim->doStep();

	}

	else
	{ 
		fclose(fp1);
		this->unscheduleUpdate();
	}

  }
}