#include <iostream>
#include <map>
#include <sstream>

#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>
#include <TGraphErrors.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <THStack.h>
#include <TLegend.h>
#include <TLine.h>
#include <TDirectory.h>

#include "TimeCorrection.h"
#include "Event.h"


bool cut(const Event& event)
{
//	bool keepElectronCut = event.begin < 5*26.131 && event.nLayer < 30 && event.nTrack == 0 ;
	bool keepHadronCut = event.nTrack > 0 ;
	bool cut = event.neutral == 0 && event.transverseRatio > 0.05 ;

	//	bool geomCut = std::sqrt( (event.cog[0]-400)*(event.cog[0]-400) + (event.cog[2]-510)*(event.cog[2]-510) ) < 100 ;
	//	bool geomCut = std::sqrt( (event.cog[0]-570)*(event.cog[0]-570) + (event.cog[2]-505)*(event.cog[2]-505) ) < 50 ;
	bool geomCut = std::sqrt( (event.cog[0]-490)*(event.cog[0]-490) + (event.cog[2]-460)*(event.cog[2]-460) ) < 100 ;

	bool muonCut = ( 1.0*event.nInteractingLayer/event.nLayer > 0.2 ) && ( 1.0*event.nHit/event.nLayer > 3 ) ;
	bool timeCut = event.spillEventTime < 30e6 ;
	//	bool timeCut = event.spillEventTime*200e-9 < 1.2 ;

	//	return ( cut && geomCut && muonCut && timeCut && keepElectronCut ) ;
	return ( cut && geomCut && muonCut && timeCut && keepHadronCut ) ;
}

bool cutSim(const Event& event)
{
//	bool keepElectronCut = event.begin < 5*26.131 && event.nLayer < 30 && event.nTrack == 0 ;
	bool keepHadronCut = event.nTrack > 0 ;
	bool cut = event.neutral == 0 && event.transverseRatio > 0.05 ;

	//	bool geomCut = std::sqrt( (event.cog[0]-400)*(event.cog[0]-400) + (event.cog[2]-510)*(event.cog[2]-510) ) < 100 ;
	//	bool geomCut = std::sqrt( (event.cog[0]-570)*(event.cog[0]-570) + (event.cog[2]-505)*(event.cog[2]-505) ) < 50 ;
	bool geomCut = std::sqrt( (event.cog[0]-490)*(event.cog[0]-490) + (event.cog[2]-460)*(event.cog[2]-460) ) < 100 ;

	bool muonCut = ( 1.0*event.nInteractingLayer/event.nLayer > 0.2 ) && ( 1.0*event.nHit/event.nLayer > 3 ) ;
	bool timeCut = event.spillEventTime < 30e6 ;
	//	bool timeCut = event.spillEventTime*200e-9 < 1.2 ;

	//	return ( cut && geomCut && muonCut && timeCut && keepElectronCut ) ;
	return ( cut && geomCut && muonCut && timeCut && keepHadronCut ) ;
}



int main()
{

	std::map<int , int> runMap ;
	//	runMap.insert( std::make_pair(10 , 728348) ) ;
	//	runMap.insert( std::make_pair(15 , 728350) ) ;
	//	runMap.insert( std::make_pair(20 , 728203) ) ;
	//	runMap.insert( std::make_pair(25 , 728352) ) ;
	//	runMap.insert( std::make_pair(30 , 728354) ) ;
	//	runMap.insert( std::make_pair(40 , 728357) ) ;
	//	runMap.insert( std::make_pair(50 , 728359) ) ;
	//	runMap.insert( std::make_pair(85 , 728360) ) ;

	//	runMap.insert( std::make_pair(10 , 715725) ) ;
	//	runMap.insert( std::make_pair(20 , 715715) ) ;
	//	runMap.insert( std::make_pair(30 , 715714) ) ;
	//	runMap.insert( std::make_pair(40 , 715713) ) ;
	//	runMap.insert( std::make_pair(50 , 715716) ) ;

	runMap.insert( std::make_pair(10 , 730716) ) ;
	runMap.insert( std::make_pair(20 , 730656) ) ;
	runMap.insert( std::make_pair(30 , 730634) ) ;
	runMap.insert( std::make_pair(40 , 730657) ) ;
	runMap.insert( std::make_pair(50 , 730651) ) ;
	runMap.insert( std::make_pair(60 , 730655) ) ;
	runMap.insert( std::make_pair(70 , 730659) ) ;
	runMap.insert( std::make_pair(80 , 730677) ) ;
	//		runMap.insert( std::make_pair(90 , 730676) ) ;

	//		runMap.insert( std::make_pair(10 , 730903) ) ;
	//		runMap.insert( std::make_pair(20 , 730888) ) ;
	//		runMap.insert( std::make_pair(30 , 730886) ) ;
	//		runMap.insert( std::make_pair(40 , 730882) ) ;
	//		runMap.insert( std::make_pair(50 , 730861) ) ;
	//		runMap.insert( std::make_pair(60 , 730858) ) ;
	//		runMap.insert( std::make_pair(70 , 730851) ) ;
	//		runMap.insert( std::make_pair(80 , 730847) ) ;


	TFile* resultFile = new TFile("result96.root" , "RECREATE") ;
	resultFile->cd() ;
	TDirectory* histosDir = resultFile->mkdir("Histos") ;
	TDirectory* graphsDir = resultFile->mkdir("Graphs") ;

	histosDir->mkdir("nHit") ;
	histosDir->mkdir("LongiProfile") ;
	histosDir->mkdir("RadiProfile") ;


	//	double beginTime = 0 ;
	unsigned long long beginTime = 5e6 ;
	TimeCorrection timeCorr ;
	timeCorr.setBeginTime(beginTime) ;

	std::map<int , TH1D*> nHitHistDataMap ;
	std::map<int , TH1D*> longiProfHistDataMap ;
	std::map<int , TH1D*> radiProfHistDataMap ;
	std::map<int , double> radiMeanDataMap ;

	std::map<int , double> nHitErrMap ;

	EventReader eventReader ;
//	Event event ;

	for ( std::map<int , int>::const_iterator it = runMap.begin() ; it != runMap.end() ; ++it )
	{
		std::stringstream data ;
		//				data << "/home/garillot/files/DATA/RootFile/electrons/" << it->second << ".root" ;
		//		data << "/home/garillot/files/Analysis/DATA/" << it->second << ".root" ;
		data << "/home/garillot/files/Analysis/DATA/" << it->second << ".root" ;
		TFile* dataFile = TFile::Open( data.str().c_str() , "READ") ;
		if ( !dataFile )
		{
			std::cerr << "File " << data.str() << " not present" << std::endl ;
			continue ;
		}
		TTree* dataTree = dynamic_cast<TTree*>( dataFile->Get("tree") ) ;
		if ( !dataTree )
		{
			std::cerr << "Tree not present in " << data.str() << std::endl ;
			continue ;
		}


		eventReader.setTree(dataTree) ;
//		event.setBranchAddress(dataTree) ;

		std::vector<Event> temp ;


		int iEntry = 0 ;
		while ( dataTree->GetEntry(iEntry++) )
		{
			Event event = eventReader.getEvent(iEntry-1) ;

			if ( !cut(event) )
				continue ;

			temp.push_back(event) ;
		}


		HistoCreator* histoCreator = new HistoCreator ;
		histoCreator->setEventList(temp) ;
		histoCreator->setDataStyle(true) ;
		TProfile* profBefore = histoCreator->getNHitOverTimeProfile("before" , beginTime*200e-9) ;


		std::vector<double> nHitCorrList ;
		for ( int i = 2 ; i < 9 ; ++i )
		{
			std::vector<Event> tempCorr = temp ;
			timeCorr.correct( tempCorr , i ) ;

			double mean = 0 ;
			for ( std::vector<Event>::const_iterator eIt = tempCorr.begin() ; eIt < tempCorr.end() ; ++eIt )
				mean += eIt->nHit ;
			mean /= tempCorr.size() ;

			nHitCorrList.push_back(mean) ;
		}

		double mean = 0 ;
		double sumSq = 0 ;
		unsigned int size = static_cast<unsigned int>( nHitCorrList.size() ) ;
		for ( unsigned int i = 0 ; i < size ; ++i )
		{
			mean += nHitCorrList.at(i) ;
			sumSq += nHitCorrList.at(i)*nHitCorrList.at(i) ;
		}
		mean /= size ;
		std::cout << "mean " << mean << std::endl ;

		double var = ((1.0*size)/(1.0*size-1.0)) * ( sumSq/size - (mean*mean) ) ;
		var = sqrt(var) ;
		std::cout << "ed : " << var << std::endl ;

		nHitErrMap.insert( std::make_pair(it->first , var) ) ;


//		timeCorr.correct(temp , 4) ;
		timeCorr.correct(temp , 2) ;

		std::stringstream toto ;
		toto << "longiProfData" << it->first << "GeV" ;
		TH1D* longiProfHistData = histoCreator->getLongiProfile( toto.str() ) ;
		longiProfHistData->SetDirectory(0) ;
		longiProfHistData->Scale( 1.0/longiProfHistData->Integral() ) ;

		longiProfHistDataMap.insert( std::make_pair(it->first , longiProfHistData) ) ;

		toto.str("") ;
		toto << "radiProfData" << it->first << "GeV" ;
		TH1D* radiProfHistData = histoCreator->getRadiProfile( toto.str() ) ;
		radiProfHistData->SetDirectory(0) ;
		radiProfHistData->Scale( 1.0/radiProfHistData->Integral() ) ;

		radiProfHistDataMap.insert( std::make_pair(it->first , radiProfHistData) ) ;


		histoCreator->setEventList(temp) ;

		radiMeanDataMap.insert( std::make_pair(it->first , histoCreator->getMeanRadius() ) ) ;
		TProfile* profAfter = histoCreator->getNHitOverTimeProfile("after" , beginTime*200e-9) ;

		//		TProfile* profAfter = histoCreator->getMeanRadiusOverTimeProfile("after" , 5e6*200e-9) ;



		if (it->first == 50)
		{
			TCanvas* c2 = new TCanvas("c2","c2",900,900) ;
			c2->cd() ;
			c2->SetTicks() ;
			gStyle->SetOptStat(0) ;

			profAfter->SetLineColor(kRed-4) ;
			gStyle->SetOptStat(0) ;
			profAfter->Draw() ;
			profBefore->Draw("same") ;

			c2->SaveAs("test.root") ;
			c2->Close() ;
			//	app->Run() ;
		}



		toto.str("") ;
		toto << "nHitData" << it->first << "GeV" ;
		TH1D* nHitHistData = histoCreator->getNHit( toto.str() ) ;
		nHitHistData->SetDirectory(0) ;
		nHitHistData->Scale( 1.0/nHitHistData->Integral() ) ;

		nHitHistDataMap.insert( std::make_pair(it->first , nHitHistData) ) ;


		dataFile->Close() ;

		delete histoCreator ;
	}


	std::vector< std::string > physicsListVec ;
	physicsListVec.push_back("FTF_BIC") ;
	physicsListVec.push_back("FTFP_BERT") ;
	physicsListVec.push_back("QGSP_BERT") ;
	//			physicsListVec.push_back("FTFP_BERT_EMY") ;
	//			physicsListVec.push_back("QGSP_BERT_EMY") ;
	//			physicsListVec.push_back("FTFP_BERT_HP") ;
	//			physicsListVec.push_back("QGSP_BERT_HP") ;

	std::vector< std::map<int , TH1D*> > nHitHistSimMap ;
	std::vector< std::map<int , TH1D*> > longiProfHistSimMap ;
	std::vector< std::map<int , TH1D*> > radiProfHistSimMap ;
	std::vector< std::map<int , double> > radiMeanSimMap ;

	for ( std::vector<std::string>::const_iterator pIt = physicsListVec.begin() ; pIt != physicsListVec.end() ; ++pIt )
	{
		std::map<int , TH1D*> nHitMap ;
		std::map<int , TH1D*> longiProfMap ;
		std::map<int , TH1D*> radiProfMap ;
		std::map<int , double> radiMeanMap ;

		for ( std::map<int , int>::const_iterator it = runMap.begin() ; it != runMap.end() ; ++it )
		{
			std::cout << "Process " << it->first << " GeV " << *pIt << std::endl ;
			std::stringstream sim ;
			//		sim << "/home/garillot/files/local/RootFile/Geant4.10.01/QGSP_BERT_HP/single_e-_" << it->first << "GeV.root" ;

			//			sim << "/home/garillot/files/Analysis/dCut0.5/Geant4.9.6/" << *pIt << "/single_pi-_" << it->first << "GeV.root" ;
			//			sim << "/home/garillot/files/Analysis/Geant4.9.6/" << *pIt << "/single_pi-_" << it->first << "GeV.root" ;
			//						sim << "/home/garillot/files/Analysis/OldDigit/Geant4.9.6/" << *pIt << "/single_pi-_" << it->first << "GeV.root" ;
			//			sim << "/home/garillot/files/Analysis/Geant4.9.6/" << *pIt << "/single_e-_" << it->first << "GeV.root" ;
			//			sim << "/home/garillot/files/Analysis/range70/Geant4.9.6/" << *pIt << "/single_e-_" << it->first << "GeV.root" ;
			//						sim << "/home/garillot/files/Analysis/range70/Geant4.9.6/" << *pIt << "/single_pi-_" << it->first << "GeV.root" ;

//			sim << "/home/garillot/files/Analysis/TBLike/Geant4.9.6/" << *pIt << "/single_pi-_" << it->first << "GeV.root" ;
			sim << "/home/garillot/files/Analysis/TBLike/Geant4.9.6/" << *pIt << "/single_pi-_" << it->first << "GeV.root" ;

			TFile* simFile = TFile::Open( sim.str().c_str() , "READ") ;
			if ( !simFile )
			{
				std::cerr << "File " << sim.str() << " not present" << std::endl ;
				continue ;
			}
			TTree* simTree = dynamic_cast<TTree*>( simFile->Get("tree") ) ;
			if ( !simTree )
			{
				std::cerr << "Tree not present in " << sim.str() << std::endl ;
				continue ;
			}

			//			Event2 event ;
			eventReader.setTree(simTree) ;
//			event.setBranchAddress(simTree) ;

			std::vector<Event> temp ;

//			event.setBranchAddress(simTree) ;

			temp.clear() ;

			int iEntry = 0 ;
			while ( simTree->GetEntry(iEntry++) )
			{
				Event event = eventReader.getEvent(iEntry-1) ;
				if ( !cut(event) )
					continue ;

				temp.push_back(event) ;
			}

			HistoCreator* histoCreator = new HistoCreator ;
			histoCreator->setEventList(temp) ;
			histoCreator->setDataStyle(false) ;

			Color_t color ;
			Color_t fillColor ;

			if ( *pIt == "FTF_BIC" )
			{
				color = kMagenta+1 ;
				fillColor = kMagenta-9 ;
			}
			else if ( *pIt == "FTFP_BERT" )
			{
				color = kOrange+7 ;
				fillColor = kOrange-9 ;
			}
			else if ( *pIt == "QGSP_BERT" )
			{
				color = kGreen+2 ;
				fillColor = kGreen-9 ;
			}
			else if ( *pIt == "FTFP_BERT_HP" )
			{
				color = kOrange+7 ;
				fillColor = kOrange-9 ;
			}
			else if ( *pIt == "QGSP_BERT_HP" )
			{
				color = kGreen+2 ;
				fillColor = kGreen-9 ;
			}
			else if ( *pIt == "FTFP_BERT_EMY" )
			{
				color = kOrange+7 ;
				fillColor = kOrange-9 ;
			}
			else if ( *pIt == "QGSP_BERT_EMY" )
			{
				color = kGreen+2 ;
				fillColor = kGreen-9 ;
			}
			else
			{
				color = kBlack ;
				fillColor = kBlack ;
			}

			histoCreator->setColor(color) ;
			histoCreator->setFillColor(fillColor) ;

			std::stringstream toto ;
			toto << "longiProfSim" << it->first << "GeV" ;
			TH1D* longiProfHistSim = histoCreator->getLongiProfile( toto.str() ) ;
			longiProfHistSim->SetDirectory(0) ;
			longiProfHistSim->Scale( 1.0/longiProfHistSim->Integral() ) ;

			longiProfMap.insert( std::make_pair(it->first , longiProfHistSim) ) ;

			toto.str("") ;
			toto << "radiProfSim" << it->first << "GeV" ;
			TH1D* radiProfHistSim = histoCreator->getRadiProfile( toto.str() ) ;
			radiProfHistSim->SetDirectory(0) ;
			radiProfHistSim->Scale( 1.0/radiProfHistSim->Integral() ) ;

			radiProfMap.insert( std::make_pair(it->first , radiProfHistSim) ) ;

			radiMeanMap.insert( std::make_pair(it->first , histoCreator->getMeanRadius()) ) ;



			toto.str("") ;
			toto << "nHitSim" << it->first << "GeV" ;
			TH1D* nHitHistSim = histoCreator->getNHit( toto.str() ) ;
			nHitHistSim->SetDirectory(0) ;
			nHitHistSim->Scale( 1.0/nHitHistSim->Integral() ) ;

			nHitMap.insert( std::make_pair(it->first , nHitHistSim) ) ;

			simFile->Close() ;
			delete histoCreator ;
		}

		nHitHistSimMap.push_back( nHitMap ) ;
		longiProfHistSimMap.push_back( longiProfMap ) ;
		radiProfHistSimMap.push_back( radiProfMap ) ;
		radiMeanSimMap.push_back( radiMeanMap ) ;
	}


	for ( unsigned int i = 0 ; i < physicsListVec.size() ; ++i )
	{
		TDirectory* dir = histosDir->GetDirectory("nHit") ;
		dir->cd() ;
		TDirectory* physDir = dir->mkdir( physicsListVec.at(i).c_str() ) ;
		physDir->cd() ;

		for ( std::map<int , TH1D*>::const_iterator it = nHitHistSimMap.at(i).begin() ; it != nHitHistSimMap.at(i).end() ; ++it )
		{
			std::cout << "Process nHit " << it->first << " GeV " << physicsListVec.at(i) << std::endl ;
			std::stringstream toto ;
			toto << it->first << "GeV" ;
			TCanvas* c1 = new TCanvas( toto.str().c_str() , toto.str().c_str() , 900 , 900 ) ;
			c1->cd() ;
			c1->SetTicks() ;
			gStyle->SetOptStat(0) ;

			THStack* stack = new THStack(toto.str().c_str() , ";Nhit;") ;
			stack->Add(it->second) ;
			stack->Add( nHitHistDataMap[it->first] , "p E X0") ;
			stack->Draw("nostack") ;

			TLegend* leg = new TLegend(0.15,0.75,0.35,0.85) ;
			leg->SetBorderSize(0) ;
			leg->AddEntry(nHitHistDataMap[it->first] , "SPS H2" , "ep" ) ;
			leg->AddEntry(it->second , physicsListVec.at(i).c_str() , "f" ) ;
			leg->Draw() ;

			c1->Write() ;
			c1->Close() ;
		}


		//		physDir->Close() ;
		//		dir->Close() ;

		TDirectory* dir2 = histosDir->GetDirectory("LongiProfile") ;
		dir2->cd() ;
		TDirectory* physDir2 = dir2->mkdir( physicsListVec.at(i).c_str() ) ;
		physDir2->cd() ;

		for ( std::map<int , TH1D*>::const_iterator it = longiProfHistSimMap.at(i).begin() ; it != longiProfHistSimMap.at(i).end() ; ++it )
		{
			std::cout << "Process LongiProfile " << it->first << " GeV " << physicsListVec.at(i) << std::endl ;
			std::stringstream toto ;
			toto << it->first << "GeV" ;
			TCanvas* c1 = new TCanvas( toto.str().c_str() , toto.str().c_str() , 900 , 900 ) ;
			c1->cd() ;
			c1->SetTicks() ;
			gStyle->SetOptStat(0) ;

			THStack* stack = new THStack(toto.str().c_str() , ";Layer;") ;
			stack->Add(it->second) ;
			stack->Add( longiProfHistDataMap[it->first] , "p E X0") ;
			stack->Draw("nostack") ;

			TLegend* leg = new TLegend(0.65,0.75,0.85,0.85) ;
			leg->SetBorderSize(0) ;
			leg->AddEntry(longiProfHistDataMap[it->first] , "SPS H2" , "ep" ) ;
			leg->AddEntry(it->second , physicsListVec.at(i).c_str() , "f" ) ;
			leg->Draw() ;

			c1->Write() ;
			c1->Close() ;
		}


		//		physDir->Close() ;
		//		dir->Close() ;

		TDirectory* dir3 = histosDir->GetDirectory("RadiProfile") ;
		dir3->cd() ;
		TDirectory* physDir3 = dir3->mkdir( physicsListVec.at(i).c_str() ) ;
		physDir3->cd() ;

		for ( std::map<int , TH1D*>::const_iterator it = radiProfHistSimMap.at(i).begin() ; it != radiProfHistSimMap.at(i).end() ; ++it )
		{
			std::cout << "Process RadiProfile " << it->first << " GeV " << physicsListVec.at(i) << std::endl ;
			std::stringstream toto ;
			toto << it->first << "GeV" ;
			TCanvas* c1 = new TCanvas( toto.str().c_str() , toto.str().c_str() , 900 , 900 ) ;
			c1->cd() ;
			c1->SetTicks() ;
			gStyle->SetOptStat(0) ;

			THStack* stack = new THStack(toto.str().c_str() , ";R (cm);") ;
			stack->Add(it->second) ;
			stack->Add( radiProfHistDataMap[it->first] , "p E X0") ;
			stack->Draw("nostack") ;

			TLegend* leg = new TLegend(0.65,0.75,0.85,0.85) ;
			leg->SetBorderSize(0) ;
			leg->AddEntry(radiProfHistDataMap[it->first] , "SPS H2" , "ep" ) ;
			leg->AddEntry(it->second , physicsListVec.at(i).c_str() , "f" ) ;
			leg->Draw() ;

			c1->Write() ;
			c1->Close() ;
		}

		//		physDir->Close() ;
		//		dir->Close() ;
	}



	graphsDir->cd() ;

	std::vector<TGraphErrors*> nHitGraphVec ;
	for ( unsigned int j = 0 ; j < physicsListVec.size()+1 ; ++j )
	{
		TGraphErrors* graph = new TGraphErrors ;
		graph->SetMarkerStyle(20) ;
		graph->SetMarkerSize(1.2f) ;
		nHitGraphVec.push_back( graph ) ;
	}

	std::vector<TGraphErrors*> nHitGraphDevVec ;
	for ( unsigned int j = 0 ; j < physicsListVec.size() ; ++j )
	{
		TGraphErrors* graph = new TGraphErrors ;
		graph->SetMarkerStyle(20) ;
		graph->SetMarkerSize(1.2f) ;
		nHitGraphDevVec.push_back( graph ) ;
	}


	std::vector<TGraphErrors*> longiProfGraphVec ;
	for ( unsigned int j = 0 ; j < physicsListVec.size()+1 ; ++j )
	{
		TGraphErrors* graph = new TGraphErrors ;
		graph->SetMarkerStyle(20) ;
		graph->SetMarkerSize(1.2f) ;
		longiProfGraphVec.push_back( graph ) ;
	}

	std::vector<TGraphErrors*> longiProfGraphDevVec ;
	for ( unsigned int j = 0 ; j < physicsListVec.size() ; ++j )
	{
		TGraphErrors* graph = new TGraphErrors ;
		graph->SetMarkerStyle(20) ;
		graph->SetMarkerSize(1.2f) ;
		longiProfGraphDevVec.push_back( graph ) ;
	}


	std::vector<TGraphErrors*> radiProfGraphVec ;
	for ( unsigned int j = 0 ; j < physicsListVec.size()+1 ; ++j )
	{
		TGraphErrors* graph = new TGraphErrors ;
		graph->SetMarkerStyle(20) ;
		graph->SetMarkerSize(1.2f) ;
		radiProfGraphVec.push_back( graph ) ;
	}

	std::vector<TGraphErrors*> radiProfGraphDevVec ;
	for ( unsigned int j = 0 ; j < physicsListVec.size() ; ++j )
	{
		TGraphErrors* graph = new TGraphErrors ;
		graph->SetMarkerStyle(20) ;
		graph->SetMarkerSize(1.2f) ;
		radiProfGraphDevVec.push_back( graph ) ;
	}


	for ( std::map<int , TH1D*>::const_iterator it = nHitHistDataMap.begin() ; it != nHitHistDataMap.end() ; ++it )
	{
		TGraphErrors* graph = nHitGraphVec.at(0) ;
		graph->SetMarkerStyle(20) ;
		graph->SetPoint( graph->GetN() , it->first , it->second->GetMean() ) ;
		graph->SetPointError( graph->GetN()-1 , 0 , nHitErrMap[ it->first ] ) ;
	}

	for ( std::map<int , TH1D*>::const_iterator it = longiProfHistDataMap.begin() ; it != longiProfHistDataMap.end() ; ++it )
	{
		TGraphErrors* graph = longiProfGraphVec.at(0) ;
		graph->SetMarkerStyle(20) ;
		graph->SetPoint( graph->GetN() , it->first , it->second->GetMean() ) ;
	}

	for ( std::map<int , TH1D*>::const_iterator it = radiProfHistDataMap.begin() ; it != radiProfHistDataMap.end() ; ++it )
	{
		TGraphErrors* graph = radiProfGraphVec.at(0) ;
		graph->SetMarkerStyle(20) ;
		graph->SetPoint( graph->GetN() , it->first , it->second->GetMean() ) ;
	}


	for ( unsigned int i = 0 ; i < physicsListVec.size() ; ++i )
	{
		Color_t color ;
		if ( physicsListVec.at(i) == "FTF_BIC" )
			color = kMagenta+1 ;
		else if ( physicsListVec.at(i) == "FTFP_BERT" )
			color = kOrange+7 ;
		else if ( physicsListVec.at(i) == "QGSP_BERT" )
			color = kGreen+2 ;
		else if ( physicsListVec.at(i) == "FTFP_BERT_HP" )
			color = kOrange+7 ;
		else if ( physicsListVec.at(i) == "QGSP_BERT_HP" )
			color = kGreen+2 ;
		else if ( physicsListVec.at(i) == "FTFP_BERT_EMY" )
			color = kOrange+7 ;
		else if ( physicsListVec.at(i) == "QGSP_BERT_EMY" )
			color = kGreen+2 ;
		else
			color = kBlack ;

		for ( std::map<int , TH1D*>::const_iterator it = nHitHistSimMap.at(i).begin() ; it != nHitHistSimMap.at(i).end() ; ++it )
		{
			TGraphErrors* graph = nHitGraphVec.at(i+1) ;
			graph->SetMarkerColor(color) ;
			graph->SetPoint( graph->GetN() , it->first , it->second->GetMean() ) ;

			double b = it->second->GetMean() ;
			double a = nHitHistDataMap[it->first]->GetMean() ;
			double aErr = nHitErrMap[it->first] ;

			double dev = (b-a)/a ;
			double devError = std::sqrt( b*b*aErr*aErr/(a*a) ) / a ;

			nHitGraphDevVec.at(i)->SetPoint( nHitGraphDevVec.at(i)->GetN() , it->first , dev ) ;
			nHitGraphDevVec.at(i)->SetPointError( nHitGraphDevVec.at(i)->GetN()-1 , 0 , devError ) ;
			nHitGraphDevVec.at(i)->SetMarkerColor(color) ;
			nHitGraphDevVec.at(i)->SetLineColor(color) ;
		}

		for ( std::map<int , TH1D*>::const_iterator it = longiProfHistSimMap.at(i).begin() ; it != longiProfHistSimMap.at(i).end() ; ++it )
		{
			TGraphErrors* graph = longiProfGraphVec.at(i+1) ;
			graph->SetMarkerColor(color) ;
			graph->SetPoint( graph->GetN() , it->first , it->second->GetMean() ) ;

			longiProfGraphDevVec.at(i)->SetPoint( longiProfGraphDevVec.at(i)->GetN() , it->first , (it->second->GetMean() - longiProfHistDataMap[it->first]->GetMean())/longiProfHistDataMap[it->first]->GetMean() ) ;
			longiProfGraphDevVec.at(i)->SetMarkerColor(color) ;
		}

		for ( std::map<int , TH1D*>::const_iterator it = radiProfHistSimMap.at(i).begin() ; it != radiProfHistSimMap.at(i).end() ; ++it )
		{
			TGraphErrors* graph = radiProfGraphVec.at(i+1) ;
			graph->SetMarkerColor(color) ;
			graph->SetPoint( graph->GetN() , it->first , it->second->GetMean() ) ;

			radiProfGraphDevVec.at(i)->SetPoint( radiProfGraphDevVec.at(i)->GetN() , it->first , (it->second->GetMean() - radiProfHistDataMap[it->first]->GetMean())/radiProfHistDataMap[it->first]->GetMean() ) ;
			radiProfGraphDevVec.at(i)->SetMarkerColor(color) ;
		}

		//		for ( std::map<int , double>::const_iterator it = radiMeanSimMap.at(i).begin() ; it != radiMeanSimMap.at(i).end() ; ++it )
		//		{
		//			TGraphErrors* graph = radiProfGraphVec.at(i+1) ;
		//			graph->SetMarkerColor(color) ;
		//			graph->SetPoint( graph->GetN() , it->first , it->second ) ;

		////			radiProfGraphDevVec.at(i)->SetPoint( radiProfGraphDevVec.at(i)->GetN() , it->first , (it->second->GetMean() - radiProfHistDataMap[it->first]->GetMean())/radiProfHistDataMap[it->first]->GetMean() ) ;
		////			radiProfGraphDevVec.at(i)->SetMarkerColor(color) ;
		//		}

	}

	int energyMax = (--runMap.end())->first + 5 ;

	TCanvas* nHitC = new TCanvas("nHitCanv" , "nHitCanv"  , 700 , 900) ;
	nHitC->cd() ;
	TPad* nHitC1 = new TPad("nHitC1","nHitC1" , 0 , 0.3 , 1.0 , 1.0) ;
	TPad* nHitC2 = new TPad("nHitC2","nHitC2" , 0 , 0 , 1.0 , 0.3) ;
	nHitC1->SetTicks() ;
	nHitC2->SetTicks() ;
	nHitC1->Draw() ;
	nHitC2->Draw() ;
	nHitC1->SetTopMargin(0.05f) ;
	nHitC1->SetBottomMargin(0.02f) ;
	nHitC2->SetTopMargin(0) ;
	nHitC2->SetBottomMargin(0.15f) ;
	nHitC1->SetLeftMargin(0.15f) ;
	nHitC2->SetLeftMargin(0.15f) ;
	nHitC1->SetRightMargin(0.02f) ;
	nHitC2->SetRightMargin(0.02f) ;

	nHitC1->cd() ;
//	TH2D* nHitRange = new TH2D("nHit" , ";Energy (GeV);<nHit>" , 1 , 0 , energyMax , 1 , 0 , 600) ;
	TH2D* nHitRange = new TH2D("nHit" , ";Energy (GeV);<nHit>" , 1 , 0 , energyMax , 1 , 0 , 1400) ;
	nHitRange->GetXaxis()->SetLabelSize(0) ;

	nHitRange->GetYaxis()->SetTitleOffset(1.8f) ;
	nHitRange->Draw() ;

	for ( unsigned int i = 0 ; i < nHitGraphVec.size() ; ++i )
		nHitGraphVec.at(i)->Draw("P same") ;


	TLegend* nHitleg = new TLegend(0.18,0.75,0.6,0.88) ;
	nHitleg->SetBorderSize(0) ;

	for ( unsigned int i = 0 ; i < nHitGraphVec.size() ; ++i )
	{
		if ( i == 0 )
			nHitleg->AddEntry(nHitGraphVec.at(i) , "SPS H2","p") ;
		else
			nHitleg->AddEntry(nHitGraphVec.at(i) , physicsListVec.at(i-1).c_str() ,"p") ;
	}
	nHitleg->Draw() ;

	nHitC2->cd() ;
	TH2D* nHitDevRange = new TH2D("nHitDev" , ";Energy (GeV);(MC-DATA)/DATA   " , 1 , 0 , energyMax , 1 , -0.19 , 0.19) ;
	nHitDevRange->GetXaxis()->SetTickLength(0.075f) ;
	nHitDevRange->GetXaxis()->SetLabelSize(0.075f) ;
	nHitDevRange->GetXaxis()->SetTitleSize(0.075f) ;
	nHitDevRange->GetXaxis()->SetTitleOffset(0.9f) ;
	nHitDevRange->GetYaxis()->SetLabelSize(0.075f) ;
	nHitDevRange->GetYaxis()->SetNdivisions(5,5,0) ;
	nHitDevRange->GetYaxis()->SetTitleSize(0.075f) ;
	nHitDevRange->GetYaxis()->SetTitleOffset(0.8f) ;
	nHitDevRange->Draw() ;

	TLine* nHitDevLine = new TLine(0,0,energyMax,0) ;
	nHitDevLine->SetLineColor(kBlack) ;
	nHitDevLine->SetLineWidth(2) ;
	nHitDevLine->Draw() ;

	for ( unsigned int i = 0 ; i < nHitGraphDevVec.size() ; ++i )
		nHitGraphDevVec.at(i)->Draw("PZ same") ;


	nHitC->Write() ;
	nHitC->Close() ;



	TCanvas* longiProfC = new TCanvas("longiProfCanv" , "longiProfCanv"  , 700 , 900) ;
	longiProfC->cd() ;
	TPad* longiProfC1 = new TPad("longiProfC1","longiProfC1" , 0 , 0.3 , 1.0 , 1.0) ;
	TPad* longiProfC2 = new TPad("longiProfC2","longiProfC2" , 0 , 0 , 1.0 , 0.3) ;
	longiProfC1->SetTicks() ;
	longiProfC2->SetTicks() ;
	longiProfC1->Draw() ;
	longiProfC2->Draw() ;
	longiProfC1->SetTopMargin(0.05f) ;
	longiProfC1->SetBottomMargin(0.02f) ;
	longiProfC2->SetTopMargin(0) ;
	longiProfC2->SetBottomMargin(0.15f) ;
	longiProfC1->SetLeftMargin(0.15f) ;
	longiProfC2->SetLeftMargin(0.15f) ;
	longiProfC1->SetRightMargin(0.02f) ;
	longiProfC2->SetRightMargin(0.02f) ;

	longiProfC1->cd() ;
	TH2D* longiProfRange = new TH2D("Longitudinal Profile" , ";Energy (GeV);<Layer>" , 1 , 0 , energyMax , 1 , 0 , 16) ;
	longiProfRange->GetXaxis()->SetLabelSize(0) ;

	longiProfRange->GetYaxis()->SetTitleOffset(1.8f) ;
	longiProfRange->Draw() ;

	for ( unsigned int i = 0 ; i < longiProfGraphVec.size() ; ++i )
		longiProfGraphVec.at(i)->Draw("P same") ;


	TLegend* longiProfleg = new TLegend(0.18,0.75,0.6,0.88) ;
	longiProfleg->SetBorderSize(0) ;

	for ( unsigned int i = 0 ; i < longiProfGraphVec.size() ; ++i )
	{
		if ( i == 0 )
			longiProfleg->AddEntry(longiProfGraphVec.at(i) , "SPS H2","p") ;
		else
			longiProfleg->AddEntry(longiProfGraphVec.at(i) , physicsListVec.at(i-1).c_str() ,"p") ;
	}
	longiProfleg->Draw() ;

	longiProfC2->cd() ;
	TH2D* longiProfDevRange = new TH2D("longiProfDevRange" , ";Energy (GeV);(MC-DATA)/DATA   " , 1 , 0 , energyMax , 1 , -0.19 , 0.19) ;
	longiProfDevRange->GetXaxis()->SetTickLength(0.075f) ;
	longiProfDevRange->GetXaxis()->SetLabelSize(0.075f) ;
	longiProfDevRange->GetXaxis()->SetTitleSize(0.075f) ;
	longiProfDevRange->GetXaxis()->SetTitleOffset(0.9f) ;
	longiProfDevRange->GetYaxis()->SetLabelSize(0.075f) ;
	longiProfDevRange->GetYaxis()->SetNdivisions(5,5,0) ;
	longiProfDevRange->GetYaxis()->SetTitleSize(0.075f) ;
	longiProfDevRange->GetYaxis()->SetTitleOffset(0.8f) ;
	longiProfDevRange->Draw() ;

	TLine* longiProfDevLine = new TLine(0,0,energyMax,0) ;
	longiProfDevLine->SetLineColor(kBlack) ;
	longiProfDevLine->SetLineWidth(2) ;
	longiProfDevLine->Draw() ;

	for ( unsigned int i = 0 ; i < longiProfGraphDevVec.size() ; ++i )
		longiProfGraphDevVec.at(i)->Draw("P same") ;


	longiProfC->Write() ;
	longiProfC->Close() ;



	TCanvas* radiProfC = new TCanvas("radiProfCanv" , "radiProfCanv"  , 700 , 900) ;
	radiProfC->cd() ;
	TPad* radiProfC1 = new TPad("radiProfC1","radiProfC1" , 0 , 0.3 , 1.0 , 1.0) ;
	TPad* radiProfC2 = new TPad("radiProfC2","radiProfC2" , 0 , 0 , 1.0 , 0.3) ;
	radiProfC1->SetTicks() ;
	radiProfC2->SetTicks() ;
	radiProfC1->Draw() ;
	radiProfC2->Draw() ;
	radiProfC1->SetTopMargin(0.05f) ;
	radiProfC1->SetBottomMargin(0.02f) ;
	radiProfC2->SetTopMargin(0) ;
	radiProfC2->SetBottomMargin(0.15f) ;
	radiProfC1->SetLeftMargin(0.15f) ;
	radiProfC2->SetLeftMargin(0.15f) ;
	radiProfC1->SetRightMargin(0.02f) ;
	radiProfC2->SetRightMargin(0.02f) ;

	radiProfC1->cd() ;
	TH2D* radiProfRange = new TH2D("Radial Profile" , ";Energy (GeV);<R> (cm)" , 1 , 0 , energyMax , 1 , 0 , 8) ;
	radiProfRange->GetXaxis()->SetLabelSize(0) ;

	radiProfRange->GetYaxis()->SetTitleOffset(1.8f) ;
	radiProfRange->Draw() ;

	for ( unsigned int i = 0 ; i < radiProfGraphVec.size() ; ++i )
		radiProfGraphVec.at(i)->Draw("P same") ;


	TLegend* radiProfleg = new TLegend(0.43,0.3,0.85,0.43) ;
	radiProfleg->SetBorderSize(0) ;

	for ( unsigned int i = 0 ; i < radiProfGraphVec.size() ; ++i )
	{
		if ( i == 0 )
			radiProfleg->AddEntry(radiProfGraphVec.at(i) , "SPS H2","p") ;
		else
			radiProfleg->AddEntry(radiProfGraphVec.at(i) , physicsListVec.at(i-1).c_str() ,"p") ;
	}
	radiProfleg->Draw() ;

	radiProfC2->cd() ;
	TH2D* radiProfDevRange = new TH2D("radiProfDevRange" , ";Energy (GeV);(MC-DATA)/DATA   " , 1 , 0 , energyMax , 1 , -0.19 , 0.19) ;
	radiProfDevRange->GetXaxis()->SetTickLength(0.075f) ;
	radiProfDevRange->GetXaxis()->SetLabelSize(0.075f) ;
	radiProfDevRange->GetXaxis()->SetTitleSize(0.075f) ;
	radiProfDevRange->GetXaxis()->SetTitleOffset(0.9f) ;
	radiProfDevRange->GetYaxis()->SetLabelSize(0.075f) ;
	radiProfDevRange->GetYaxis()->SetNdivisions(5,5,0) ;
	radiProfDevRange->GetYaxis()->SetTitleSize(0.075f) ;
	radiProfDevRange->GetYaxis()->SetTitleOffset(0.8f) ;
	radiProfDevRange->Draw() ;

	TLine* radiProfDevLine = new TLine(0,0,energyMax,0) ;
	radiProfDevLine->SetLineColor(kBlack) ;
	radiProfDevLine->SetLineWidth(2) ;
	radiProfDevLine->Draw() ;

	for ( unsigned int i = 0 ; i < radiProfGraphDevVec.size() ; ++i )
		radiProfGraphDevVec.at(i)->Draw("P same") ;


	radiProfC->Write() ;
	radiProfC->Close() ;



	return 0 ;


}

