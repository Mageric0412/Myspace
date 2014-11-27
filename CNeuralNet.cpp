#include "CNeuralNet.h"
#include <windows.h>


//*************************** methods for Neuron **********************
//
//---------------------------------------------------------------------
SNeuron::SNeuron(int NumInputs): m_iNumInputs(NumInputs+1),m_dActivation(0),m_dError(0)
											
{
	//we need an additional weight for the bias hence the +1
	for (int i=0; i<NumInputs+1; i++)
	{
		//set up the weights with an initial random value
		m_vecWeight.push_back(RandomClamped());
		m_vecPrevUpdate.push_back(0);
	}
}




//************************ methods for NeuronLayer **********************

//-----------------------------------------------------------------------
//	ctor creates a layer of neurons of the required size by calling the 
//	SNeuron ctor the rqd number of times
//-----------------------------------------------------------------------
SNeuronLayer::SNeuronLayer(int NumNeurons, 
                           int NumInputsPerNeuron):	m_iNumNeurons(NumNeurons)
{
	for (int i=0; i<NumNeurons; i++)

		m_vecNeurons.push_back(SNeuron(NumInputsPerNeuron));
}




//************************ methods forCNeuralNet ************************

//------------------------------default ctor ----------------------------
//
//	creates a ANN based on the default values in defines.h
//-----------------------------------------------------------------------
CNeuralNet::CNeuralNet(int NumInputs,
					   int NumOutputs,
					   int HiddenNeurons,
					   double LearningRate) : m_iNumInputs(NumInputs),
					                         m_iNumOutputs(NumOutputs),
					                         m_iNumHiddenLayers(1),
					                         m_iNeuronsPerHiddenLyr(HiddenNeurons),
					                         m_dLearningRate(LearningRate),
					                         m_dErrorSum(9999),
					                         m_bTrained(false),
					                         m_iNumEpochs(0)
{


	CreateNet();

}


//------------------------------createNet()------------------------------
//
//	this method builds the ANN. The weights are all initially set to 
//	random values -1 < w < 1
//------------------------------------------------------------------------
void CNeuralNet::CreateNet()
{
	//create the layers of the network
	if (m_iNumHiddenLayers > 0)
	{
		//create first hidden layer
	  m_vecLayers.push_back(SNeuronLayer(m_iNeuronsPerHiddenLyr, m_iNumInputs));
    
    for (int i=0; i<m_iNumHiddenLayers-1; i++)
    {

			m_vecLayers.push_back(SNeuronLayer(m_iNeuronsPerHiddenLyr,
                                         m_iNeuronsPerHiddenLyr));
    }

    //create output layer
	  m_vecLayers.push_back(SNeuronLayer(m_iNumOutputs, m_iNeuronsPerHiddenLyr));
	}

  else
  {
	  //create output layer
	  m_vecLayers.push_back(SNeuronLayer(m_iNumOutputs, m_iNumInputs));
  }
}


void CNeuralNet::InitializeNetwork()
{
	for(int i=0;i<m_iNumHiddenLayers+1;i++)
	{
		for (int n=0;n<m_vecLayers[i].m_iNumNeurons;n++)
		{
			for (int k=0;k<m_vecLayers[i].m_vecNeurons[n].m_iNumInputs;k++)
			{
				m_vecLayers[i].m_vecNeurons[n].m_vecWeight[k]=RandomClamped();
			}
		}
	}
	m_dErrorSum=9999;
	m_iNumEpochs=0;

}

//-------------------------------Update-----------------------------------
//
//	given an input vector this function calculates the output vector
//
//------------------------------------------------------------------------
vector<double> CNeuralNet::Update(vector< double> inputs)
{
	//stores the resultant outputs from each layer
	vector<double> outputs;
	outputs.clear();
	for (int k=0;k<inputs.size();k++)
	{
		inputs[k]+=RandFloat()*MAX_NOISE_TO_ADD;
	}
	int cWeight = 0;
	
	//first check that we have the correct amount of inputs
	if (inputs.size() != m_iNumInputs)
  {
		//just return an empty vector if incorrect.
		return outputs;
  }
	
	//For each layer....
	for (int i=0; i<m_iNumHiddenLayers + 1; i++)
	{
		if ( i > 0 )
    {
			inputs = outputs;
    }

		outputs.clear();
		
		cWeight = 0;

		//for each neuron sum the (inputs * corresponding weights).Throw 
		//the total at our sigmoid function to get the output.
		for (int j=0; j<m_vecLayers[i].m_iNumNeurons; j++)
		{

			double netinput = 0.0f;

			int	NumInputs = m_vecLayers[i].m_vecNeurons[j].m_iNumInputs;
			
			//for each weight
			for (int k=0; k<NumInputs - 1; k++)
			{
				//sum the weights x inputs
				netinput += m_vecLayers[i].m_vecNeurons[j].m_vecWeight[k] * 
                    inputs[cWeight++];
			}

			//add in the bias
			netinput += m_vecLayers[i].m_vecNeurons[j].m_vecWeight[NumInputs-1] * 
                  CParams::dBias;

			m_vecLayers[i].m_vecNeurons[j].m_dActivation=Sigmoid(netinput,CParams::dActivationResponse);
			
			outputs.push_back(m_vecLayers[i].m_vecNeurons[j].m_dActivation);

			cWeight = 0;
		}
	}
	inputs.clear();
	return outputs;
}

bool CNeuralNet::NetworkTrainingEpoch(vector<vector<MyNum> > &SetIn, vector<vector<iovector> > &SetOut)
{
	vector<double> input;
	double sumi=0;
	//int fr=RandInt(5,SetIn.size()-1);
	//int vec=RandInt(3,SetIn[0].size()-10);
    
	//for (int fr=0;fr<SetIn.size();fr++)
   // {
	// int fr=RandInt(3,SetIn.size());
	//for (int vec=0;vec<SetIn[fr].size();vec++)
	//{   
	  //int wh=5;

	  int fr=RandInt(2,SetIn.size()-1);
	  int vec=RandInt(3,SetIn[0].size()-10);
	 
	// while (wh)
  // {
		vector< double>::iterator curWeight;
		vector<SNeuron>::iterator curNrnOut,curNrnHid;
		for (int i=0;i<9;i++)
		{
			
			   input.push_back(SetIn[fr][vec].VecNum[i]);
		    
		}

		for (int i=0;i<input.size();i++)
		{
			sumi+=abs(input[i]);
		}

		if (sumi==0)
		{
			return true;
		}

		double WeightUpdate=0;
		m_dErrorSum=0;

		vector<double> outputs=Update(input);

		if(outputs.size()==0) return false;

		for (int op=0;op<m_iNumOutputs;op++)
		{
			double err=(SetOut[fr][vec][op]-outputs[op])*outputs[op]*(1-outputs[op]);
			
			m_dErrorSum+=(SetOut[fr][vec][op]-outputs[op])*(SetOut[fr][vec][op]-outputs[op]);
			
			m_vecLayers[1].m_vecNeurons[op].m_dError=err;
			
			curWeight=m_vecLayers[1].m_vecNeurons[op].m_vecWeight.begin();
			
			curNrnHid=m_vecLayers[0].m_vecNeurons.begin();

			int w=0;

			while (curWeight!=m_vecLayers[1].m_vecNeurons[op].m_vecWeight.end()-1)
			{
				WeightUpdate=err*m_dLearningRate*curNrnHid->m_dActivation;
				
				*curWeight+=WeightUpdate+m_vecLayers[1].m_vecNeurons[op].m_vecPrevUpdate[w]*MOMENTUM;
				
				m_vecLayers[1].m_vecNeurons[op].m_vecPrevUpdate[w]=WeightUpdate;
				
				++curWeight;
				++curNrnHid;
                ++w;
			}
			WeightUpdate = err * m_dLearningRate * CParams::dBias;

			*curWeight += WeightUpdate + m_vecLayers[1].m_vecNeurons[op].m_vecPrevUpdate[w] * MOMENTUM;  

			m_vecLayers[1].m_vecNeurons[op].m_vecPrevUpdate[w] = WeightUpdate;
		}

		curNrnHid = m_vecLayers[0].m_vecNeurons.begin();

		int n = 0;

		while(curNrnHid != m_vecLayers[0].m_vecNeurons.end())
		{
			double err = 0;

			curNrnOut = m_vecLayers[1].m_vecNeurons.begin();

			
			while(curNrnOut != m_vecLayers[1].m_vecNeurons.end())
			{
				err += curNrnOut->m_dError * curNrnOut->m_vecWeight[n];

				++curNrnOut;
			}

			
			err *= curNrnHid->m_dActivation * (1 - curNrnHid->m_dActivation);     

			
			for (int w=0; w<m_iNumInputs; w++)
			{
				WeightUpdate = err * m_dLearningRate * input[w];

				curNrnHid->m_vecWeight[w] += WeightUpdate + curNrnHid->m_vecPrevUpdate[w] * MOMENTUM;

			
				curNrnHid->m_vecPrevUpdate[w] = WeightUpdate;

			}

			
			WeightUpdate = err * m_dLearningRate * CParams::dBias;

			curNrnHid->m_vecWeight[m_iNumInputs] += WeightUpdate + curNrnHid->m_vecPrevUpdate[m_iNumInputs] * MOMENTUM;
			
			curNrnHid->m_vecPrevUpdate[m_iNumInputs] = WeightUpdate;

			++curNrnHid;
			++n;
		}

		input.clear();
		outputs.clear();
	// }//内循环

	//}//外循环
	//	wh--;
  // }
	 return true;
}

bool CNeuralNet::Train(CData* data)
{
	vector< vector<MyNum> >  SetIn =data->GetInputSet();
	vector<vector<vector<double> > > SetOut=data->GetOutputSet();

	InitializeNetwork();

	while(m_dErrorSum>ERROR_THRESHOLD)
	{
		if (!NetworkTrainingEpoch(SetIn,SetOut))
		{
			return false;
		}
		++m_iNumEpochs;
	  Sleep(30);
	}
	m_bTrained=true;
	return true;
}


double CNeuralNet::Sigmoid( double netinput, double response)
{
	return (1/(1+exp(-netinput/response)));
}


