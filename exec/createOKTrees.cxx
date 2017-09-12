#include <iostream>

#include "Event.h"
#include "TimeCorrection.h"

#include <TFile.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TLegend.h>
#include <TLine.h>

#include <sstream>
#include <string>


bool cut(Event event)
{
	bool geomCut = std::sqrt( (event.cog[0]-600)*(event.cog[0]-600) + (event.cog[2]-530)*(event.cog[2]-530) ) < 95 ;
	//		bool geomCut = std::sqrt( (event.cog[0]-480)*(event.cog[0]-480) + (event.cog[2]-460)*(event.cog[2]-460) ) < 80 ;
	bool timeCut = event.spillEventTime < 30e6 ;
	bool cut = ( event.transverseRatio > 0.05f && event.neutral == 0 && event.nTrack > 0 && double(event.nHit)/event.nLayer > 2.2 && double(event.nInteractingLayer)/event.nLayer > 0.2 ) ;

	return ( cut && geomCut && timeCut ) ;
	//	return true ;
}

int main()
{
	std::string dir = "/home/garillot/files/Analysis/DATA" ;

	std::map<std::string,double> list ;

	//pions SPS Oct2016
	// 1 pC 2thr
//	list.insert( std::make_pair( "733660" , 20 ) ) ;
//	list.insert( std::make_pair( "733665" , 30 ) ) ;
//	list.insert( std::make_pair( "733683" , 40 ) ) ;
//	list.insert( std::make_pair( "733686" , 50 ) ) ;
//	list.insert( std::make_pair( "733689" , 60 ) ) ;
//	list.insert( std::make_pair( "733693" , 70 ) ) ;
//	list.insert( std::make_pair( "733696" , 80 ) ) ;

	// 5 pC 2thr
		list.insert( std::make_pair( "733756" , 20 ) ) ;
		list.insert( std::make_pair( "733750" , 30 ) ) ;
		list.insert( std::make_pair( "733724" , 40 ) ) ;
		list.insert( std::make_pair( "733728" , 50 ) ) ;
		list.insert( std::make_pair( "733742" , 60 ) ) ;
		list.insert( std::make_pair( "733743" , 70 ) ) ;
		list.insert( std::make_pair( "733754" , 80 ) ) ;


	std::string aStr("1pC") ;

	//pions SPS Oct2015
	// 163
	//	list.insert( std::make_pair( "730716" , 10 ) ) ;
	//	list.insert( std::make_pair( "730656" , 20 ) ) ;
	//	list.insert( std::make_pair( "730634" , 30 ) ) ;
	//	list.insert( std::make_pair( "730657" , 40 ) ) ;
	//	list.insert( std::make_pair( "730651" , 50 ) ) ;
	//	list.insert( std::make_pair( "730655" , 60 ) ) ;
	//	list.insert( std::make_pair( "730659" , 70 ) ) ;
	//	list.insert( std::make_pair( "730677" , 80 ) ) ;

	//	// 214
	//	list.insert( std::make_pair( "730903" , 10 ) ) ;
	//	list.insert( std::make_pair( "730888" , 20 ) ) ;
	//	list.insert( std::make_pair( "730886" , 30 ) ) ;
	//	list.insert( std::make_pair( "730882" , 40 ) ) ;
	//	list.insert( std::make_pair( "730861" , 50 ) ) ;
	//	list.insert( std::make_pair( "730858" , 60 ) ) ;
	//	list.insert( std::make_pair( "730851" , 70 ) ) ;
	//	list.insert( std::make_pair( "730847" , 80 ) ) ;



	TimeCorrection timeCorr ;
	timeCorr.setBeginTime(0) ;


	EventReader eventReader ;

	int nHit ;
	int nHit1 ;
	int nHit2 ;
	int nHit3 ;
	int nHough ;
	int nHough1 ;
	int nHough2 ;
	int nHough3 ;

	for ( std::map<std::string,double>::const_iterator it = list.begin() ; it != list.end() ; ++it )
	{
		std::stringstream data ;
		data << "/home/garillot/files/Analysis/DATA/" << it->first << ".root" ;
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

		std::vector<Event> temp ;

		for ( int iEntry = 0 ; iEntry < dataTree->GetEntries() ; ++iEntry )
		{
			Event event = eventReader.getEvent(iEntry) ;

			if ( !cut(event) )
				continue ;

			temp.push_back(event) ;
		}


		timeCorr.correct(temp) ;


		dataFile->Close() ;

		std::stringstream toto ;
		toto << it->second << "GeV.root" ;
		TFile* file = new TFile(toto.str().c_str() , "RECREATE") ;
		TTree* tree = new TTree("tree","tree") ;


		tree->Branch("nHit" , &nHit) ;
		tree->Branch("nHit1" , &nHit1) ;
		tree->Branch("nHit2" , &nHit2) ;
		tree->Branch("nHit3" , &nHit3) ;
		tree->Branch("nHough" , &nHough) ;
		tree->Branch("nHough1" , &nHough1) ;
		tree->Branch("nHough2" , &nHough2) ;
		tree->Branch("nHough3" , &nHough3) ;

		for ( std::vector<Event>::iterator jt = temp.begin() ; jt != temp.end() ; ++jt )
		{
			Event event = *jt ;
			nHit1 = event.nHit1 ;
			nHit2 = event.nHit2 ;
			nHit3 = event.nHit3 ;
			nHit = nHit1 + nHit2 + nHit3 ;

			nHough1 = event.nHough1 ;
			nHough2 = event.nHough2 ;
			nHough3 = event.nHough3 ;
			nHough = nHough1 + nHough2 + nHough3 ;

			tree->Fill() ;
		}

		tree->Write("tree") ;

		file->Close() ;
	}

	return 0 ;
}
