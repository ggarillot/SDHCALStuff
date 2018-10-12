#include <iostream>
#include <map>
#include <sstream>

#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <THStack.h>
#include <TLegend.h>
#include <TLine.h>
#include <TDirectory.h>
#include <TLatex.h>

#include "TimeCorrection.h"
#include "Event.h"
#include "DataMcCanvas.h"

void addG4Version(TPad* canvas , std::string version)
{
	canvas->cd() ;
	std::stringstream toto ;
	toto << "Geant4." << version ;
	TLatex* t = new TLatex(.10 , .96 , toto.str().c_str()) ;
	t->SetNDC() ;
	t->SetTextAlign(11) ;
	t->SetTextSize(0.03f) ;
	t->Draw() ;
}

void addEnergyText(TPad* canvas , double energy)
{
	canvas->cd() ;
	std::stringstream toto ;
	toto << energy << " GeV" ;
	TLatex* t = new TLatex(.525 , .96 , toto.str().c_str()) ;
	t->SetNDC() ;
	t->SetTextAlign(21) ;
	t->SetTextSize(0.03f) ;
	t->Draw() ;
}

void writeHistos(std::map<int , TH1D*> simMap , std::map<int , TH1D*> dataMap , std::string physList , std::string g4Version , std::string histoTitle)
{
	for ( std::map<int , TH1D*>::const_iterator it = simMap.begin() ; it != simMap.end() ; ++it )
	{
		//		std::cout << "Process nHit " << it->first << " GeV " << physList << std::endl ;
		std::stringstream toto ;
		toto << it->first << "GeV" ;

		auto histoSim = it->second ;
		auto histoData = dataMap[it->first] ;

		DataMcCanvas* c = new DataMcCanvas( toto.str().c_str() ) ;

		c->topPad()->cd() ;

		double maxSim = histoSim->GetBinContent( histoSim->GetMaximumBin() ) ;
		double maxData = histoData->GetBinContent( histoData->GetMaximumBin() ) ;

		if ( maxSim > maxData )
		{
			histoSim->SetTitle( histoTitle.c_str() ) ;
			histoSim->Draw() ;
			histoData->Draw("p0 same") ;
		}
		else
		{
			histoData->SetTitle( histoTitle.c_str() ) ;
			histoData->Draw("p0") ;
			histoSim->Draw("same") ;
			histoData->Draw("p0 same") ;
		}

		TLegend* leg = new TLegend(0.15,0.75,0.35,0.85) ;
		leg->SetBorderSize(0) ;
		leg->AddEntry(histoData , "SPS H2" , "ep" ) ;
		leg->AddEntry(histoSim , physList.c_str() , "f" ) ;
		leg->Draw() ;

		c->topPad()->RedrawAxis() ;

		addEnergyText(c->topPad() , it->first) ;
		addG4Version(c->topPad() , g4Version) ;

		c->Write() ;
		c->Close() ;
	}
}

struct ColorPhysList
{
		ColorPhysList(Color_t c , Color_t f)
			: color(c) , fillColor(f) {}
		Color_t color = kBlack ;
		Color_t fillColor = kBlack ;
} ;

int main()
{
	std::map<std::string , ColorPhysList> colorMap = {} ;
	colorMap.insert( {"FTF_BIC" , {kMagenta+1 , kMagenta-9}} ) ;
	colorMap.insert( {"FTFP_BERT" , {kOrange+7 , kOrange-9}} ) ;
	colorMap.insert( {"QGSP_BERT" , {kGreen+2 , kGreen-9}} ) ;
	colorMap.insert( {"FTFP_BERT_HP" , {kOrange+7 , kOrange-9}} ) ;
	colorMap.insert( {"QGSP_BERT_HP" , {kGreen+2 , kGreen-9}} ) ;
	colorMap.insert( {"FTFP_BERT_EMY" , {kOrange+7 , kOrange-9}} ) ;
	colorMap.insert( {"QGSP_BERT_EMY" , {kGreen+2 , kGreen-9}} ) ;

//	std::string g4Version = "9.6" ;
	std::string g4Version = "10.4" ;
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


	std::stringstream resFileName ;
	resFileName << "result" << g4Version << ".root" ;
	TFile* resultFile = new TFile(resFileName.str().c_str() , "RECREATE") ;
	resultFile->cd() ;
	TDirectory* histosDir = resultFile->mkdir("Histos") ;
	TDirectory* graphsDir = resultFile->mkdir("Graphs") ;

	histosDir->mkdir("nHit") ;
	histosDir->mkdir("LongiProfile") ;
	histosDir->mkdir("RadiProfile") ;


	std::map<int , TH1D*> nHitHistDataMap ;
	std::map<int , TH1D*> longiProfHistDataMap ;
	std::map<int , TH1D*> radiProfHistDataMap ;
	std::map<int , double> radiMeanDataMap ;

	std::map<int , double> nHitErrMap ;

	for ( std::map<int , int>::const_iterator it = runMap.begin() ; it != runMap.end() ; ++it )
	{
		EventList eventList ;
		eventList.setType(EventList::kHadron) ;
		eventList.setGeomCut( {{490,460,100}} ) ;

		std::stringstream data ;
		//		data << "/home/garillot/files/Analysis/DATA/" << it->second << ".root" ;
		data << "/home/garillot/files/DATA/Analysis/SPS_Oct2015/163/" << it->second << ".root" ;

		unsigned long long beginTime = 5e6 ;
		unsigned long long endTime = 30e6 ;

		eventList.setBeginTimeCut(beginTime) ;
		eventList.setEndTimeCut(endTime) ;
		eventList.loadFile( data.str() ) ;

		HistoCreator* histoCreator = new HistoCreator ;
		histoCreator->setEventList( eventList.events() ) ;
		histoCreator->setDataStyle(true) ;

		TProfile* profBefore = nullptr ;
		TProfile* profAfter = nullptr ;
		profBefore = histoCreator->getNHitOverTimeProfile("before" , beginTime*200e-9) ;

		TimeCorrection timeCorr ;
		timeCorr.setBeginTime(beginTime) ;
		timeCorr.setEndTime(endTime) ;

		std::vector<double> nHitCorrList ;
		for ( int i = 2 ; i < 9 ; ++i )
		{
			auto eventList2 = eventList ;

			timeCorr.correctHits( eventList2 , i ) ;

			double mean = 0 ;
			for ( const auto& event : eventList2.events() )
				mean += event->nHit ;

			mean /= eventList2.events().size() ;

			std::cout << i << " , mean : " << mean << std::endl ;

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

		timeCorr.correctHits(eventList , 2) ;
		//		timeCorr.correctProfiles(eventList , 1) ;

		std::stringstream toto ;
		toto << "longiProfData" << it->first << "GeV" ;
		TH1D* longiProfHistData = histoCreator->getLongiProfile( toto.str() ) ;
		longiProfHistData->SetDirectory(0) ;

		longiProfHistDataMap.insert( std::make_pair(it->first , longiProfHistData) ) ;

		toto.str("") ;
		toto << "radiProfData" << it->first << "GeV" ;
		TH1D* radiProfHistData = histoCreator->getRadiProfile( toto.str() ) ;
		radiProfHistData->SetDirectory(0) ;

		radiProfHistDataMap.insert( std::make_pair(it->first , radiProfHistData) ) ;

		histoCreator->setEventList( eventList.events() ) ;

		radiMeanDataMap.insert( std::make_pair(it->first , histoCreator->getMeanRadius() ) ) ;

		profAfter = histoCreator->getNHitOverTimeProfile("after" , beginTime*200e-9) ;

		//if (it->first == 50)
		{
			TCanvas* c2 = new TCanvas("c2","c2",900,900) ;
			c2->cd() ;
			c2->SetTicks() ;
			gStyle->SetOptStat(0) ;

			profAfter->SetLineColor(kRed-4) ;
			gStyle->SetOptStat(0) ;
			profAfter->Draw() ;
			profBefore->Draw("same") ;

			std::stringstream o ; o << "test" << it->first << ".root" ;
			c2->SaveAs(o.str().c_str()) ;
			c2->Close() ;
			//	app->Run() ;
		}



		toto.str("") ;
		toto << "nHitData" << it->first << "GeV" ;
		TH1D* nHitHistData = histoCreator->getNHit( toto.str() ) ;
		nHitHistData->SetDirectory(0) ;

		nHitHistDataMap.insert( std::make_pair(it->first , nHitHistData) ) ;

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
			EventList eventList ;
			eventList.setType(EventList::kHadron) ;


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
			sim << "/home/garillot/files/TBLike/SPS_Oct2015/Analysis/Geant4." << g4Version << "/" << *pIt << "/pi-_" << it->first << "GeV_Digital.root" ;

			eventList.loadFile( sim.str() ) ;


			HistoCreator* histoCreator = new HistoCreator ;
			histoCreator->setEventList( eventList.events() ) ;
			histoCreator->setDataStyle(false) ;

			Color_t color ;
			Color_t fillColor ;
			auto col = colorMap.find(*pIt) ;
			if ( col == colorMap.end() )
			{
				color = kBlack ;
				fillColor = kBlack ;
			}

			color = col->second.color ;
			fillColor = col->second.fillColor ;

			histoCreator->setColor(color) ;
			histoCreator->setFillColor(fillColor) ;

			std::stringstream toto ;
			toto << "longiProfSim" << it->first << "GeV" ;
			TH1D* longiProfHistSim = histoCreator->getLongiProfile( toto.str() ) ;
			longiProfHistSim->SetDirectory(0) ;

			longiProfMap.insert( std::make_pair(it->first , longiProfHistSim) ) ;

			toto.str("") ;
			toto << "radiProfSim" << it->first << "GeV" ;
			TH1D* radiProfHistSim = histoCreator->getRadiProfile( toto.str() ) ;
			radiProfHistSim->SetDirectory(0) ;

			radiProfMap.insert( std::make_pair(it->first , radiProfHistSim) ) ;

			radiMeanMap.insert( std::make_pair(it->first , histoCreator->getMeanRadius()) ) ;


			toto.str("") ;
			toto << "nHitSim" << it->first << "GeV" ;
			TH1D* nHitHistSim = histoCreator->getNHit( toto.str() ) ;
			nHitHistSim->SetDirectory(0) ;

			nHitMap.insert( std::make_pair(it->first , nHitHistSim) ) ;

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

		writeHistos(nHitHistSimMap.at(i) , nHitHistDataMap , physicsListVec.at(i) , g4Version , ";nHit;") ;


		TDirectory* dir2 = histosDir->GetDirectory("LongiProfile") ;
		dir2->cd() ;
		TDirectory* physDir2 = dir2->mkdir( physicsListVec.at(i).c_str() ) ;
		physDir2->cd() ;

		writeHistos(longiProfHistSimMap.at(i) , longiProfHistDataMap , physicsListVec.at(i) , g4Version , ";Layer;") ;


		TDirectory* dir3 = histosDir->GetDirectory("RadiProfile") ;
		dir3->cd() ;
		TDirectory* physDir3 = dir3->mkdir( physicsListVec.at(i).c_str() ) ;
		physDir3->cd() ;

		writeHistos(radiProfHistSimMap.at(i) , radiProfHistDataMap , physicsListVec.at(i) , g4Version , ";Distance from shower axis (cm);") ;
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

	for ( const auto& it : radiMeanDataMap )
	{
		TGraphErrors* graph = radiProfGraphVec.at(0) ;
		graph->SetMarkerStyle(20) ;
		graph->SetPoint( graph->GetN() , it.first , it.second ) ;
	}


	for ( unsigned int i = 0 ; i < physicsListVec.size() ; ++i )
	{
		Color_t color ;
		auto col = colorMap.find( physicsListVec.at(i) ) ;
		if ( col == colorMap.end() )
			color = kBlack ;

		color = col->second.color ;

		for ( std::map<int , TH1D*>::const_iterator it = nHitHistSimMap.at(i).begin() ; it != nHitHistSimMap.at(i).end() ; ++it )
		{
			TGraphErrors* graph = nHitGraphVec.at(i+1) ;
			graph->SetMarkerColor(color) ;
			graph->SetPoint( graph->GetN() , it->first , it->second->GetMean() ) ;

			double b = it->second->GetMean() ;
			double a = nHitHistDataMap[it->first]->GetMean() ;
			double aErr = nHitErrMap[it->first] ;

			double dev = b/a ;
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

			longiProfGraphDevVec.at(i)->SetPoint( longiProfGraphDevVec.at(i)->GetN() , it->first , (it->second->GetMean())/longiProfHistDataMap[it->first]->GetMean() ) ;
			longiProfGraphDevVec.at(i)->SetMarkerColor(color) ;
		}

		for ( const auto& it : radiMeanSimMap.at(i) )
		{
			TGraphErrors* graph = radiProfGraphVec.at(i+1) ;
			graph->SetMarkerColor(color) ;

			int point = graph->GetN() ;
			graph->SetPoint( point , it.first , it.second ) ;

			radiProfGraphDevVec.at(i)->SetPoint( point , it.first , it.second/radiMeanDataMap[it.first] ) ;
			radiProfGraphDevVec.at(i)->SetMarkerColor(color) ;
		}

	}

	int energyMax = runMap.rbegin()->first + 5 ;


	DataMcCanvas* nHitC = new DataMcCanvas("nHitCanv" , {0,energyMax} , {0,1400} , {0.81,1.19} , "Energy (GeV)" , "<nHit>") ;

	nHitC->topPad()->cd() ;
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

	nHitC->bottomPad()->cd() ;



	for ( unsigned int i = 0 ; i < nHitGraphDevVec.size() ; ++i )
		nHitGraphDevVec.at(i)->Draw("PZ same") ;

	nHitC->Write() ;
	nHitC->Close() ;


	DataMcCanvas* longiProfC = new DataMcCanvas("longiProfCanv" , {0,energyMax} , {0,16} , {0.81,1.19} , "Energy (GeV)" , "<Layer>") ;

	longiProfC->topPad()->cd() ;

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

	longiProfC->bottomPad()->cd() ;

	for ( unsigned int i = 0 ; i < longiProfGraphDevVec.size() ; ++i )
		longiProfGraphDevVec.at(i)->Draw("P same") ;

	longiProfC->Write() ;
	longiProfC->Close() ;


	DataMcCanvas* radiProfC = new DataMcCanvas("radiProfCanv" , {0,energyMax} , {0,8} , {0.81,1.19} , "Energy (GeV)" , "<R> (cm)") ;

	radiProfC->topPad()->cd() ;

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

	radiProfC->bottomPad()->cd() ;

	for ( unsigned int i = 0 ; i < radiProfGraphDevVec.size() ; ++i )
		radiProfGraphDevVec.at(i)->Draw("P same") ;


	radiProfC->Write() ;
	radiProfC->Close() ;


	return 0 ;
}

