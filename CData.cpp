#include "CData.h"
#include "utils.h"
#include "defines.h"
#define txt "tracking.txt"



const  float scale=1.8;
double THRESHOLD=30;
double threspeed=40;
void CData::Init()
{  
	speedvectorsum.clear();
	dimension.clear();
	load=Loadtxt();
	if (load)
	{
		dimension.reserve(355);
		//  dimensionperframe.reserve(45);
		//  perframeunit.reserve(45);
		//将从txt载入的数据进行处理，构造一个包含每个点相邻点距离信息的数组
		for (int frame=1;frame<=350;frame++)
		{ 		 
			for (int  i=0;i<framepoint.size()-3;i++)
			{			
				for (int j=0; j<framepoint.size()-3; j++)
				{
					if (frame<frameid[i][0]||frame>=frameid[i][0]+frameid[i][1]||frame<frameid[j][0]||frame>=frameid[j][0]+frameid[j][1]||(i==j))
					{
						perframeunit.push_back(SVector2D(9999.0f,9999.0f));
					}
					else
					{
						perframeunit.push_back(julidistance(framepoint[i][frame-frameid[i][0]],framepoint[j][frame-frameid[j][0]]));
					}		  

				}
				dimensionperframe.push_back(perframeunit);
				perframeunit.clear();
			}

			dimension.push_back(dimensionperframe);
			dimensionperframe.clear();
		}


		//speedvectorsum.reserve(45);
		//构造包含每个点速度信息的数组
		speedvector.reserve(355);
		for (int i=0;i<framepoint.size()-3;i++)
		{
			for (int frame=1;frame<=350;frame++)
			{
				if (frame<frameid[i][0]+4||frame>=frameid[i][0]+frameid[i][1])
				{
					speedvector.push_back(SVector2D(0,0));
				}
				else
				{
					speedvector.push_back(julidistance(framepoint[i][frame-frameid[i][0]-4],framepoint[i][frame-frameid[i][0]]));
					//double r=Vec2DLength(julidistance(framepoint[i][frame-frameid[i][0]],framepoint[i][frame-frameid[i][0]+10]));

					//if (max[i]<Vec2DLength(speedvector.back())) max[i]=Vec2DLength(speedvector.back());

					// if (max2[i]<r)  max2[i]=r;
				}

			}
			speedvectorsum.push_back(speedvector);
			speedvector.clear();
		}

	}



}


bool CData:: Loadtxt()	
{   

	vector<float> vet;
	vector<float>::iterator ite1;
	FILE *fp;
	float w;
	int i=0;
	if((fp=fopen(txt,"r"))==NULL)
		return false;

	else
	{ vet.reserve(15000);
	while(!feof(fp))
	{
		fscanf(fp,"%f",&w);
		vet.push_back(w);		
	}
	}

	fclose(fp);

	float kk=vet.back();
	ite1=vet.begin();
	while(*ite1!=kk)
	{   
		int id=int(ceil(*ite1));
		int start=int(ceil(*(++ite1)));
		int frame=int(ceil(*(++ite1)));
		frameid[id-1][0]=start;
		frameid[id-1][1]=frame;
		position.reserve(400);
		while(frame--)
		{  
			position.push_back(SPoint(*(ite1+1)*scale+10,*(ite1+2)*scale+55));
			ite1+=2;
		}
		ite1++;
		temp=position;
		framepoint.push_back(temp);
		position.clear();

	}
	vet.clear();
	vector<float> (vet).swap(vet);
	return true;

}

//对每个点一定阈值范围内相邻点进行向量相乘，获取乘积与余弦值，靠运动方向左的乘积为负，右为正。
// 不到5个相邻点则处理为0
void CData::CreateTrainingSetFromData()
{  SetinSum.clear();
Setoutsum.clear();
m_SetIn.clear();
m_SetOut.clear();
m_SetIn.reserve(355);
m_SetOut.reserve(355);
//SetinSumframe.reserve(45);
SetinSum.reserve(355);
for (int frame=1;frame<=350;frame++)
{
	for (int i=0;i<framepoint.size()-3;i++)
	{  

		SVector2D transsp;
		transsp=speedvectorsum[i][frame-1];
		if(Vec2DLength(transsp)!=0)
			Vec2DNormalize(transsp);
		//SVector2D transsp=Vec2DNormalize3(speedvectorsum[i][frame-1]);
		double dcos=transsp.y;
		double lr=(transsp.x>0? 1:-1);
		double dsin=lr*sqrt(1-dcos*dcos);
		SVector2D transvec;
		MyNum num;
		for (int i = 0; i < 9; i++)
		{
			num.VecNum[i]=0;
		}

		for (int j=0;j<framepoint.size()-3;j++)
		{  

			SVector2D oldvec=dimension[frame-1][i][j];
			transvec.x=oldvec.x*dcos-oldvec.y*dsin;
			transvec.y=oldvec.x*dsin+oldvec.y*dcos;

			if(((int)(transvec.x/THRESHOLD)==-1||(int)(transvec.x/THRESHOLD)==-2)&&((int)(transvec.y/THRESHOLD)==1||(int)(transvec.y/THRESHOLD)==2))
			{ num.VecNum[0]+=0.3;  continue; }
			else if(((int)(transvec.x/THRESHOLD)==0)&&((int)(transvec.y/THRESHOLD)==1||(int)(transvec.y/THRESHOLD)==2))
			{ num.VecNum[1]+=0.3;  continue;}
			else if(((int)(transvec.x/THRESHOLD)==1||(int)(transvec.x/THRESHOLD)==2)&&((int)(transvec.y/THRESHOLD)==1||(int)(transvec.y/THRESHOLD)==2))
			{  num.VecNum[2]+=0.3; continue; }
			else if(((int)(transvec.x/THRESHOLD)==-1||(int)(transvec.x/THRESHOLD)==-2)&&(int)(transvec.y/THRESHOLD)==0)
			{  num.VecNum[3]+=0.3; continue; }
			else if(((int)(transvec.x/THRESHOLD)==0)&&((int)(transvec.y/THRESHOLD)==0))
			{  num.VecNum[4]+=0.3; continue; }
			else if(((int)(transvec.x/THRESHOLD)==1||(int)(transvec.x/THRESHOLD)==2)&&((int)(transvec.y/THRESHOLD)==0))
			{  num.VecNum[5]+=0.3; continue; }
			else if(((int)(transvec.x/THRESHOLD)==-1||(int)(transvec.x/THRESHOLD)==-2)&&((int)(transvec.y/THRESHOLD)==-1||(int)(transvec.y/THRESHOLD)==-2))
			{  num.VecNum[6]+=0.3; continue; }
			else if(((int)(transvec.x/THRESHOLD)==0)&&((int)(transvec.y/THRESHOLD)==-1||(int)(transvec.y/THRESHOLD)==-2))
			{  num.VecNum[7]+=0.3; continue; }
			else if(((int)(transvec.x/THRESHOLD)==1||(int)(transvec.x/THRESHOLD)==2)&&((int)(transvec.y/THRESHOLD)==-1||(int)(transvec.y/THRESHOLD)==-2))
			{  num.VecNum[8]+=0.3; continue; }

		}

		SetinVector.push_back(num);		 
	}

	SetinSum.push_back(SetinVector);
	SetinVector.clear();

}
Setoutmon.reserve(45);
Setoutsum.reserve(355);
//对将来4帧之后的运动速度进行计算，与当前速度相乘，计算夹角余弦，乘积有正负。
for (int frame=1;frame<=350;frame++)
{
	for (int i=0;i<framepoint.size()-3;i++)
	{
		if (frame<frameid[i][0]||frame>=frameid[i][0]+frameid[i][1]-4||speedvectorsum[i][frame-1].x==0)
		{
			Setoutnorm.push_back(0);
			Setoutnorm.push_back(0);
			Setoutmon.push_back(Setoutnorm);
			Setoutnorm.clear();
		}
		else 
		{   	
			SVector2D sv=julidistance(framepoint[i][frame-frameid[i][0]],framepoint[i][frame-frameid[i][0]+4])-speedvectorsum[i][frame-1]; 
			//double out=Vec2DNormalize2(sv,threspeed)*Vec2DNormalize2(speedvectorsum[i][frame-1],threspeed);
			SVector2D nowspd;
			nowspd=speedvectorsum[i][frame-1];

			if(Vec2DLength(nowspd)!=0)
				Vec2DNormalize(nowspd);
			//SVector2D transsp=Vec2DNormalize3(speedvectorsum[i][frame-1]);
			double dcosout=nowspd.y;
			double lr=(nowspd.x>0? 1:-1);
			double dsinout=lr*sqrt(1-dcosout*dcosout);

			SVector2D newtranspd;
			newtranspd.x=sv.x*dcosout-sv.y*dsinout;
			newtranspd.y=sv.x*dsinout+sv.y*dcosout;

			SVector2D normaltranspd;
			normaltranspd=Vec2DNormalize2(newtranspd,Vec2DLength(speedvectorsum[i][frame-1]));
			Setoutnorm.push_back(normaltranspd.x);		
			Setoutnorm.push_back(normaltranspd.y);

			Setoutmon.push_back(Setoutnorm);
			Setoutnorm.clear();
		}

	}
	Setoutsum.push_back(Setoutmon);
	Setoutmon.clear();
}

for (int i=0;i<350;i++)
{
	m_SetIn.push_back(SetinSum[i]);
}

for (int i = 0; i <350; i++)
{
	m_SetOut.push_back(Setoutsum[i]);
}



}


bool CData::AddData()
{
	newpoint.clear();
	newspvet.clear();
	respvet.clear();
	for (int i=0;i<m_iVectorSize/2;i++)
	{
		newpoint.push_back(SPoint(RandFloat()*100+200,RandFloat()*400+200));
		newspvet.push_back(SVector2D(RandFloat()*10,RandFloat()*4-2));
	}

	for (int j=m_iVectorSize/2;j<m_iVectorSize;j++)
	{
		newpoint.push_back(SPoint(RandFloat()*400+200,RandFloat()*400+200));
		newspvet.push_back(SVector2D(RandFloat()*(-10),RandFloat()*4-2));
	}

	respvet=newspvet;
	return true;
}
