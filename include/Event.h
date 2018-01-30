#ifndef Event2_h
#define Event2_h

#include <vector>
#include <map>
#include <array>

#include <TROOT.h>
#include <TTree.h>

#include <iostream>


struct Event
{
		Event() ;

		float energy = 0 ;
		int nHit = 0 ;
		int nHit1 = 0 ;
		int nHit2 = 0 ;
		int nHit3 = 0 ;
		int nHough = 0 ;
		int nHough1 = 0 ;
		int nHough2 = 0 ;
		int nHough3 = 0 ;

		float nHitCustom = 0 ;
		float nHit1Custom = 0 ;
		float nHit2Custom = 0 ;
		float nHit3Custom = 0 ;

		double density = 0 ;

		int nLayer = 0 ;
		int nTrack = 0 ;
		bool neutral = false ;
		double begin = 0 ;
		double end = 0 ;
		int nInteractingLayer = 0 ;
		float transverseRatio = 0 ;

		float cog[4] = {0,0,0,0} ;

		std::vector<double> longiProfile = {} ;
		std::vector<double> radiProfile = {} ;

		ULong64_t spillEventTime = 0 ;

		double emFraction = 0 ;

		std::vector<float> thr = {} ;
		std::vector<float> densityPerHit = {} ;

		std::array< std::array<int,10> , 4 > hitThrDensity = {{}} ;
} ;

class EventReader
{
	public :
		EventReader() ;
		~EventReader() ;

		Event getEvent(Long64_t entry , bool computeDensity = false) ;

		void setTree(TTree* _tree) ;

		void setTreeWrite(TTree* _tree) ;


		EventReader(const EventReader &toCopy) = delete ;
		void operator=(const EventReader &toCopy) = delete ;


	protected :
		TTree* tree = nullptr ;

	protected :
		float energy = 0 ;
		int nHit = 0 ;
		int nHit1 = 0 ;
		int nHit2 = 0 ;
		int nHit3 = 0 ;
		int nHough = 0 ;
		int nHough1 = 0 ;
		int nHough2 = 0 ;
		int nHough3 = 0 ;

		float nHitCustom = 0 ;
		float nHit1Custom = 0 ;
		float nHit2Custom = 0 ;
		float nHit3Custom = 0 ;

		double density = 0 ;

		int nLayer = 0 ;
		int nTrack = 0 ;
		bool neutral = 0 ;
		double begin = 0 ;
		double end = 0 ;
		int nInteractingLayer = 0 ;
		float transverseRatio = 0 ;

		float cog[4] = {0,0,0,0} ;

		std::vector<double>* longiProfile = nullptr ;
		std::vector<double>* radiProfile = nullptr ;

		ULong64_t spillEventTime = 0 ;

		double emFraction = 0 ;

		std::vector<float>* thr = nullptr ;
		std::vector<float>* densityPerHit = nullptr ;
} ;


#endif //Event.h
