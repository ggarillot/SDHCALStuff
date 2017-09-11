#include "TimeCorrection.h"

#include <sstream>

#include <TF1.h>
#include <TProfile.h>
#include <limits>

#include <TCanvas.h>
#include <TStyle.h>
#include <TApplication.h>

TimeCorrection::TimeCorrection()
{

}

void TimeCorrection::correct(std::vector<Event>& eventList , int polOrder)
{
	TProfile* hnHit = new TProfile("hnHit" , "hnHit" , 50 , 0 , 7 ) ;
	TProfile* hnHit1 = new TProfile("hnHit1" , "hnHit1" , 50 , 0 , 7 ) ;
	TProfile* hnHit2 = new TProfile("hnHit2" , "hnHit2" , 50 , 0 , 7 ) ;
	TProfile* hnHit3 = new TProfile("hnHit3" , "hnHit3" , 50 , 0 , 7 ) ;

	std::vector<TProfile*> longiProfVec ;
	std::vector<TProfile*> radiProfVec ;

	std::vector<TF1*> longiProfVecFit ;
	std::vector<TF1*> radiProfVecFit ;

	for ( unsigned int i = 0 ; i < 48 ; ++i )
	{
		std::stringstream toto ; toto << "lp" << i ;
		TF1* lp = new TF1(toto.str().c_str() , "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x") ;
		toto << "p" ;
		TProfile* prof = new TProfile(toto.str().c_str() , toto.str().c_str() , 70 , 0 , 7 ) ;
		longiProfVec.push_back(prof) ;
		longiProfVecFit.push_back(lp) ;

		std::stringstream toto2 ; toto2 << "rp" << i ;
		TF1* rp = new TF1(toto2.str().c_str() , "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x") ;
		toto << "p" ;
		TProfile* prof2 = new TProfile(toto2.str().c_str() , toto2.str().c_str() , 70 , 0 , 7 ) ;
		radiProfVec.push_back(prof2) ;
		radiProfVecFit.push_back(rp) ;
	}


	double begin = beginTime*200e-9 ;


	for ( std::vector<Event>::iterator it = eventList.begin() ; it != eventList.end() ; ++it )
	{
		Event& event = *it ;
		double time = event.spillEventTime*200e-9 - begin ;

		hnHit->Fill(time , event.nHit) ;
		hnHit1->Fill(time , event.nHit1) ;
		hnHit2->Fill(time , event.nHit2) ;
		hnHit3->Fill(time , event.nHit3) ;

		for ( unsigned int i = 0 ; i < 48 ; ++i )
		{
			longiProfVec.at(i)->Fill(time , event.longiProfile.at(i) ) ;
			radiProfVec.at(i)->Fill(time , event.radiProfile.at(i) ) ;
		}
	}

	std::stringstream funcStr ;

	for (int i = 0 ; i <= polOrder ; ++i )
	{
		if ( i > 0)
			funcStr << " + " ;

		funcStr << "[" << i << "] " ;

		for ( int j = 0 ; j < i ; ++j )
			funcStr << "*x" ;
	}

	TF1* f = new TF1("f" , funcStr.str().c_str() ) ;
	TF1* f1 = new TF1("f1" , funcStr.str().c_str() ) ;
	TF1* f2 = new TF1("f2" , funcStr.str().c_str() ) ;
	TF1* f3 = new TF1("f3" , funcStr.str().c_str() ) ;


//	f->SetParLimits(2 , 0 , 50000) ;
//	f1->SetParLimits(2 , 0 , 50000) ;
//	f2->SetParLimits(2 , 0 , 50000) ;
//	f3->SetParLimits(2 , 0 , 50000) ;

	hnHit->Fit(f , "QB") ;
	hnHit1->Fit(f1 , "QB") ;
	hnHit2->Fit(f2 , "QB") ;
	hnHit3->Fit(f3 , "QB") ;


	for ( unsigned int i = 0 ; i < 48 ; ++i )
	{
		longiProfVec.at(i)->Fit(longiProfVecFit.at(i) , "QB") ;
		radiProfVec.at(i)->Fit(radiProfVecFit.at(i) , "QB") ;
	}


	std::cout << "Time Origin : " << std::endl ;
	std::cout << "pol " << polOrder << "  nHit : " << f->GetParameter(0) << std::endl ;
//	std::cout << "nHit1 : " << f1->GetParameter(0) << std::endl ;
//	std::cout << "nHit2 : " << f2->GetParameter(0) << std::endl ;
//	std::cout << "nHit3 : " << f3->GetParameter(0) << std::endl ;


	for ( std::vector<Event>::iterator it = eventList.begin() ; it != eventList.end() ; ++it )
	{
		Event& event = *it ;
		double time = event.spillEventTime*200e-9 - begin ;

		for ( unsigned int i = 0 ; i < 48 ; ++i )
		{
			event.longiProfile.at(i) -= ( longiProfVecFit.at(i)->Eval(time) - longiProfVecFit.at(i)->GetParameter(0) ) ;
			event.radiProfile.at(i) -= ( radiProfVecFit.at(i)->Eval(time) - radiProfVecFit.at(i)->GetParameter(0) ) ;
		}

		event.nHit -= ( f->Eval(time) - f->GetParameter(0) ) ;
		event.nHit1 -= ( f1->Eval(time) - f1->GetParameter(0) ) ;
		event.nHit2 -= ( f2->Eval(time) - f2->GetParameter(0) ) ;
		event.nHit3 -= ( f3->Eval(time) - f3->GetParameter(0) ) ;

	}

	delete f ;
	delete f1 ;
	delete f2 ;
	delete f3 ;
	delete hnHit ;
	delete hnHit1 ;
	delete hnHit2 ;
	delete hnHit3 ;

	for ( unsigned int i = 0 ; i < 48 ; ++i )
	{
		delete longiProfVecFit.at(i) ;
		delete radiProfVecFit.at(i) ;
		delete longiProfVec.at(i) ;
		delete radiProfVec.at(i) ;
	}
}

