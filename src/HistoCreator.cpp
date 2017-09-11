#include "HistoCreator.h"

#include <iostream>

HistoCreator::HistoCreator()
	: eventList()
{
}

HistoCreator::~HistoCreator()
{
}


TProfile* HistoCreator::getNHitOverTimeProfile(std::string profName , double timeRef) const
{
	TProfile* prof = new TProfile(profName.c_str() , "" , 50 , 0 , 7 ) ;

	for ( std::vector<Event>::const_iterator it = eventList.begin() ; it != eventList.end() ; ++it )
	{
		const Event& event = *it ;
		double time = event.spillEventTime*200e-9 - timeRef ;

		prof->Fill(time , event.nHit) ;
	}

	return prof ;
}

TProfile* HistoCreator::getNHit1OverTimeProfile(std::string profName , double timeRef) const
{
	TProfile* prof = new TProfile(profName.c_str() , "" , 50 , 0 , 7 ) ;

	for ( std::vector<Event>::const_iterator it = eventList.begin() ; it != eventList.end() ; ++it )
	{
		const Event& event = *it ;
		double time = event.spillEventTime*200e-9 - timeRef ;

		prof->Fill(time , event.nHit1) ;
	}

	return prof ;
}

TProfile* HistoCreator::getNHit2OverTimeProfile(std::string profName , double timeRef) const
{
	TProfile* prof = new TProfile(profName.c_str() , "" , 50 , 0 , 7 ) ;

	for ( std::vector<Event>::const_iterator it = eventList.begin() ; it != eventList.end() ; ++it )
	{
		const Event& event = *it ;
		double time = event.spillEventTime*200e-9 - timeRef ;

		prof->Fill(time , event.nHit2) ;
	}

	return prof ;
}

TProfile* HistoCreator::getNHit3OverTimeProfile(std::string profName , double timeRef) const
{
	TProfile* prof = new TProfile(profName.c_str() , "" , 50 , 0 , 7 ) ;

	for ( std::vector<Event>::const_iterator it = eventList.begin() ; it != eventList.end() ; ++it )
	{
		const Event& event = *it ;
		double time = event.spillEventTime*200e-9 - timeRef ;

		prof->Fill(time , event.nHit3) ;
	}

	return prof ;
}

TProfile* HistoCreator::getMeanRadiusOverTimeProfile(std::string profName , double timeRef) const
{
	TProfile* prof = new TProfile(profName.c_str() , "" , 50 , 0 , 7 ) ;

	for ( std::vector<Event>::const_iterator it = eventList.begin() ; it != eventList.end() ; ++it )
	{
		const Event& event = *it ;
		double time = event.spillEventTime*200e-9 - timeRef ;

		double mean = 0 ;
		double norm = 0 ;
		for ( unsigned int i = 0 ; i < event.radiProfile.size() && i < 48 ; ++i )
		{
			mean += event.radiProfile.at(i) * (i+1) ;
			norm += event.radiProfile.at(i) ;
		}

		mean /= norm ;

		prof->Fill(time , mean) ;
	}

	return prof ;
}

double HistoCreator::getMeanRadius() const
{
	double meanRadius = 0 ;

	for ( std::vector<Event>::const_iterator it = eventList.begin() ; it != eventList.end() ; ++it )
	{
		const Event& event = *it ;

		double mean = 0 ;
		double norm = 0 ;

		for ( unsigned int i = 0 ; i < event.radiProfile.size() && i < 48 ; ++i )
		{
			mean += event.radiProfile.at(i) * (i+1) ;
			norm += event.radiProfile.at(i) ;
		}

		mean /= norm ;
		meanRadius += mean ;
	}

	meanRadius /= eventList.size() ;
	return meanRadius ;
}

TH1D* HistoCreator::getNHit(std::string histName) const
{
	int max = static_cast<int>( eventList.at(0).energy*20 + 300 ) ;

	TH1D* histo = new TH1D( histName.c_str() , "" , 100 , 0 , max) ;

	if ( dataStyle )
	{
		histo->Sumw2() ;
		histo->SetMarkerStyle(20) ;
		histo->SetMarkerColor(kBlack) ;
		histo->SetLineColor(kBlack) ;
		histo->SetFillStyle(0) ;
	}
	else
	{
		histo->SetLineColor( static_cast<Color_t>(color->GetNumber()) ) ;
		histo->SetFillColor( static_cast<Color_t>(fillColor->GetNumber()) ) ;
	}


	for ( std::vector<Event>::const_iterator it = eventList.begin() ; it != eventList.end() ; ++it )
	{
		const Event& event = *it ;
		histo->Fill( event.nHit ) ;
	}

	return histo ;
}


TH1D* HistoCreator::getLongiProfile(std::string histName) const
{
	TH1D* histo = new TH1D( histName.c_str() , "" , 50 , 0 , 50) ;

	if ( dataStyle )
	{
		histo->Sumw2() ;
		histo->SetMarkerStyle(20) ;
		histo->SetMarkerColor(kBlack) ;
		histo->SetLineColor(kBlack) ;
		histo->SetFillStyle(0) ;
	}
	else
	{
		histo->SetLineColor( static_cast<Color_t>(color->GetNumber()) ) ;
		histo->SetFillColor( static_cast<Color_t>(fillColor->GetNumber()) ) ;
	}


	for ( std::vector<Event>::const_iterator it = eventList.begin() ; it != eventList.end() ; ++it )
	{
		const Event& event = *it ;

		unsigned int realBegin = static_cast<unsigned int>( event.begin/26.131 + 0.5) - 1 ;

		//		if ( realBegin < 0 )
		//			continue ;

		if ( realBegin > 20 )
			continue ;

		//		std::cout << event.longiProfile->at(0) << std::endl ;

		int showerI = 0 ;
		for ( unsigned int i = realBegin ; i < event.longiProfile.size() ; i++ )
		{
//			histo->Fill( showerI , event.longiProfile->at(i)/event.nHit ) ;
						histo->Fill( showerI , event.longiProfile.at(i) ) ;
			showerI++ ;
		}
	}

	return histo ;
}

TH1D* HistoCreator::getRadiProfile(std::string histName) const
{
	TH1D* histo = new TH1D( histName.c_str() , "" , 50 , 0 , 50) ;

	if ( dataStyle )
	{
		histo->Sumw2() ;
		histo->SetMarkerStyle(20) ;
		histo->SetMarkerColor(kBlack) ;
		histo->SetLineColor(kBlack) ;
		histo->SetFillStyle(0) ;
	}
	else
	{
		histo->SetLineColor( static_cast<Color_t>(color->GetNumber()) ) ;
		histo->SetFillColor( static_cast<Color_t>(fillColor->GetNumber()) ) ;
	}


	for ( std::vector<Event>::const_iterator it = eventList.begin() ; it != eventList.end() ; ++it )
	{
		const Event& event = *it ;

		unsigned int realBegin = static_cast<unsigned int>( event.begin/26.131 + 0.5) - 1 ;

		//		if ( realBegin < 0 )
		//			continue ;

		if ( realBegin > 20 )
			continue ;


		for ( unsigned int i = 0 ; i < event.radiProfile.size() && i < 49 ; i++ )
		{
//			histo->Fill( i , event.radiProfile->at(i)/event.nHit ) ;
			histo->Fill( i , event.radiProfile.at(i) ) ;
		}

	}

	return histo ;
}
