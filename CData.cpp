#include "CData.h"



//------------------------------------------------------------------------
//
//  constant training data
//------------------------------------------------------------------------
const double InputVectors[NUM_PATTERNS][NUM_VECTORS*2]=
{
	0.939,0.342, 0.939,0.342,0.939,0.342, 0.939,0.342,  0.939,0.342,  0.939,0.342,  //20度

	0.866,0.5, 0.866,0.5, 0.866,0.5, 0.866,0.5, 0.866,0.5, 0.866,0.5,    //30度

	0.707,0.707, 0.707,0.707,0.707,0.707, 0.707,0.707, 0.707,0.707, 0.707,0.707,   //45度

	0.584,0.812,0.584,0.812,0.584,0.812,0.584,0.812, 0.584,0.812, 0.584,0.812,     //分水岭54.3度

	0.5,0.866, 0.5,0.866, 0.5,0.866, 0.5,0.866, 0.5,0.866, 0.5,0.866,  //60度

	0.259,0.966, 0.259,0.966,0.259,0.966, 0.259,0.966, 0.259,0.966, 0.259,0.966,     //75度

	0,1, 0,1, 0,1, 0,1,  0,1,  0,1,   //90度

	-0.259,0.996, -0.259,0.996,-0.259,0.996, -0.259,0.996, -0.259,0.996,-0.259,0.996,   //105度

	-0.5,0.866, -0.5,0.866,-0.5,0.866,-0.5,0.866, -0.5,0.866,-0.5,0.866,//120度

	-0.707,0.707, -0.707,0.707, -0.707,0.707, -0.707,0.707, -0.707,0.707, -0.707,0.707, //135度

	-0.866,0.5,-0.866,0.5,-0.866,0.5, -0.866,0.5,-0.866,0.5,-0.866,0.5,//150度

	-0.966,0.259, -0.966,0.259, -0.966,0.259, -0.966,0.259, -0.966,0.259, -0.966,0.259, //165度

	-1,0, -1,0,-1,0,-1,0, -1,0,-1,0,   //180度

}; 

const double Names[NUM_PATTERNS][NUM_VECOUT]=
{
	8.6565,7.9827,1.0474,0.8723,//20

	8.5571,6.4580,1.1188,0.8032,//30

	9.1624,2.4132,1.1574,0.7445,//45

	10.2452,0.0396,1.1697,0.7304,//54.3

	9.9226,1.2410,1.1631,0.7273,//60

	10.9472,5.3067,1.1625,0.7268,//75

	12.3767,7.0980,1.1520,0.7370,//90

	12.9160,9.0594,1.1263,0.7663,//105

	14.9214,12.5060,1.1017,0.7752,//120

	16.8136,15.2285,1.0613,0.7957,//135

	18.3444,17.2689,1.0208,0.8293,//150

	19.1562,18.9033,0.9663,0.8820,//165

	20.2038,20.0835,0.9218,0.9174,//180

};


//--------------------------------- Init ---------------------------------
//
//  Initializes the appropriate vectors with the const training data
//------------------------------------------------------------------------
void CData::Init()
{
	//for each const pattern  
	for (int ptn=0; ptn<m_iNumPatterns; ptn++)
	{
		//add it to the vector of patterns
		vector<double> temp;
		vector<double> temp2;
		for (int v=0; v<m_iVectorSize*2; ++v)
		{
			temp.push_back(InputVectors[ptn][v]);
		}

		m_vecPatterns.push_back(temp);

		for (int i=0;i<NUM_VECOUT;i++)
		{
			temp2.push_back(Names[ptn][i]);
		}
		//输出向量的具体选择
	    m_Vecout.push_back(temp2);
	}

}


// --------------------------- CreateTrainingSetFromData -----------------
//
//  this function creates a training set from the data defined as constants
//  in CData.h. 
//------------------------------------------------------------------------
void CData::CreateTrainingSetFromData()
{
	//empty the vectors
	m_SetIn.clear();
	m_SetOut.clear();

	//add each pattern
	for (int ptn=0; ptn<m_iNumPatterns; ++ptn)
	{    
		//add the data to the training set
		m_SetIn.push_back(m_vecPatterns[ptn]);
		//std::vector with zeros
		vector<double> outputs(m_iNumPatterns, 0);
		//set the relevant output to 1
		//输出向量选择
		outputs[ptn] = 1;
		//add it to the output set
		m_SetOut.push_back(outputs);
	}
}

vector<double> CData::PatternName(int val)
{
	if (m_Vecout.size() > 0)
	{
		return m_Vecout[val];
		//return newNames[val];
	}

	else
	{
		//CCLOG("outpatternError");
	}
}


bool CData::AddData(vector<double> &data, vector<double> New)
{
	//check that the size is correct
	

	//add the name
	m_Vecout.push_back(New);

	//add the data
	m_vecPatterns.push_back(data);

	//keep a track of number of patterns
	++m_iNumPatterns;

	//create the new training set
	CreateTrainingSetFromData();

	return true; 
}

