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

#include "TMVA/Reader.h"
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"


#include "Event.h"
#include "Fit.h"

bool cut(const Event& event)
{
	bool keepHadronCut = event.nTrack > 0 ;
	bool cut = event.neutral == 0 && event.transverseRatio > 0.05f ;

	bool muonCut = ( 1.0*event.nInteractingLayer/event.nLayer > 0.2 ) && ( 1.0*event.nHit/event.nLayer > 3 ) ;

	bool beginCut = event.begin > -10 ;
	//	return ( cut && muonCut && keepHadronCut && beginCut ) ;
	return ( cut && muonCut && keepHadronCut && beginCut ) ;
}

int main()
{
	TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" ) ;

	float nHit ;
	float nHit1 ;
	float nHit2 ;
	float nHit3 ;
	float density ;
//	float nHitnHit ;
//	float energy ;
//	float emFraction ;
	float begin ;
	float end ;

	reader->AddVariable("nHit" , &nHit) ;
	reader->AddVariable("nHit1" , &nHit1) ;
	reader->AddVariable("nHit2" , &nHit2) ;
	reader->AddVariable("nHit3" , &nHit3) ;
	reader->AddVariable("density" , &density) ;
	reader->AddVariable("begin" , &begin) ;
	reader->AddVariable("end" , &end) ;
//	reader->AddVariable("emFraction" , &emFraction) ;
	//	reader->AddVariable("nHit*nHit" , &nHitnHit) ;

	TString methodName = "MLP method" ;

	reader->BookMVA( methodName , "/home/garillot/Code/SDHCALStuff/dataset/weights/TMVARegression_MLP.weights.xml" ) ;

	std::map<float,std::shared_ptr<TH1>> histoMap ;

	for ( int i = 10 ; i < 81 ; i += 10 )
	{
		std::stringstream fileName ; fileName << "/home/garillot/files/Analysis/Centered/OldDigit/Geant4.9.6/FTF_BIC/single_pi-_" << i << "GeV.root" ;
		TFile* file = TFile::Open( fileName.str().c_str() ) ;
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
		//		Event2 event ;
		eventReader.setTree(tree) ;
		//		event.setBranchAddress(tree) ;


		auto it = histoMap.find(i) ;

		if ( it == histoMap.end() )
		{
			std::stringstream histoName ; histoName << i << "GeV" ;
			histoMap[i] = std::shared_ptr<TH1>( new TH1D( histoName.str().c_str() , histoName.str().c_str() , 100 , 0 , 2*i ) ) ;
			histoMap[i]->SetDirectory(0) ;
			it = histoMap.find(i) ;
		}


		int iEntry = 0 ;
		while( tree->GetEntry(iEntry++) )
		{
			Event event = eventReader.getEvent(iEntry-1) ;
			if ( !cut(event) )
				continue ;

			nHit = event.nHit ;
			nHit1 = event.nHit1 ;
			nHit2 = event.nHit2 ;
			nHit3 = event.nHit3 ;
			density = static_cast<float>( event.density ) ;
//			nHitnHit = nHit*nHit ;
//			emFraction = static_cast<float>(event.emFraction) ;
			begin = static_cast<float>(event.begin) ;
			end = static_cast<float>(event.end) ;
			it->second->Fill( static_cast<double>(reader->EvaluateRegression( methodName )[0] ) ) ;
		}

		file->Close() ;
	}

	Fit* fit = new Fit ;
	fit->loadHistos(histoMap) ;
	fit->fitAllHistos() ;

	fit->drawLin() ;
	fit->drawLinDev() ;
	fit->drawResol() ;



	TFile* histoFile = new TFile("histo.root" , "RECREATE") ;
	histoFile->cd() ;

	for ( auto it = histoMap.begin() ; it != histoMap.end() ; ++it )
		it->second->Write() ;

	histoFile->Close() ;


	histoMap.clear() ;


	delete reader ;



	return 0 ;
}
