#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TMVA/DataLoader.h"

#include "Event.h"


bool cut(const Event& event)
{
	bool keepHadronCut = event.nTrack > 0 ;
	bool cut = event.neutral == 0 && event.transverseRatio > 0.05f ;

	bool muonCut = ( 1.0*event.nInteractingLayer/event.nLayer > 0.2 ) && ( 1.0*event.nHit/event.nLayer > 3 ) ;

//	bool beginCut = event.begin < 1000 ;
	//	return ( cut && muonCut && keepHadronCut && beginCut ) ;
	return ( cut && muonCut && keepHadronCut ) ;
}

int main( )
{
	std::cout << std::endl ;
	std::cout << "==> Start TMVARegression" << std::endl ;

	// --- Here the preparation phase begins

	TString outfileName( "TMVAReg.root" ) ;
	TFile* outputFile = TFile::Open( outfileName, "RECREATE" ) ;

	TMVA::Factory* factory = new TMVA::Factory( "TMVARegression", outputFile, "!V:!Silent:Color:DrawProgressBar:AnalysisType=Regression" ) ;

	// Define the input variables that shall be used for the MVA training


	TTree* trainTree = new TTree("trainTree" , "trainTree") ;
	trainTree->SetDirectory(0) ;
	TTree* testTree = new TTree("testTree" , "testTree") ;
	testTree->SetDirectory(0) ;



	TFile* file = TFile::Open("/home/garillot/files/Analysis/Centered/OldDigit/Geant4.9.6/FTF_BIC/forNN_pi-_1-120GeV.root") ;
	if ( !file )
	{
		std::cerr << "ERROR : could not open data file" << std::endl ;
		return 1 ;
	}
	TTree* tree = dynamic_cast<TTree*>( file->Get("tree") ) ;
	if ( !tree )
	{
		std::cerr << "ERROR : tree not present" << std::endl ;
		return 1 ;
	}

	EventReader eventReader ;
	//	Event2 event ;
	eventReader.setTree(tree) ;
	//	event.setBranchAddress(tree) ;

	eventReader.setTreeWrite(trainTree) ;
	eventReader.setTreeWrite(testTree) ;

	int iEntry = 0 ;
	while( tree->GetEntry( iEntry++ ) )
	{
		Event event = eventReader.getEvent(iEntry-1) ;
		if ( !cut(event) )
			continue ;


		trainTree->Fill() ;
	}
	file->Close() ;

	file = TFile::Open("/home/garillot/files/Analysis/Centered/OldDigit/Geant4.9.6/FTF_BIC/uniform_pi-_1-90GeV.root") ;
	if ( !file )
	{
		std::cerr << "ERROR : could not open data file" << std::endl ;
		return 1 ;
	}
	tree = dynamic_cast<TTree*>( file->Get("tree") ) ;
	if ( !tree )
	{
		std::cerr << "ERROR : tree not present" << std::endl ;
		return 1 ;
	}

	//	EventReader eventReader ;
	//	Event2 event ;
	eventReader.setTree(tree) ;
	//	event.setBranchAddress(tree) ;

	iEntry = 0 ;
	while( tree->GetEntry( iEntry++ ) )
	{
		Event event = eventReader.getEvent(iEntry-1) ;
		if ( !cut(event) )
			continue ;


		testTree->Fill() ;
	}
	file->Close() ;

	TMVA::DataLoader* dataloader = new TMVA::DataLoader("dataset") ;


	dataloader->AddVariable("nHit" , 'F') ;
	dataloader->AddVariable("nHit1" , 'F') ;
	dataloader->AddVariable("nHit2" , 'F') ;
	dataloader->AddVariable("nHit3" , 'F') ;
	dataloader->AddVariable("density" , 'F') ;
//	dataloader->AddVariable("begin" , 'F') ;
	dataloader->AddVariable("end" , 'F') ;

	//	dataloader->AddVariable("emFraction" , 'F') ;


	dataloader->AddTarget( "energy" ) ;


	// global event weights per tree (see below for setting event-wise weights)
	Double_t regWeight  = 1.0 ;

	// You can add an arbitrary number of regression trees
	dataloader->AddRegressionTree( trainTree , regWeight , TMVA::Types::kTraining ) ;
	dataloader->AddRegressionTree( testTree , regWeight , TMVA::Types::kTesting ) ;


	// This would set individual event weights (the variables defined in the
	// expression need to exist in the original TTree)
	//   factory->SetWeightExpression( "var1", "Regression" ) ;

	// Apply additional cuts
	TCut mycut = "" ;

	// tell the factory to use all remaining events in the trees after training for testing:

//	dataloader->PrepareTrainingAndTestTree( mycut, "nTrain_Regression=200000:nTest_Regression=200000:SplitMode=Block:NormMode=NumEvents:!V" ) ;
	dataloader->PrepareTrainingAndTestTree( mycut, "SplitMode=Block:NormMode=NumEvents:!V" ) ;

	const char* nCycles = "3000" ;
	const char* networkStructure = "10,5" ;
	const char* batchSize = "1" ;

	std::stringstream toto ;
	toto << "!H:!V:VarTransform=N,G,D:NeuronType=tanh:" ;
	toto << "NCycles=" << nCycles << ":" ;
	toto << "HiddenLayers=" << networkStructure << ":" ;
	toto << "TestRate=1:TrainingMethod=BFGS:" ;
	toto << "BPMode=batch:BatchSize=" << batchSize << ":" ;
	toto << "ConvergenceImprove=1e-8:ConvergenceTests=50:UseRegulator=True:" ;
	toto << "CalculateErrors=false" ;

	factory->BookMethod( dataloader, TMVA::Types::kMLP, "MLP", toto.str().c_str() ) ;


	// ---- Now you can tell the factory to train, test, and evaluate the MVAs
	factory->TrainAllMethods() ;
	factory->TestAllMethods() ;
	factory->EvaluateAllMethods() ;

	outputFile->Close() ;

	std::cout << "==> Wrote root file : " << outputFile->GetName() << std::endl ;
	std::cout << "==> TMVARegression is done !" << std::endl ;

	delete factory ;
	delete dataloader ;

	//	system ("cp weights/TMVARegression_MLP.weights.xml .") ;
	return 0 ;
}
