#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__
#include"Roadmap.h"
#include "cocos2d.h"
#include "SVector2D.h"
#include "defines.h"
#include "CNeuralNet.h"
#include "CData.h"

#include <vector>
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
	void Reset_Agent(float angle,float xoffense);

	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	virtual void onTouchMoved(Touch *touch, Event *unused_event);
	virtual void onTouchEnded(Touch *touch, Event *unused_event);
	void menuCallback(CCObject *pSender);


	
	bool thinkdirection(int i,int j);
	int rangefield(int i,int j);
	int  thinkcollison(int i,int j,float r);
	void addTrackflag(int i,int j,int feild);

	//��ȡ��ײƫ��
	double GetColliShift(int i,int j,float radius);

	
	

protected:
	//�Զ���drawʵ��
	void onDraw(const cocos2d::Mat4 &transform, bool transformUpdated);
	cocos2d::CustomCommand _customCommand;

private:
	//����draw����
	virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
	void update(float dt);
	bool TrainNetwork();
	bool TestForMatch(std::vector<double> vecinput);
	std::vector<double> getNormalInput(int i,int j);
	RVO::Vector2 getANNoutputVec();
	void setNewAvoidVelocity(RVO::RVOSimulator *sim,int agenti,int agentj,int bestMatch,double collishift);
	
	Size s;
	RVO::RVOSimulator *sim;
	int agentno;
	SpriteBatchNode*  BatchNode;
	bool started;
	FILE *fp1;

	Vec2 DeltaAverrageVec,DeltaNormalVec,DeltaAverrageAllVec,drawvec,drawpos,SumVec,drawnormVec,agentveci,agentvecj,drawnormVeci,drawnormVecj;
	
	bool recorded,recorded2,ended,RecordDeltavec;

	int Veccount,countAll,tempfield,flag,orderflag,blue,green;

	float normallength,drawveclength,DeltaCAngel,iCAngel,jCAngel,agentvecinormlength,agentvecjnormlength;

	CCLabelBMFont *label,*label1,*label2,*label3,*label4,*labelangle;

	double P;

	//ANN
	std::vector<double> m_vecVectors;
	double  m_dHighestOutput;
	int     m_iMatch;
	int     m_iBestMatch;
	CNeuralNet*     m_pNet;
	CData*          m_pData;
	bool trained,finished,avoid,persisted;
	std::vector<std::vector<double> > SetIn;
	std::vector<std::vector<double> > SetOut;
	std::vector<RVO::Vector2> goals;
	float direc;
	float RealAngle;
};

#endif // __HELLOWORLD_SCENE_H__
