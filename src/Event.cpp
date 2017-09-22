#include "Event.h"


using namespace std ;

Event::Event()
{
}

EventReader::EventReader()
{
}

EventReader::~EventReader()
{
}


Event EventReader::getEvent(Long64_t entry)
{
	Event event ;
	int isOk = tree->GetEntry(entry) ;

	if ( isOk == 0 )
		return event ;

	event.energy = energy ;
	event.nHit = nHit ;
	event.nHit1 = nHit1 ;
	event.nHit2 = nHit2 ;
	event.nHit3 = nHit3 ;
	event.nHough = nHough ;
	event.nHough1 = nHough1 ;
	event.nHough2 = nHough2 ;
	event.nHough3 = nHough3 ;

	event.nHitCustom = nHitCustom ;
	event.nHit1Custom = nHit1Custom ;
	event.nHit2Custom = nHit2Custom ;
	event.nHit3Custom = nHit3Custom ;

	event.density = density ;

	event.nLayer = nLayer ;
	event.nTrack = nTrack ;
	event.neutral = neutral ;
	event.begin = begin ;
	event.end = end ;
	event.nInteractingLayer = nInteractingLayer ;
	event.transverseRatio = transverseRatio ;

	for ( int i = 0 ; i < 4 ; ++i )
		event.cog[i] = cog[i] ;

	event.longiProfile = *longiProfile ;
	event.radiProfile = *radiProfile ;

	event.spillEventTime = spillEventTime ;

	event.emFraction = emFraction ;

	return event ;
}

void EventReader::setTree(TTree* _tree)
{
	tree = _tree ;
	tree->SetBranchAddress("energy" , &energy) ;
	tree->SetBranchAddress("nHit" , &nHit) ;
	tree->SetBranchAddress("nHit1" , &nHit1) ;
	tree->SetBranchAddress("nHit2" , &nHit2) ;
	tree->SetBranchAddress("nHit3" , &nHit3) ;
	tree->SetBranchAddress("nHough" , &nHough) ;
	tree->SetBranchAddress("nHough1" , &nHough1) ;
	tree->SetBranchAddress("nHough2" , &nHough2) ;
	tree->SetBranchAddress("nHough3" , &nHough3) ;

	tree->SetBranchAddress("nHitCustom" , &nHitCustom) ;
	tree->SetBranchAddress("nHit1Custom" , &nHit1Custom) ;
	tree->SetBranchAddress("nHit2Custom" , &nHit2Custom) ;
	tree->SetBranchAddress("nHit3Custom" , &nHit3Custom) ;

	tree->SetBranchAddress("longiProfile" , &longiProfile) ;
	tree->SetBranchAddress("radiProfile" , &radiProfile) ;
	tree->SetBranchAddress("density" , &density) ;

	tree->SetBranchAddress("nLayer" , &nLayer) ;
	tree->SetBranchAddress("begin" , &begin) ;
	tree->SetBranchAddress("end" , &end) ;
	tree->SetBranchAddress("nInteractingLayer" , &nInteractingLayer) ;
	tree->SetBranchAddress("transverseRatio" , &transverseRatio) ;

	tree->SetBranchAddress("nTrack" , &nTrack) ;
	tree->SetBranchAddress("neutral" , &neutral) ;
	tree->SetBranchAddress("spillEventTime" , &spillEventTime) ;

	tree->SetBranchAddress("emFraction" , &emFraction) ;
	tree->SetBranchAddress("thrust" , &cog) ;
}

void EventReader::setTreeWrite(TTree* _tree)
{
	_tree->Branch("energy" , &energy) ;
	_tree->Branch("nHit" , &nHit) ;
	_tree->Branch("nHit1" , &nHit1) ;
	_tree->Branch("nHit2" , &nHit2) ;
	_tree->Branch("nHit3" , &nHit3) ;
	_tree->Branch("nHough" , &nHough) ;
	_tree->Branch("nHough1" , &nHough1) ;
	_tree->Branch("nHough2" , &nHough2) ;
	_tree->Branch("nHough3" , &nHough3) ;

	_tree->Branch("nHitCustom" , &nHitCustom) ;
	_tree->Branch("nHit1Custom" , &nHit1Custom) ;
	_tree->Branch("nHit2Custom" , &nHit2Custom) ;
	_tree->Branch("nHit3Custom" , &nHit3Custom) ;

	_tree->Branch("longiProfile" , &longiProfile) ;
	_tree->Branch("radiProfile" , &radiProfile) ;
	_tree->Branch("density" , &density) ;

	_tree->Branch("nLayer" , &nLayer) ;
	_tree->Branch("begin" , &begin) ;
	_tree->Branch("end" , &end) ;
	_tree->Branch("nInteractingLayer" , &nInteractingLayer) ;
	_tree->Branch("transverseRatio" , &transverseRatio) ;

	_tree->Branch("nTrack" , &nTrack) ;
	_tree->Branch("neutral" , &neutral) ;
	_tree->Branch("spillEventTime" , &spillEventTime) ;

	_tree->Branch("emFraction" , &emFraction) ;
	_tree->Branch("thrust" , &cog , "thrust/D") ;
}
