
#include <vector>
#include <math.h>
#include <fstream>
#include <windows.h>
#include "defines.h"
#include "CData.h"
#include "utils.h"
#include "CParams.h"
#pragma once

using namespace std;


typedef vector<double> iovector;
//-------------------------------------------------------------------
//	define neuron struct
//------------------------------------------------------------------



struct SNeuron
{
	//the number of inputs into the neuron
	int				      m_iNumInputs;

	//the weights for each input
	vector<double>	m_vecWeight;

	//the previous timesteps weight update used 
	//to add momentum
	vector<double>  m_vecPrevUpdate;

	//the activation of this neuron
	double          m_dActivation;

	//the error value
	 double          m_dError;
	//ctor
	SNeuron(int NumInputs);
};



//---------------------------------------------------------------------
//	struct to hold a layer of neurons.
//---------------------------------------------------------------------

struct SNeuronLayer
{
	//the number of neurons in this layer
	int					      m_iNumNeurons;

	//the layer of neurons
	vector<SNeuron>		m_vecNeurons;

	SNeuronLayer(int NumNeurons, 
		int NumInputsPerNeuron);
};


//----------------------------------------------------------------------
//	neural net class
//----------------------------------------------------------------------

class CNeuralNet
{
	
private:
	
	int					m_iNumInputs;

	int					m_iNumOutputs;

	int					m_iNumHiddenLayers;

	int					m_iNeuronsPerHiddenLyr;

	//we must specify a learning rate for backprop
	double      m_dLearningRate;

	//cumulative error for the network (sum (outputs - expected))
	 double      m_dErrorSum;

	//true if the network has been trained
	bool        m_bTrained;

	//epoch counter
	int         m_iNumEpochs;

	//storage for each layer of neurons including the output layer
	vector<SNeuronLayer>	m_vecLayers;



	void            CreateNet();

	//sets all the weights to small random values
	void            InitializeNetwork();

	//sigmoid response curve
	inline double	  Sigmoid(double activation,double response);

public:

	CNeuralNet::CNeuralNet(int    NumInputs,
		int    NumOutputs,
		int    HiddenNeurons,
		double LearningRate);
		
	vector<double>	Update(vector<double> inputs);
	bool            NetworkTrainingEpoch(vector<iovector> &SetIn,
		vector<iovector> &SetOut);
	bool            Train(vector<vector<double>>SetIn,vector<vector<double>> SetOut);

	
	bool            Trained(){return m_bTrained;}
	double          Error() {return m_dErrorSum;}



	

};
				

