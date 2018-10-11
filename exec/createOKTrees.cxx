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


	for ( std::map<std::string,double>::const_iterator it = list.begin() ; it != list.end() ; ++it )
	{
		EventList eventList ;
		eventList.setType(EventList::kHadron) ;
		eventList.setGeomCut( {{600,530,95}} ) ;

		std::stringstream data ;
		data << "/home/garillot/files/Analysis/DATA/" << it->first << ".root" ;

		eventList.loadFile( data.str() ) ;

		timeCorr.correctHits( eventList ) ;

		std::stringstream toto ;
		toto << it->second << "GeV.root" ;
		TFile* file = new TFile(toto.str().c_str() , "RECREATE") ;
		TTree* tree = new TTree("tree","tree") ;

		EventReader eventReader ;
		eventReader.setTreeWrite(tree) ;

		for ( const auto& event : eventList.events() )
		{
			eventReader.loadEvent(*event) ;
			tree->Fill() ;
		}

		tree->Write("tree") ;

		file->Close() ;
	}

	return 0 ;
}
