#include "HistoCreator.h"


#include <iostream>

HistoCreator::HistoCreator()
{
}

HistoCreator::~HistoCreator()
{
}


TProfile* HistoCreator::getNHitOverTimeProfile(std::string profName , double timeRef) const
{
	TProfile* prof = new TProfile(profName.c_str() , "" , 50 , 0 , 7 ) ;

	for ( const auto& event : eventList )
	{
		double time = event->spillEventTime*200e-9 - timeRef ;
		prof->Fill(time , event->nHit) ;
	}

	return prof ;
}

TProfile* HistoCreator::getNHit1OverTimeProfile(std::string profName , double timeRef) const
{
	TProfile* prof = new TProfile(profName.c_str() , "" , 50 , 0 , 7 ) ;

	for ( const auto& event : eventList )
	{
		double time = event->spillEventTime*200e-9 - timeRef ;
		prof->Fill(time , event->nHit1) ;
	}

	return prof ;
}

TProfile* HistoCreator::getNHit2OverTimeProfile(std::string profName , double timeRef) const
{
	TProfile* prof = new TProfile(profName.c_str() , "" , 50 , 0 , 7 ) ;

	for ( const auto& event : eventList )
	{
		double time = event->spillEventTime*200e-9 - timeRef ;
		prof->Fill(time , event->nHit2) ;
	}

	return prof ;
}

TProfile* HistoCreator::getNHit3OverTimeProfile(std::string profName , double timeRef) const
{
	TProfile* prof = new TProfile(profName.c_str() , "" , 50 , 0 , 7 ) ;

	for ( const auto& event : eventList )
	{
		double time = event->spillEventTime*200e-9 - timeRef ;
		prof->Fill(time , event->nHit3) ;
	}

	return prof ;
}

TProfile* HistoCreator::getMeanRadiusOverTimeProfile(std::string profName , double timeRef) const
{
	TProfile* prof = new TProfile(profName.c_str() , "" , 50 , 0 , 7 ) ;

	for ( const auto& event : eventList )
	{
		double time = event->spillEventTime*200e-9 - timeRef ;

		double mean = 0 ;
		double norm = 0 ;
		for ( unsigned int i = 0 ; i < event->radiProfile.size() && i < 48 ; ++i )
		{
			mean += event->radiProfile.at(i) * (i+1) ;
			norm += event->radiProfile.at(i) ;
		}

		mean /= norm ;

		prof->Fill(time , mean) ;
	}

	return prof ;
}

double HistoCreator::getMeanRadius() const
{
	double meanRadius = 0 ;

	for ( const auto& event : eventList )
	{
		double mean = 0 ;

		for ( unsigned int i = 0 ; i < event->radiProfile.size() && i < 48 ; ++i )
			mean += event->radiProfile.at(i)*event->nHit * i ;

		mean /= event->nHit ;
		meanRadius += mean ;
	}

	meanRadius /= eventList.size() ;
	return meanRadius ;
}

TH1D* HistoCreator::getNHit(std::string histName , int thr , bool custom) const
{
	int nBins = 70 ;
	int max = static_cast<int>( eventList.at(0)->energy*20 + 300 ) ;

	double perBin = 1.0*max/nBins ;

	max += nBins*(static_cast<int>(perBin+1) - perBin) ;

	TH1D* histo = new TH1D( histName.c_str() , "" , nBins , 0 , max) ;

	histo->GetXaxis()->SetLabelSize(0.025f) ;
	histo->GetYaxis()->SetLabelSize(0.025f) ;

	histo->GetYaxis()->SetTitleOffset(1.5f) ;

	histo->GetXaxis()->SetTitleFont(62) ;
	histo->GetYaxis()->SetTitleFont(62) ;

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
		histo->SetLineWidth(2) ;
		histo->SetLineColor( static_cast<Color_t>(color->GetNumber()) ) ;
		histo->SetFillColor( static_cast<Color_t>(fillColor->GetNumber()) ) ;
	}


	for ( const auto& event : eventList )
	{
		float nHit = static_cast<float>(event->nHit) ;
		if (custom)
			nHit = event->nHitCustom ;
		histo->Fill( nHit ) ;
	}

	for ( int i = 0 ; i < nBins ; ++i )
	{
		histo->SetBinContent( i+1 , histo->GetBinContent(i+1)/eventList.size() ) ;
		if ( dataStyle )
			histo->SetBinError( i+1 , histo->GetBinError(i+1)/eventList.size() ) ;
	}

	return histo ;
}

TH1D* HistoCreator::getLongiProfile(std::string histName) const
{
	std::array<double,48> values = {} ;

	TH1D* histo = new TH1D( histName.c_str() , "" , 48 , -0.5 , 47.5) ;

	histo->GetXaxis()->SetLabelSize(0.025f) ;
	histo->GetYaxis()->SetLabelSize(0.025f) ;

	histo->GetYaxis()->SetTitleOffset(1.5f) ;

	histo->GetXaxis()->SetTitleFont(62) ;
	histo->GetYaxis()->SetTitleFont(62) ;

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


	for ( const auto& event : eventList )
	{
//		unsigned int realBegin = static_cast<unsigned int>( event->begin/26.131 + 0.5) - 1 ;
		unsigned int realBegin = static_cast<unsigned int>( event->begin) ;

		if ( realBegin > 20 )
			continue ;

		unsigned int showerI = 0 ;
		for ( unsigned int i = realBegin ; i < event->longiProfile.size() ; i++ )
		{
			values[showerI] += event->longiProfile.at(i) ;
			showerI++ ;

			if ( showerI > 47 )
				break ;
		}
	}

	for ( auto& i : values )
		i /= eventList.size() ;

	for ( unsigned int i = 0 ; i < 48 ; ++i )
	{
		histo->SetBinContent( i + 1 , values[i] ) ;
		histo->SetBinError( i + 1 , 0 ) ;
	}

	return histo ;
}

TH1D* HistoCreator::getRadiProfile(std::string histName) const
{
	std::array<double,40> values = {} ;

	TH1D* histo = new TH1D( histName.c_str() , "" , 36 , -0.5 , 35.5) ;

	histo->GetXaxis()->SetLabelSize(0.025f) ;
	histo->GetYaxis()->SetLabelSize(0.025f) ;

	histo->GetYaxis()->SetTitleOffset(1.5f) ;

	histo->GetXaxis()->SetTitleFont(62) ;
	histo->GetYaxis()->SetTitleFont(62) ;

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


	for ( const auto& event : eventList )
	{
//		unsigned int realBegin = static_cast<unsigned int>( event->begin/26.131 + 0.5) - 1 ;
		unsigned int realBegin = static_cast<unsigned int>( event->begin) ;
		if ( realBegin > 20 )
			continue ;

		for ( unsigned int i = 0 ; i < event->radiProfile.size() && i < 40 ; i++ )
			values[i] += event->radiProfile[i] / ( 3.1415926536*( 2*(i+1)-1 ) ) ;
	}

	for ( auto& i : values )
		i /= eventList.size() ;

	for ( unsigned int i = 0 ; i < 36 ; ++i )
	{
		histo->SetBinContent( i+1 , values[i] ) ;
		histo->SetBinError( i+1 , 0 ) ;
	}

	return histo ;
}
