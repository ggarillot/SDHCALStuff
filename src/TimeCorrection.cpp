#include "TimeCorrection.h"

#include <sstream>

#include <TF1.h>
#include <TProfile.h>
#include <limits>
#include <array>

#include <TCanvas.h>
#include <TStyle.h>
#include <TApplication.h>

std::string TimeCorrection::getFuncStr(int polOrder) const
{
	std::stringstream funcStr ;

	for ( int i = 0 ; i <= polOrder ; ++i )
	{
		if ( i > 0 )
			funcStr << " + " ;

		funcStr << "[" << i << "] " ;

		for ( int j = 0 ; j < i ; ++j )
			funcStr << "*x" ;
	}
	return funcStr.str() ;
}

void TimeCorrection::correctHits(EventList& eventList, int polOrder)
{
	auto events = eventList.eventsNotConst() ;
	double timeOfSpill = (endTime-beginTime)*200e-9 ;

	auto funcStr = getFuncStr(polOrder) ;

	std::array<TProfile*,4> profArray = {} ;
	std::array<TF1*,4> fitArray = {} ;

	for ( unsigned int i = 0 ; i < 4 ; ++i )
	{
		std::stringstream toto ; toto << "prof" << i ;
		auto prof = new TProfile(toto.str().c_str() , toto.str().c_str() , 50 , 0 , timeOfSpill ) ;
		prof->SetDirectory(0) ;
		profArray.at(i) = prof ;

		std::stringstream toto2 ; toto2 << "fit" << i ;
		auto fit = new TF1(toto2.str().c_str() , funcStr.c_str() ) ;
		fitArray.at(i) = fit ;
	}

	for ( auto& event : events )
	{
		double time = (event->spillEventTime - beginTime)*200e-9 ;

		profArray.at(0)->Fill(time , event->nHit) ;
		profArray.at(1)->Fill(time , event->nHit1) ;
		profArray.at(2)->Fill(time , event->nHit2) ;
		profArray.at(3)->Fill(time , event->nHit3) ;
	}

	for ( unsigned int i = 0 ; i < 4 ; ++i )
		profArray.at(i)->Fit(fitArray.at(i) , "QB") ;

	for ( auto& event : events )
	{
		double time = (event->spillEventTime - beginTime)*200e-9 ;

		event->nHit -= ( fitArray.at(0)->Eval(time) - fitArray.at(0)->GetParameter(0) ) ;
		event->nHit1 -= ( fitArray.at(1)->Eval(time) - fitArray.at(1)->GetParameter(0) ) ;
		event->nHit2 -= ( fitArray.at(2)->Eval(time) - fitArray.at(2)->GetParameter(0) ) ;
		event->nHit3 -= ( fitArray.at(3)->Eval(time) - fitArray.at(3)->GetParameter(0) ) ;
	}

	for ( unsigned int i = 0 ; i < 4 ; ++i )
	{
		delete fitArray.at(i) ;
		delete profArray.at(i) ;
	}
}

void TimeCorrection::correctProfiles(EventList& eventList, int polOrder)
{
	auto events = eventList.eventsNotConst() ;
	double timeOfSpill = (endTime-beginTime)*200e-9 ;

	auto funcStr = getFuncStr(polOrder) ;

	std::vector<TProfile*> longiProfVec ;
	std::vector<TProfile*> radiProfVec ;

	std::vector<TF1*> longiProfVecFit ;
	std::vector<TF1*> radiProfVecFit ;

	for ( unsigned int i = 0 ; i < 48 ; ++i )
	{
		std::stringstream toto ; toto << "lp" << i ;
		TF1* lp = new TF1(toto.str().c_str() , funcStr.c_str() ) ;
		toto << "p" ;
		TProfile* prof = new TProfile(toto.str().c_str() , toto.str().c_str() , 50 , 0 , timeOfSpill ) ;
		prof->SetDirectory(0) ;
		longiProfVec.push_back(prof) ;
		longiProfVecFit.push_back(lp) ;

		std::stringstream toto2 ; toto2 << "rp" << i ;
		TF1* rp = new TF1(toto2.str().c_str() , funcStr.c_str() ) ;
		toto << "p" ;
		TProfile* prof2 = new TProfile(toto2.str().c_str() , toto2.str().c_str() , 50 , 0 , timeOfSpill ) ;
		prof2->SetDirectory(0) ;
		radiProfVec.push_back(prof2) ;
		radiProfVecFit.push_back(rp) ;
	}

	for ( auto& event : events )
	{
		double time = (event->spillEventTime - beginTime)*200e-9 ;

		for ( unsigned int i = 0 ; i < 48 ; ++i )
		{
			longiProfVec.at(i)->Fill(time , event->longiProfile.at(i) ) ;
			radiProfVec.at(i)->Fill(time , event->radiProfile.at(i) ) ;
		}
	}

	for ( unsigned int i = 0 ; i < 48 ; ++i )
	{
		longiProfVec.at(i)->Fit(longiProfVecFit.at(i) , "QB") ;
		radiProfVec.at(i)->Fit(radiProfVecFit.at(i) , "QB") ;
	}

	for ( auto& event : events )
	{
		double time = (event->spillEventTime - beginTime)*200e-9 ;

		for ( unsigned int i = 0 ; i < 48 ; ++i )
		{
			event->longiProfile.at(i) -= ( longiProfVecFit.at(i)->Eval(time) - longiProfVecFit.at(i)->GetParameter(0) ) ;
			event->radiProfile.at(i) -= ( radiProfVecFit.at(i)->Eval(time) - radiProfVecFit.at(i)->GetParameter(0) ) ;

			if ( event->longiProfile.at(i) < 0 )
				event->longiProfile.at(i) = 0 ;
			if ( event->radiProfile.at(i) < 0 )
				event->radiProfile.at(i) = 0 ;
		}
	}

	for ( unsigned int i = 0 ; i < 48 ; ++i )
	{
		delete longiProfVecFit.at(i) ;
		delete radiProfVecFit.at(i) ;
		delete longiProfVec.at(i) ;
		delete radiProfVec.at(i) ;
	}
}

void TimeCorrection::correctThrDensity(std::vector<std::shared_ptr<Event>>& events, int polOrder)
{
	auto funcStr = getFuncStr(polOrder) ;

	double timeOfSpill = (endTime-beginTime)*200e-9 ;

	std::array< std::array<TProfile*,10> , 4 > profileArray = {{}} ;
	for ( unsigned int i = 0 ; i < 4 ; ++i )
		for ( unsigned int j = 0 ; j < 10 ; ++j )
		{
			std::stringstream toto ; toto << "prof" << i << j ;
			profileArray.at(i).at(j) = new TProfile(toto.str().c_str() , toto.str().c_str() , 50 , 0 , timeOfSpill ) ;
		}

	for ( auto& event : events )
	{
		double time = (event->spillEventTime - beginTime)*200e-9 ;

		for ( unsigned int i = 0 ; i < 4 ; ++i )
			for ( unsigned int j = 0 ; j < 10 ; ++j )
				profileArray.at(i).at(j)->Fill(time , event->hitThrDensity.at(i).at(j)) ;
	}

	std::array< std::array<TF1*,10> , 4 > fitArray = {{}} ;
	for ( unsigned int i = 0 ; i < 4 ; ++i )
		for ( unsigned int j = 0 ; j < 10 ; ++j )
		{
			std::stringstream toto ; toto << "fit" << i << j ;
			fitArray.at(i).at(j) = new TF1(toto.str().c_str() , funcStr.c_str() ) ;

			profileArray.at(i).at(j)->Fit(fitArray.at(i).at(j) , "QB") ;
		}

	for ( auto& event : events )
	{
		double time = (event->spillEventTime - beginTime)*200e-9 ;

		for ( unsigned int i = 0 ; i < 4 ; ++i )
			for ( unsigned int j = 0 ; j < 10 ; ++j )
				event->hitThrDensity.at(i).at(j) -= ( fitArray.at(i).at(j)->Eval(time) - fitArray.at(i).at(j)->GetParameter(0) ) ;
	}

	for ( unsigned int i = 0 ; i < 4 ; ++i )
		for ( unsigned int j = 0 ; j < 10 ; ++j )
		{
			delete fitArray.at(i).at(j) ;
			delete profileArray.at(i).at(j) ;
		}
}

void TimeCorrection::correctThrDensity(EventList& eventList, int polOrder)
{
	correctThrDensity( eventList.eventsNotConst() , polOrder) ;
}































