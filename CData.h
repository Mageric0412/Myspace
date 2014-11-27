#ifndef DATA_H
#define DATA_H

//turn off the warnings for the STL
#pragma warning (disable : 4786)

#include <vector>
#include <iomanip>
#include <windows.h>

#include "defines.h"
#include "utils.h"
#pragma once

using namespace std;



//------------------------------------------------------------------------
//
//  class to handle the training data
//------------------------------------------------------------------------
class CData
{
private:

	vector<SPoint> position; 

	vector<SPoint> temp; 

	vector<vector<SPoint> > framepoint;

	vector<vector<vector<SVector2D> > > dimension; 


	vector<MyNum> SetinVector;//

	vector<vector<double> > Setintemp;//

	vector<vector<SVector2D> > dimensionperframe;

	vector<vector<MyNum> > SetinSum;//

	vector<SVector2D> perframeunit;

	vector<SVector2D> speedvector;

	vector<vector<SVector2D> > speedvectorsum;

	vector<vector<double> > Setoutmon;

	vector<double> Setoutnorm;

	vector<vector<vector<double> > > Setoutsum;

	vector< vector<MyNum>  > m_SetIn;

	vector<vector<vector<double> > > m_SetOut;


	vector<string>          m_vecNames;


	vector<vector<double> > m_vecPatterns;


	int            m_iNumPatterns;


	int            m_iVectorSize;


	void           Init();


public:
	int frameid[50][2];

	bool load;
	CData(int NumStartPatterns,
		int VectorSize)    :m_iNumPatterns(NumStartPatterns),
		m_iVectorSize(VectorSize)

	{
		Init();

		CreateTrainingSetFromData();
	}

	string  PatternName(int val);


	bool    AddData();


	void CreateTrainingSetFromData();

	vector< vector<MyNum>  > GetInputSet() {return m_SetIn;}

	vector<vector<vector<double> > > GetOutputSet(){return m_SetOut;}

	bool Loadtxt();

	vector<vector<SPoint> > Getframepoint(){return framepoint;}

	vector<SPoint> newpoint ; 

	vector<SVector2D> newspvet;

	vector<SVector2D>respvet;

	int GetVectorSize(){return m_iVectorSize;}
};

#endif
