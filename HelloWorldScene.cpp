#include "HelloWorldScene.h"
#include <math.h>
USING_NS_CC;
#define threshold 0.05
#define EXITLENGTH    8  
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
{   P=3.141592653;	
	 orderflag=15;
	 direc=40.0f;
	 m_dHighestOutput=0;
	 m_iBestMatch=-1;
	 m_iMatch=-1;
	 RealAngle=60;
	 SumVec=Vec2(0.0f,0.0f);
	 DeltaAverrageAllVec=Vec2(0.0f,0.0f);
	 Veccount=countAll=0;
	 tempfield=-8;
	 recorded=false;
	 recorded2=false;
	 ended=false;
	 RecordDeltavec=false;
	 blue=green=0;
	 //ANN
	 trained=false;
	 started=false;
	 finished=false;
	 avoid=false;
	 persisted=false;
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    s = Director::getInstance()->getWinSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	m_pData = new CData(NUM_PATTERNS, NUM_VECTORS);
	m_pNet = new CNeuralNet(NUM_VECTORS*2, NUM_PATTERNS,  NUM_HIDDEN_NEURONS, LEARNING_RATE);

	/*if((fp1=fopen("tracking.txt","w"))==NULL)
	{
	printf("file open error1.\n");
	exit(0);
	}
	else
	{
	fprintf(fp1,"f fi fj  colf    Pos.x       Pos.y    PosLength  DeltaVec.x  DeltaVec.y  DeltaVecLength\n");
	}*/

	
	sim = new RVO::RVOSimulator();

	setupScenario3(sim,RealAngle);
	//setupScenario2(sim);
	//buildRoadmap(sim);

    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(s.width*0.9,origin.y +s.height*0.88+ closeItem->getContentSize().height/2));

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

	CCSprite *trainButton = CCSprite::create("green_edit.png");	
	CCSprite *trainButtoned = CCSprite::create("yellow_edit.png");

	CCSprite *upButton = CCSprite::create("f1.png");
	CCSprite *upButtoned = CCSprite::create("f2.png");

	CCSprite *downButton = CCSprite::create("b1.png");
	CCSprite *downButtoned= CCSprite::create("b2.png");

	CCMenuItemSprite *trainBtn =CCMenuItemSprite::create(trainButton, trainButtoned, trainButton, this, menu_selector(HelloWorld::menuCallback));
	CCMenuItemSprite *startBtn =CCMenuItemSprite::create(startButton, startButtoned, startButton, this, menu_selector(HelloWorld::menuCallback));
	CCMenuItemSprite *resetBtn=CCMenuItemSprite::create(resultButton,resultButtoned,resultButton,this,menu_selector(HelloWorld::menuCallback));
	CCMenuItemSprite *upBtn =CCMenuItemSprite::create(upButton, upButtoned, upButton, this, menu_selector(HelloWorld::menuCallback));
	CCMenuItemSprite *downBtn =CCMenuItemSprite::create(downButton, downButtoned, downButton, this, menu_selector(HelloWorld::menuCallback));

	trainBtn->setTag(301);
	//trainBtn->setPositionY(s.height*0.2);
	startBtn->setTag(302);
	
	resetBtn->setTag(303);

	upBtn->setTag(304);
	upBtn->setPositionX(s.width*0.2);
	downBtn->setTag(305);
	resetBtn->setPositionY(s.height*0.1);
	//resetBtn->setVisible(false);
	
	
	CCMenu *menu2 = CCMenu::create(resetBtn,startBtn,NULL);
	CCMenu *menu3=CCMenu::create(trainBtn,NULL);
	CCMenu *menu4=CCMenu::create(upBtn,downBtn,NULL);

	
	menu2->setPosition(Vec2(s.width*0.8,s.height*0.8));
	menu3->setPosition(Vec2(s.width*0.9,s.height*0.8));
	menu4->setPosition(Vec2(s.width*0.4,s.height*0.8));

	this->addChild(menu2,1);
	this->addChild(menu3,1);
	this->addChild(menu4,1);
	

	listener->onTouchBegan=CC_CALLBACK_2(HelloWorld::onTouchBegan,this);
	listener->onTouchMoved=CC_CALLBACK_2(HelloWorld::onTouchMoved,this);
	listener->onTouchEnded=CC_CALLBACK_2(HelloWorld::onTouchEnded,this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener,this);
    
    //BatchNode=SpriteBatchNode::create("agent01.png");
	//addChild(BatchNode, 0, 1);
	
	//addObstacle2();
	//重置个体位置
	for (size_t i=0; i<sim->getNumAgents();++i)
	{
		addNewAgent(Vec2(sim->getAgentPosition(i).x(),sim->getAgentPosition(i).y()),i);
	}

	goals.push_back(RVO::Vector2(-sim->getAgentPosition(0).x(),-sim->getAgentPosition(0).y()));
	goals.push_back(RVO::Vector2(direc,0));
	//pos
	CCSprite* agentflag=CCSprite::create("agent04.png");
	CCSprite* agentflag2=CCSprite::create("agent04.png");
	//deltavec
	CCSprite* agentflag3=CCSprite::create("agent03.png");
	CCSprite* agentflag4=CCSprite::create("agent03.png");
    //veci
	CCSprite* agentflag5=CCSprite::create("agent03.png");
	CCSprite* agentflag6=CCSprite::create("agent03.png");
	//vecj
	CCSprite* agentflag7=CCSprite::create("agent03.png");
	CCSprite* agentflag8=CCSprite::create("agent03.png");

	
	agentflag->setScale(0.3);
	agentflag2->setScale(0.3);
	agentflag3->setScale(0.3);
	agentflag4->setScale(0.3);
	agentflag5->setScale(0.3);
	agentflag6->setScale(0.3);
	agentflag7->setScale(0.3);
	agentflag8->setScale(0.3);

	this->addChild(agentflag,2,111);
	this->addChild(agentflag2,2,112);
	this->addChild(agentflag3,2,113);
	this->addChild(agentflag4,2,114);
	this->addChild(agentflag5,2,115);
	this->addChild(agentflag6,2,116);
	this->addChild(agentflag7,2,117);
	this->addChild(agentflag8,2,118);

    agentflag->setPosition(Vec2(s.width*0.2,s.height*0.25));
	agentflag2->setPosition(Vec2(s.width*0.2,s.height*0.25));
	agentflag3->setPosition(Vec2(s.width*0.4,s.height*0.3));
	agentflag4->setPosition(Vec2(s.width*0.4,s.height*0.3));

	agentflag5->setPosition(Vec2(s.width*0.6,s.height*0.3));
	agentflag6->setPosition(Vec2(s.width*0.6,s.height*0.3));
	agentflag7->setPosition(Vec2(s.width*0.8,s.height*0.3));
	agentflag8->setPosition(Vec2(s.width*0.8,s.height*0.3));

	label=CCLabelBMFont::create("DCAngle: 0","fonts/helvetica-32.fnt");
	label1=CCLabelBMFont::create("iCAngle: 0","fonts/helvetica-32.fnt");
	label2=CCLabelBMFont::create("jCAngle: 0","fonts/helvetica-32.fnt");
	label3=CCLabelBMFont::create("DeltaPosition","fonts/helvetica-32.fnt");
	label4=CCLabelBMFont::create("ANN ErrorSum:","fonts/helvetica-32.fnt");
	labelangle=CCLabelBMFont::create("Angle: 0","fonts/helvetica-32.fnt");

	label->setScale(0.6);
	label1->setScale(0.6);
	label2->setScale(0.6);
	label3->setScale(0.6);
	label4->setScale(0.6);
	labelangle->setScale(0.5);

	label->setPosition(Vec2(s.width*0.4,s.height*0.13));
	label1->setPosition(Vec2(s.width*0.6,s.height*0.13));
	label2->setPosition(Vec2(s.width*0.8,s.height*0.13));
	label3->setPosition(Vec2(s.width*0.2,s.height*0.13));
	label4->setPosition(Vec2(s.width*0.5,s.height*0.09));
	labelangle->setPosition(Vec2(s.width/2,s.height*0.8));

	this->addChild(label,3,120);
	this->addChild(label1,3,121);
	this->addChild(label2,3,122);
	this->addChild(label3,3,119);
	this->addChild(label4,3,123);
	this->addChild(labelangle,3,124);
    //DrawPrimitives::drawRect(Vec2(s.width/2,s.height/2),Vec2(15,15));
    CCLOG("%lf,%lf",s.width,s.height);
	scheduleUpdate();

	


    return true;
}


void HelloWorld::menuCallback(CCObject *pSender)
{
	CCMenuItemSprite *item = (CCMenuItemSprite*)pSender;
	if(item->getTag() == 303)
	{   
	//fclose(fp1);
	//CCScene *s=HelloWorld::createScene();
	//CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, s));
	Reset_Agent(RealAngle,direc);
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
	else if(item->getTag()==301)
	{
		if(trained!=true)
		{  
			SetIn=m_pData->GetInputSet();
			SetOut=m_pData->GetOutputSet();
			trained=true;
			
		}
	}
	else if(item->getTag()==304)
	{
		RealAngle=RealAngle+1.0f;
	}
	else if(item->getTag()==305)
	{
		RealAngle=RealAngle-1.0f;
	}
}

void HelloWorld::Reset_Agent(float angle,float xoffense)
{
	goals.clear();
	
	float xo=cosf(P/180*angle)*xoffense;
	float yo=sinf(P/180*angle)*xoffense;
	
		sim->setAgentPosition(0,RVO::Vector2(-xo,yo));
		sim->setAgentPosition(1,RVO::Vector2(-xoffense,0));

		for (size_t i=0; i<sim->getNumAgents();++i)
		{
			auto sp=(CCSprite*)this->getChildByTag(i);
			sp->setPosition(Vec2( sim->getAgentPosition(i).x()*3+s.width/2, sim->getAgentPosition(i).y()*3+s.height/2) );
		   goals.push_back(RVO::Vector2(-sim->getAgentPosition(i).x(),-sim->getAgentPosition(i).y()));
		}
		started=false;
		SumVec=Vec2(0.0f,0.0f);
		DeltaAverrageAllVec=Vec2(0.0f,0.0f);
		Veccount=countAll=0;
		tempfield=-8;
		recorded=false;
		recorded2=false;
		RecordDeltavec=false;
		blue=green=0;
		avoid=false;
		persisted=false;
		/*for (int i=15;i<orderflag;i++)
		{
			this->removeChildByTag(i);
		}*/
		scheduleUpdate();
}


//使用ANN
bool HelloWorld::TestForMatch(std::vector<double> vecinput)
{
	vector<double> outputs = m_pNet->Update(vecinput);
	m_dHighestOutput = 0;
	m_iBestMatch = 0;
	m_iMatch = -1;
	if (outputs.size() == 0)
	{
		CCLOG("Error in with ANN output");

		return false;
	}

	for (int i=0; i<outputs.size(); ++i)
	{
		if (outputs[i] > m_dHighestOutput)
		{
			//make a note of the most likely candidate
			m_dHighestOutput = outputs[i];

			m_iBestMatch = i;


			//if the candidates output exceeds the threshold we 
			//have a match! ...so make a note of it.
			if (m_dHighestOutput > MATCH_TOLERANCE)
			{
				m_iMatch = m_iBestMatch;

			}
		}
	}
	return true;
}

bool HelloWorld::TrainNetwork()
{
	if (!m_pNet->NetworkTrainingEpoch(SetIn,SetOut))
	{
		return false;
	}
		
	return true;
}

void HelloWorld::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	_customCommand.init(_globalZOrder);
	_customCommand.func = CC_CALLBACK_0(HelloWorld::onDraw, this, transform, flags);
	renderer->addCommand(&_customCommand);
}


void HelloWorld::onDraw(const cocos2d::Mat4 &transform, bool transformUpdated)
{
	Director *director = Director::getInstance();
	//CCASSERT(nullptr != director, "Director is null when setting matrix stack");
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);
	//绘制实时相对位置，相对速度、各agent速度
	CHECK_GL_ERROR_DEBUG();
	glLineWidth(1);
	DrawPrimitives::setDrawColor4B(255, 0, 0, 255);
	DrawPrimitives::drawLine(Vec2(s.width*0.2,s.height*0.25),Vec2(s.width*0.2+drawpos.x*6,s.height*0.25+drawpos.y*6));
	DrawPrimitives::drawLine(Vec2(s.width*0.4,s.height*0.3),Vec2(s.width*0.4+drawvec.x*60*drawveclength/(normallength*normallength),s.height*0.3+drawvec.y*60*drawveclength/(normallength*normallength)));
	DrawPrimitives::drawLine(Vec2(s.width*0.6,s.height*0.3),Vec2(s.width*0.6+agentveci.x*60*agentveci.length()/(agentvecinormlength*agentvecinormlength),s.height*0.3+agentveci.y*60*agentveci.length()/(agentvecinormlength*agentvecinormlength)));
	DrawPrimitives::drawLine(Vec2(s.width*0.8,s.height*0.3),Vec2(s.width*0.8+agentvecj.x*60*agentvecj.length()/(agentvecjnormlength*agentvecjnormlength),s.height*0.3+agentvecj.y*60*agentvecj.length()/(agentvecjnormlength*agentvecjnormlength)));

	CHECK_GL_ERROR_DEBUG();
	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

	//绘制初始相对速度基、各agent初始速度基
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);
	CHECK_GL_ERROR_DEBUG();
	glLineWidth(1);
	DrawPrimitives::setDrawColor4B(0, 0, 255, 255);
	DrawPrimitives::drawLine(Vec2(s.width*0.4,s.height*0.3),Vec2(s.width*0.4+drawnormVec.x*60/normallength,s.height*0.3+drawnormVec.y*60/normallength));
	DrawPrimitives::drawLine(Vec2(s.width*0.6,s.height*0.3),Vec2(s.width*0.6+drawnormVeci.x*60/agentvecinormlength,s.height*0.3+drawnormVeci.y*60/agentvecinormlength));
	DrawPrimitives::drawLine(Vec2(s.width*0.8,s.height*0.3),Vec2(s.width*0.8+drawnormVecj.x*60/agentvecjnormlength,s.height*0.3+drawnormVecj.y*60/agentvecjnormlength));
	CHECK_GL_ERROR_DEBUG();
	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);


	//绘制初始相对速度、各agent初始速度
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);
	CHECK_GL_ERROR_DEBUG();
	glLineWidth(1);
	DrawPrimitives::setDrawColor4B(0, 255, 0, 255);
	DrawPrimitives::drawLine(Vec2(s.width*0.4,s.height*0.3),Vec2(s.width*0.4+drawnormVec.x*60*drawveclength/(normallength*normallength),s.height*0.3+drawnormVec.y*60*drawveclength/(normallength*normallength)));
	DrawPrimitives::drawLine(Vec2(s.width*0.6,s.height*0.3),Vec2(s.width*0.6+drawnormVeci.x*60*drawnormVeci.length()/(agentvecinormlength*agentvecinormlength),s.height*0.3+drawnormVeci.y*60*drawnormVeci.length()/(agentvecinormlength*agentvecinormlength)));
	DrawPrimitives::drawLine(Vec2(s.width*0.8,s.height*0.3),Vec2(s.width*0.8+drawnormVecj.x*60*drawnormVecj.length()/(agentvecjnormlength*agentvecjnormlength),s.height*0.3+drawnormVecj.y*60*drawnormVecj.length()/(agentvecjnormlength*agentvecjnormlength)));
	CHECK_GL_ERROR_DEBUG();
	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);



}	
//判断方向
bool HelloWorld::thinkdirection(int i,int j)
{
	Vec2 vec1=Vec2(sim->getAgentPrefVelocity(i).x_,sim->getAgentPrefVelocity(i).y_);
	Vec2 vec2=Vec2(sim->getAgentPrefVelocity(j).x_,sim->getAgentPrefVelocity(j).y_);
	
  if (vec1.dot(vec2)>0)
  { //同向
	  return false;
  }

  //异向
  return true;
}

//获取double输入
std::vector<double> HelloWorld::getNormalInput(int i,int j)
{

	Vec2 veci=Vec2(sim->getAgentPrefVelocity(i).x(),sim->getAgentPrefVelocity(i).y());
	Vec2 vecj=Vec2(sim->getAgentPrefVelocity(j).x(),sim->getAgentPrefVelocity(j).y());
	float cosflo=veci.dot(vecj)/(veci.length()*vecj.length());
	std::vector<double> vecinput;
	double sinflo=sqrt(1-cosflo*cosflo);
	for (int i=0;i<6;i++)
	{
		vecinput.push_back(cosflo);
	    vecinput.push_back(sinflo);
	}

   return vecinput;
}


//判断j在i的范围分域中的哪一块
int HelloWorld::rangefield(int i,int j)
{
	Vec2 posvec=Vec2((sim->getAgentPosition(j)-sim->getAgentPosition(i)).x_,(sim->getAgentPosition(j)-sim->getAgentPosition(i)).y_);
    Vec2 vec=Vec2(sim->getAgentPrefVelocity(i).x_,sim->getAgentPrefVelocity(i).y_);
	float agentcos=posvec.dot(vec)/(posvec.length()*vec.length());
	float directnum=posvec.x*vec.y-vec.x*posvec.y;
		
        //在60度到81度之间	
	if (agentcos>0.1564&&agentcos<=0.5)
		{
			return (directnum>0?3:-3);
		}
		//在30度到60度之间
		else if(agentcos>0.5&&agentcos<=0.866)
		{
			return(directnum>0?2:-2);
		}
		//在0到30度之间
		else if(agentcos>0.866&&agentcos<1)
		{
			return(directnum>0?1:-1);
		}

		//在81度到90度之间
		else if (abs(agentcos)<0.1564)
		{
			return(directnum>0?4:-4);
		}

		//约为0度
		else if(abs(agentcos-1)<0.00001)
		{
			 return 0;
		}
		//其他情况
		else
		 return(directnum>0?7:-7);

}

//计算j是否会与i碰撞
int HelloWorld::thinkcollison(int i,int j,float r)
{ 
	 //float r=2.5;
	Vec2 vec=Vec2((sim->getAgentPrefVelocity(i)-sim->getAgentPrefVelocity(j)).x_,(sim->getAgentPrefVelocity(i)-sim->getAgentPrefVelocity(j)).y_);
	Vec2 cpos=Vec2((sim->getAgentPosition(i)-sim->getAgentPosition(j)).x_,(sim->getAgentPosition(i)-sim->getAgentPosition(j)).y_);
 
	if (cpos.length()<15&&((vec.dot(-cpos))/(vec.length()*cpos.length())>0))
  {
	  if(cpos.length()>2*r)
		{ 
			if ((cpos.x*cpos.x*vec.y*vec.y+vec.x*vec.x*cpos.y*cpos.y-2*cpos.x*cpos.y*vec.x*vec.y-4*r*r*(vec.x*vec.x+vec.y*vec.y)<=0))
		   {  
			   //大约偏转在6度以内标记为0
			 if (abs((vec.dot(-cpos))/(vec.length()*cpos.length())-1)<=0.05&&vec.length()>threshold)
		     {
				 return 0;
		     }
			 else return 1;
		   }		 
		  else return 2;
	   }
	  else return 0;	 
  }
 else return 2;
}

//非质心碰撞的偏移向量改变比率
double HelloWorld::GetColliShift(int i,int j,float radius)
{  //获取第一次取小值时候的偏移碰撞向量
	Vec2 deltavec=Vec2((sim->getAgentPrefVelocity(i)-sim->getAgentPrefVelocity(j)).x_,(sim->getAgentPrefVelocity(i)-sim->getAgentPrefVelocity(j)).y_);
	Vec2 deltapos=Vec2((sim->getAgentPosition(i)-sim->getAgentPosition(j)).x_,(sim->getAgentPosition(i)-sim->getAgentPosition(j)).y_);
	float alpha=(-deltapos.length()*deltapos.dot(deltavec)/(deltapos.length()*deltavec.length())-2*radius)/deltavec.length();
	Vec2 shiftVec=deltapos+alpha*deltavec;

	//计算相对位置到非质心碰撞的质心相对向量的位置关系，在相对速度左为1，右为0
	int shiftpos=deltapos.x*shiftVec.y-deltapos.y*shiftVec.x>0? 1:0;
	//计算个体j的速度向量到相对位置间的位置关系，在个体j左为1，右为0
	int vecpos=sim->getAgentPrefVelocity(j).x()*deltapos.y-sim->getAgentPrefVelocity(j).y()*deltapos.x>0?1:0;
	//计算质心相对向量与相对位置的cos值
	double shiftCos=shiftVec.dot(deltapos)/(shiftVec.length()*deltapos.length());
	//计算临界碰撞情况时的cos值
	double normCos=2*radius/deltapos.length();
	//反三角函数计算角度比例
	double collishift=(acosf(normCos)-acosf(shiftCos))/acosf(normCos);
	//返回非质心的偏移比率，正数表示个体j先通过，i后通过，负数表示个体i先通过，个体j后通过
	return  ((shiftpos==vecpos)?collishift:-collishift);
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
	   int choice=i%2+1;  
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
void HelloWorld::addTrackflag(int i,int j,int field)
{
	CCSprite* ob5=CCSprite::create("flag.png");
	CCSprite* ob6=CCSprite::create("flag.png");
	ob6->setPosition(Vec2(sim->getAgentPosition(j).x()*3+s.width/2,sim->getAgentPosition(j).y()*3+s.height/2));
	ob5->setPosition(Vec2(sim->getAgentPosition(i).x()*3+s.width/2,sim->getAgentPosition(i).y()*3+s.height/2));
	float veclen=Vec2(sim->getAgentPrefVelocity(i).x(),sim->getAgentPrefVelocity(i).y()).length();
	float veclen1=Vec2(sim->getAgentPrefVelocity(j).x(),sim->getAgentPrefVelocity(j).y()).length();
	
	if (field!=rangefield(i,j))
	{
		blue+=50;
		green+=50;
	}

	ob5->setColor(Color3B(int(200*veclen),green,0));
	ob6->setColor(Color3B(int(200*veclen1),green,0));
	//ob5->setOpacity(200*veclen);
	addChild(ob5,3,orderflag);
	addChild(ob6,3,orderflag+1);
	orderflag++;
	
    
}





void HelloWorld::update(float dt)
{
	//auto BatchNode=static_cast<SpriteBatchNode*>( getChildByTag(1));
	labelangle->setString(CCString::createWithFormat("Angle:%.1f",RealAngle)->getCString());
	
	//ANN训练的实时显示
  if(!finished)
   {	
	 if (trained)
	  {
		TrainNetwork();
	    label4->setString(CCString::createWithFormat("ErrorSum:%.4f",float(m_pNet->Error()))->getCString());	

	   if (m_pNet->Error()<ERROR_THRESHOLD)
	   {
		   finished=true;
		   label4->setString(CCString::createWithFormat("ErrorSum:%.4f",float(m_pNet->Error()))->getCString());
	   }
      }	
   }

 else if(started&&finished)
  {
	if(!reachedGoal(sim,goals))
	{	
		
	for (size_t i=0; i<sim->getNumAgents();i++)
	{
		//CCSprite* sp=(CCSprite*)BatchNode->getChildByTag(i);
		CCSprite* sp=(CCSprite*)this->getChildByTag(i);
		  sp->setPosition(Vec2(sim->getAgentPosition(i).x()*3+s.width/2,sim->getAgentPosition(i).y()*3+s.height/2));
		 
		//addNewAgent(Vec2(sim->getAgentPosition(i).x(),sim->getAgentPosition(i).y()));
	   for(int j=0;j<sim->getNumAgents();j++)
		{ 
			if(i!=j&&i<j)
	      {

			  //mageric
			  if (abs(rangefield(i,j))>4||abs(rangefield(j,i))>4)
			  {
				  persisted=false;
			  }
			  Vec2 JudgePos=Vec2((sim->getAgentPosition(i)-sim->getAgentPosition(j)).x_,(sim->getAgentPosition(i)-sim->getAgentPosition(j)).y_);

			  if (thinkcollison(i,j,2.5)!=2)
		    {
			  
			  Vec2 vecw=Vec2((sim->getAgentPrefVelocity(i)-sim->getAgentPrefVelocity(j)).x(),(sim->getAgentPrefVelocity(i)-sim->getAgentPrefVelocity(j)).y());
			  Vec2 cposw=Vec2((sim->getAgentPosition(i)-sim->getAgentPosition(j)).x(),(sim->getAgentPosition(i)-sim->getAgentPosition(j)).y());
			  float vecwlength=vecw.length();
			  float length=cposw.length();
			  Vec2 veci=Vec2(sim->getAgentPrefVelocity(i).x(),sim->getAgentPrefVelocity(i).y());
			    Vec2 vecj=Vec2(sim->getAgentPrefVelocity(j).x(),sim->getAgentPrefVelocity(j).y());
			  float vecilength=veci.length();
			  float vecjlength=vecj.length();
			  flag=0;
		      int field=0;
			  //对初始相对速度记录，作为标准基
			  if (!RecordDeltavec)
			  {
				  normallength=vecwlength;
				  agentvecinormlength=veci.length();
				  agentvecjnormlength=vecj.length();
				  drawnormVeci=veci;
			      drawnormVecj=vecj;
				  drawnormVec=vecw;
				  RecordDeltavec=true;
			  }

			  drawpos=cposw;
			  drawvec=vecw;
			  drawveclength=vecwlength;
			  agentveci=veci;
			  agentvecj=vecj;
			  iCAngel=acosf( drawnormVeci.dot(veci)/(agentvecinormlength*veci.length()))*180/P;
			  jCAngel=acosf( drawnormVecj.dot(vecj)/(agentvecjnormlength*vecj.length()))*180/P;
			  DeltaCAngel=acosf(drawnormVec.dot(vecw)/(normallength*vecw.length()))*180/P;
			  float ratio=normallength/(agentvecinormlength);

			  label->setString(CCString::createWithFormat("DCAngle: %.4f\n Length:%.4f",DeltaCAngel,vecwlength)->getCString());
			  label1->setString(CCString::createWithFormat("iCAngle: %.4f\n Length:%.4f",iCAngel,vecilength)->getCString());
			  label2->setString(CCString::createWithFormat("jCAngle: %.4f\n Length:%.4f",jCAngel,vecjlength)->getCString());
			  label3->setString(CCString::createWithFormat("DeltaPosition\n Length:%.4f",length)->getCString());
			  

			   CCSprite* aflag2=(CCSprite*)this->getChildByTag(112);
			   CCSprite* aflag3=(CCSprite*)this->getChildByTag(114);
			   CCSprite* aflag4=(CCSprite*)this->getChildByTag(116);
			   CCSprite* aflag5=(CCSprite*)this->getChildByTag(118);

			  aflag2->setPosition(Vec2(s.width*0.2+cposw.x*6,s.height*0.25+cposw.y*6));
			  aflag3->setPosition(Vec2(s.width*0.4+vecw.x*60*vecwlength/(normallength*normallength),s.height*0.3+vecw.y*60*vecwlength/(normallength*normallength)));
			  aflag4->setPosition(Vec2(s.width*0.6+veci.x*60*veci.length()/(agentvecinormlength*agentvecinormlength),s.height*0.3+veci.y*60*veci.length()/(agentvecinormlength*agentvecinormlength)));
			  aflag5->setPosition(Vec2(s.width*0.8+vecj.x*60*vecj.length()/(agentvecjnormlength*agentvecjnormlength),s.height*0.3+vecj.y*60*vecj.length()/(agentvecjnormlength*agentvecjnormlength)));
			  
			  // tempfield=rangefield(i,j);
			  //判断是否异向
			  if (thinkdirection(i,j))
			  {
				  flag=1;
			  }			  

			 float cos=(-cposw).dot(veci)/(cposw.length()*veci.length());

			// fprintf(fp1,"%d  %d  %d   %d  %.8f %.8f %.8f %.8f %.8f %.8f  %.8f  %.8f  %.8f  %.8f  %.8f  %.8f  %.8f\n",flag,rangefield(i,j),rangefield(j,i), thinkcollison(i,j,2.5),cposw.x,cposw.y,length,vecw.x,vecw.y,vecwlength,vecilength,veci.x,veci.y,vecj.length(),vecj.x,vecj.y,cos);
			
			    if (length<14)
			  { 
				  DeltaAverrageAllVec+=vecw;
				  countAll++;
				  if (!recorded2)
				  {
					  tempfield=rangefield(i,j);
					  recorded2=true;
				  }
				  // addTrackflag(i,j,tempfield);

			    if (tempfield!=rangefield(i,j))
				{
					//fprintf(fp1,"logr\n");

					DeltaAverrageVec=SumVec/Veccount;
					//char* str=NULL;
					//DeltaVec.push_back(DeltaAverrageVec);
					//fprintf(fp1,"%d  %d  %.8f  %.8f   %.8f   %.8f\n",flag,tempfield,DeltaAverrageVec.x,DeltaAverrageVec.y,DeltaAverrageVec.x-DeltaNormalVec.x,DeltaAverrageVec.y-DeltaNormalVec.y);
					//mess.push_back(str);
					Veccount=0;
					SumVec=Vec2(0.0f,0.0f);
					tempfield=rangefield(i,j);
					SumVec+=vecw;
					Veccount++;
				}
				else if(tempfield==rangefield(i,j))
				{
				 SumVec+=vecw;
				 Veccount++;
					//fprintf(fp1,"log\n");
				}
				
			  }
			 else if(length<16&&!recorded)
			 {
				 DeltaNormalVec=Vec2((sim->getAgentPrefVelocity(i)-sim->getAgentPrefVelocity(j)).x(),(sim->getAgentPrefVelocity(i)-sim->getAgentPrefVelocity(j)).y());
				 recorded=true;
			  }

			 //ANN
				if (!avoid)
				{
					std::vector<double> input=getNormalInput(i,j);

					if (input[0]>0.5&&input[0]<0.965)
					{
						persisted=true;
					}
					

					
					if(TestForMatch(input))
					{
						float shift=GetColliShift(i,j,2.5);
						if (shift<0.9||shift>-0.9)
						{
							shift>0?setNewAvoidVelocity(sim,j,i,m_iBestMatch,abs(shift)):setNewAvoidVelocity(sim,i,j,m_iBestMatch,abs(shift));
						}
						else
						{
							int ra[2];
							ra[0]=i;
							ra[1]=j;
							int rad=rand()%2;						
							setNewAvoidVelocity(sim,ra[rad],ra[1-rad],m_iBestMatch,1);
						}
					}
					
					avoid=true;
				}

				
		  }//collision deal
		
          else if(!persisted&&thinkcollison(i,j,2.5)==2&&JudgePos.length()>EXITLENGTH)
		 {
		 //return formal
		  setPreferredVelocities(sim,i,goals);
		  setPreferredVelocities(sim,j,goals);
		  avoid=false;
		 }
	  }
		   

     }
	}
	
	//CCLOG("agent 1 speed:%lf %lf",sim->getAgentVelocity(1).x(),sim->getAgentVelocity(1).y());
	//CCLOG("agent 1 position:%lf %lf",sim->getAgentPosition(1).x(),sim->getAgentPosition(1).y());

	//setPreferredVelocities(sim);
	sim->doStep();

	}

	else
	{ 
		//char* str2=NULL;
		DeltaAverrageVec=SumVec/Veccount;
		DeltaAverrageAllVec=DeltaAverrageAllVec/countAll;
		//fprintf(fp1,"%d  %d  %.8f  %.8f   %.8f   %.8f\n\n",flag,tempfield,DeltaAverrageVec.x,DeltaAverrageVec.y,DeltaAverrageVec.x-DeltaNormalVec.x,DeltaAverrageVec.y-DeltaNormalVec.y);
		//mess.push_back(str2);
		//fprintf(fp1,"%.8f   %.8f\n",DeltaNormalVec.x,DeltaNormalVec.y);
		//fprintf(fp1,"%.8f   %.8f\n",DeltaAverrageAllVec.x,DeltaAverrageAllVec.y);
		//fprintf(fp1,"%.8f  %.8f\n",DeltaAverrageAllVec.x-DeltaNormalVec.x,DeltaAverrageAllVec.y-DeltaNormalVec.y);
		//fclose(fp1);
		this->unscheduleUpdate();
	}

  }
}


void HelloWorld::setNewAvoidVelocity(RVO::RVOSimulator *sim,int agenti,int agentj,int bestMatch,double collishift)
{
      float judge=rangefield(agenti,agentj)>0?1.0f:-1.0f;
     std::vector<double> vecout=m_pData->PatternName(bestMatch);
  if (bestMatch>=1)
  {
	double x1=sim->getAgentPrefVelocity(agenti).x()*cosf(judge*(vecout[0]*P/180)*collishift)-sim->getAgentPrefVelocity(agenti).y()*sinf(judge*(vecout[0]*P/180)*collishift);
	double y1=sim->getAgentPrefVelocity(agenti).y()*cosf(judge*(vecout[0]*P/180)*collishift)+sim->getAgentPrefVelocity(agenti).x()*sinf(judge*(vecout[0]*P/180)*collishift);

	double x2=sim->getAgentPrefVelocity(agentj).x()*cosf(judge*(vecout[1]*P/180)*collishift)-sim->getAgentPrefVelocity(agentj).y()*sinf(judge*(vecout[1]*P/180)*collishift);
	double y2=sim->getAgentPrefVelocity(agentj).y()*cosf(judge*(vecout[1]*P/180)*collishift)+sim->getAgentPrefVelocity(agentj).x()*sinf(judge*(vecout[1]*P/180)*collishift);

	RVO::Vector2 Nveci=RVO::Vector2(x1,y1)*vecout[2];
	RVO::Vector2 Nvecj=RVO::Vector2(x2,y2)*vecout[3];

	sim->setAgentPrefVelocity(agenti,Nveci);
	sim->setAgentPrefVelocity(agentj,Nvecj);

  }
  else
  {
	double x1=sim->getAgentPrefVelocity(agenti).x()*cosf((-judge)*(vecout[0]*P/180)*collishift)-sim->getAgentPrefVelocity(agenti).y()*sinf((-judge)*(vecout[0]*P/180)*collishift);
	double y1=sim->getAgentPrefVelocity(agenti).y()*cosf((-judge)*(vecout[0]*P/180)*collishift)+sim->getAgentPrefVelocity(agenti).x()*sinf((-judge)*(vecout[0]*P/180)*collishift);

	double x2=sim->getAgentPrefVelocity(agentj).x()*cosf(judge*(vecout[1]*P/180)*collishift)-sim->getAgentPrefVelocity(agentj).y()*sinf(judge*(vecout[1]*P/180)*collishift);
	double y2=sim->getAgentPrefVelocity(agentj).y()*cosf(judge*(vecout[1]*P/180)*collishift)+sim->getAgentPrefVelocity(agentj).x()*sinf(judge*(vecout[1]*P/180)*collishift);

	RVO::Vector2 Nveci=RVO::Vector2(x1,y1)*vecout[2];
	RVO::Vector2 Nvecj=RVO::Vector2(x2,y2)*vecout[3];

	sim->setAgentPrefVelocity(agenti,Nveci);
	sim->setAgentPrefVelocity(agentj,Nvecj);
  }
	
}