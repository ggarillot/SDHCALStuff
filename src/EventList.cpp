#include "EventList.h"

#include <TFile.h>
#include <TTree.h>

#include "TimeCorrection.h"

EventList::~EventList()
{
	std::cout << "~EventList()" << std::endl ;

	for ( auto& i : _events )
	{
		i.reset() ;
	}
}

void EventList::loadFile(std::string fileName)
{
	std::cout << "Load file : " << fileName << std::endl ;
	TFile* file = new TFile(fileName.c_str() , "READ") ;
	TTree* tree ;
	file->GetObject("tree" , tree) ;

	EventReader eventReader ;
	eventReader.setTree(tree) ;

	int iEntry = 0 ;
	while ( tree->GetEntry(iEntry++) )
	{
		auto event = std::make_shared<Event>( eventReader.getEvent(iEntry-1 , true) ) ;
		if ( !cut(*event) )
			continue ;

		_events.push_back( event ) ;
	}

	delete tree ;
	file->Close() ;
	delete file ;
}

void EventList::loadFile(std::string fileName , unsigned long long beginTime, unsigned long long endTime)
{
	std::cout << "Load file : " << fileName << ", and correct time with begin = " << beginTime << " to end = " << endTime << std::endl ;
	TimeCorrection timeCorr ;
	timeCorr.setBeginTime(beginTime) ;
	timeCorr.setEndTime(endTime) ;

	beginTimeCut = beginTime ;
	endTimeCut = endTime ;

	TFile* file = new TFile(fileName.c_str() , "READ") ;
	TTree* tree ;
	file->GetObject("tree" , tree) ;

	EventReader eventReader ;
	eventReader.setTree(tree) ;

	std::vector<std::shared_ptr<Event>> temp ;

	int iEntry = 0 ;
	while ( tree->GetEntry(iEntry++) )
	{
		auto event = std::make_shared<Event>( eventReader.getEvent(iEntry-1 , true) ) ;
		if ( !cut(*event) )
			continue ;

		temp.push_back(event) ;
	}

	timeCorr.correctThrDensity(temp) ;

	_events.insert( _events.end() , temp.begin() , temp.end() ) ;

	delete tree ;
	file->Close() ;
	delete file ;
}


bool EventList::cut(const Event& event) const
{
	bool beamCut = false ;

	if (geomCut.at(2) < std::numeric_limits<double>::epsilon())
		beamCut = true ;
	else
		beamCut = ( event.cog[0]-geomCut.at(0) )*( event.cog[0]-geomCut.at(0) ) + ( event.cog[2]-geomCut.at(1) )*( event.cog[2]-geomCut.at(1) ) < geomCut.at(2)*geomCut.at(2) ;

	bool timeCut = ( event.spillEventTime >= beginTimeCut && event.spillEventTime <= endTimeCut ) ;

	bool cut = ( event.transverseRatio > 0.05f && event.neutral == 0 && double(event.nHit)/event.nLayer > 2.2 && double(event.nInteractingLayer)/event.nLayer > 0.2 ) ;

//	bool hadronCut = (event.begin > 5 || event.nLayer > 30 ) && event.nTrack > 0 ;
	bool hadronCut = true ;
	//	bool cut = ( event.begin < 4 && event.nLayer < 30 && event.transverseRatio > 0.05f && event.neutral == 0 && event.nTrack == 0 && double(event.nHit)/event.nLayer > 2.2 && double(event.nInteractingLayer)/event.nLayer > 0.2 ) ;
//	bool elecCut = ( event.begin < 5 && event.nLayer < 30 ) ;
	bool elecCut = true ;

	return ( cut && beamCut && elecCut && hadronCut && timeCut ) ;
}
