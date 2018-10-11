#include <iostream>

#include <TROOT.h>
#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TDirectory.h>
#include <TColor.h>

#include <TH1.h>
#include <TH2.h>
#include <THistPainter.h>
#include <THStack.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TText.h>

#include <TimeCorrection.h>
#include <Event.h>

#include <array>
#include <map>
#include <sstream>
#include <string>

bool cutNormal(const Event& event)
{
	bool geomCut = (event.cog[0] - 535)*(event.cog[0] - 535) + (event.cog[2] - 555)*(event.cog[2] - 555) < 100*100 ;
	bool timeCut = event.spillEventTime > 5e6 && event.spillEventTime < 30e6 ;
	bool muonCut = (event.transverseRatio > 0.05f && 1.0*event.nHit/event.nLayer > 3 && 1.0*event.nInteractingLayer/event.nLayer > 0.2 && event.neutral == 0) ;

	return geomCut && timeCut && muonCut ;

}

bool cutShift(const Event& event)
{
	bool geomCut = (event.cog[0] - 270)*(event.cog[0] - 270) + (event.cog[2] - 350)*(event.cog[2] - 350) < 100*100 ;
	bool timeCut = event.spillEventTime > 5e6 && event.spillEventTime < 30e6 ;
	bool muonCut = (event.transverseRatio > 0.05f && 1.0*event.nHit/event.nLayer > 3 && 1.0*event.nInteractingLayer/event.nLayer > 0.2 && event.neutral == 0) ;

	return geomCut && timeCut && muonCut ;
}

bool cutShift2(const Event& event)
{
	bool geomCut = (event.cog[0] - 780)*(event.cog[0] - 780) + (event.cog[2] - 705)*(event.cog[2] - 705) < 100*100 ;
	bool timeCut = event.spillEventTime > 5e6 && event.spillEventTime < 30e6 ;
	bool muonCut = (event.transverseRatio > 0.05f && 1.0*event.nHit/event.nLayer > 3 && 1.0*event.nInteractingLayer/event.nLayer > 0.2 && event.neutral == 0) ;

	return geomCut && timeCut && muonCut ;
}

struct NormalOrShift
{
		std::map<int,int> runMap = {{}} ;
		std::map<int,TH1*> histoMap = {{}} ;

		std::string name = "Normal" ;
		bool isHomogenous = false ;

		Color_t col = kRed ;
		Color_t filCol = kRed-10 ;
		Style_t fillStyle = 0 ;

		std::array<double,3> geomCut = {{535,555,100}} ;

		//bool (*cut)(const Event& event) = &cutNormal ;
} ;

int main()
{
	std::string dataDir = "/home/garillot/files/Analysis/DATA/H2Sept2017/Ignore/" ;
	//	std::string dataDir = "/home/garillot/files/Analysis/DATA/H2Sept2017/Recover/" ;

	std::map<int,int> runNonHomCenterMap ;
	runNonHomCenterMap.insert( {80,736543} ) ;
	runNonHomCenterMap.insert( {60,736560} ) ;
	runNonHomCenterMap.insert( {50,736562} ) ;
	runNonHomCenterMap.insert( {40,736571} ) ;
	
	std::map<int,int> runNonHomShiftMap ;
	runNonHomShiftMap.insert( {80,736535} ) ;
	runNonHomShiftMap.insert( {60,736559} ) ;
	runNonHomShiftMap.insert( {50,736566} ) ;
	runNonHomShiftMap.insert( {40,736567} ) ;

	std::map<int,int> runNonHomShift2Map ;
	runNonHomShift2Map.insert( {80,736531} ) ;
	
	std::map<int,int> runHomCenterMap ;
	runHomCenterMap.insert( {80,736532} ) ;
	runHomCenterMap.insert( {60,736545} ) ;
	runHomCenterMap.insert( {50,736564} ) ;
	runHomCenterMap.insert( {40,736570} ) ;
	
	std::map<int,int> runHomShiftMap ;
	runHomShiftMap.insert( {80,736536} ) ;
	runHomShiftMap.insert( {60,736558} ) ;
	runHomShiftMap.insert( {50,736565} ) ;
	runHomShiftMap.insert( {40,736569} ) ;

	std::map<int,int> runHomShift2Map ;
	runHomShift2Map.insert( {80,736530} ) ;
	
	TFile* outputFile = new TFile("positionShift.root" , "RECREATE") ;

	std::map<TDirectory* , std::vector<NormalOrShift>> fullMap ;
	TDirectory* nonHomDir = outputFile->mkdir("Non Homogenous") ;
	TDirectory* homDir = outputFile->mkdir("Homogenous") ;

	NormalOrShift center ;
	center.name = "Center" ;
	center.geomCut = {{535,555,100}} ;
	center.col = kRed ;
	center.filCol = kRed-10 ;
	center.fillStyle = 1001 ;

	NormalOrShift shift ;
	shift.name = "x : -25 , y : -20" ;
	shift.geomCut = {{270,350,100}} ;
	shift.col = kBlue+2 ;
	shift.filCol = kBlue-10 ;
	shift.fillStyle = 3001 ;

	NormalOrShift shift2 ;
	shift2.name = "x : +25 , y : +15" ;
	shift2.geomCut = {{780,705,100}} ;
	shift2.col = kGreen+2 ;
	shift2.filCol = kGreen-4 ;
	shift2.fillStyle = 3003 ;

	fullMap.insert( { nonHomDir , {center , shift , shift2} } ) ;
	fullMap.insert( { homDir , {center , shift , shift2} } ) ;


	std::vector<TH2D*> histoBeamVec ;

	for ( auto& i : fullMap.at(homDir) )
		i.isHomogenous = true ;

	fullMap.at(nonHomDir).at(0).runMap = runNonHomCenterMap ;
	fullMap.at(nonHomDir).at(1).runMap = runNonHomShiftMap ;
	fullMap.at(nonHomDir).at(2).runMap = runNonHomShift2Map ;
	fullMap.at(homDir).at(0).runMap = runHomCenterMap ;
	fullMap.at(homDir).at(1).runMap = runHomShiftMap ;
	fullMap.at(homDir).at(2).runMap = runHomShift2Map ;

	EventReader eventReader ;

	for ( const int i : {80 , 60 , 50 , 40} )
	{
		for ( TDirectory* dir : {nonHomDir , homDir} )
		{
			dir->cd() ;
			std::vector<NormalOrShift>& posVec = fullMap.at(dir) ;

			std::stringstream tutu ; tutu << dir->GetName() << "_" << i << "GeV" ;

			gStyle->SetOptStat(0) ;
			TCanvas* c = new TCanvas( tutu.str().c_str() , tutu.str().c_str() , 900 , 900) ;

			c->SetTopMargin(0.05f) ;
			c->SetRightMargin(0.05f) ;
			c->SetFrameLineColor(kGray+3) ;
			c->SetTicks() ;

			TLegend* leg = new TLegend(0.15,0.80,0.35,0.90) ;
			leg->SetBorderSize(0) ;
			leg->SetTextColor(kGray+3) ;

			std::array<double,2> xLimits = {{0,0}} ;
			std::array<double,2> yLimits = {{0,0}} ;

			for ( auto pos = posVec.begin() ; pos < posVec.end() ; ++pos )
			{
				if ( pos->runMap.find(i) == pos->runMap.end() )
					continue ;

				if ( i == 80 && dir == nonHomDir )
				{
					std::stringstream r ; r << "histo_" << pos->name ;
					TH2D* histo = new TH2D( r.str().c_str() , ";x (mm);y (mm)" , 100 , 0 , 1000 , 100 , 0 , 1000 ) ;
					histo->SetLineColor(pos->col) ;
					histo->SetFillColor(pos->col) ;
					histo->SetMarkerColor(pos->col) ;
					histo->GetYaxis()->SetTitleOffset(1.5) ;
					histoBeamVec.push_back( histo ) ;
				}

				EventList eventList ;
				eventList.setType(EventList::kHadron) ;
				eventList.setGeomCut( pos->geomCut ) ;

				std::stringstream toto ; toto << dataDir << pos->runMap.at(i) << ".root" ;

				eventList.loadFile( toto.str() ) ;


				if ( i == 80 && dir == nonHomDir )
				{
					TH2D* histo = histoBeamVec.back() ;

					EventList eventList2 ;
					eventList2.setType(EventList::kNothing) ;
					eventList2.loadFile( toto.str() ) ;

					for ( const auto& event : eventList2.events() )
					{
						if ( event->cog[0] > 0 && event->cog[0] < 1000 && event->cog[2] > 0 && event->cog[2] < 1000 )
							histo->Fill(event->cog[2] , event->cog[0]) ;
					}
				}


				TimeCorrection timeCorr ;
				timeCorr.setBeginTime(5e6) ;

				timeCorr.correctHits(eventList) ;

				HistoCreator histCreator ;
				histCreator.setDataStyle(false) ;
				histCreator.setColor(pos->col) ;
				histCreator.setFillColor(pos->filCol) ;
				histCreator.setEventList( eventList.events() ) ;

				std::stringstream titi ; titi << pos->name << i << "GeV" ;

				TH1* histo = histCreator.getNHit(titi.str() , 0 , true) ;
				histo->SetDirectory(0) ;
				histo->SetFillStyle(pos->fillStyle) ;
				histo->SetLineWidth(2) ;

				xLimits.at(0) = std::min(xLimits.at(0) , histo->GetXaxis()->GetXmin() ) ;
				xLimits.at(1) = std::max(xLimits.at(1) , histo->GetXaxis()->GetXmax() ) ;

				yLimits.at(0) = std::min(yLimits.at(0) , histo->GetYaxis()->GetXmin() ) ;
				yLimits.at(1) = std::max(yLimits.at(1) , ( histo->GetMaximum() + std::sqrt( histo->GetMaximum()) )/ histo->Integral() ) ;

				histo->Scale( 1.0/histo->Integral() ) ;
				pos->histoMap.insert( {i , histo} ) ;

				leg->AddEntry(histo , pos->name.c_str() , "f" ) ;
			}

			c->cd() ;

			TH2D* range = new TH2D( tutu.str().c_str() , ";Nhit;" , 1 , xLimits.at(0) , xLimits.at(1) , 1 , yLimits.at(0) , yLimits.at(1) ) ;
			range->SetFillStyle(0) ;
			range->Draw() ;

			for ( const auto& pos : posVec )
			{
				if ( pos.histoMap.find(i) == pos.histoMap.end() )
					continue ;
				pos.histoMap.at(i)->Draw("HIST same") ;
			}

			TAxis* xAxis = range->GetXaxis() ;
			TAxis* yAxis = range->GetYaxis() ;
			xAxis->SetAxisColor(kGray+3) ;
			yAxis->SetAxisColor(kGray+3) ;
			xAxis->SetTitleColor(kGray+3) ;
			xAxis->SetLabelColor(kGray+3) ;
			yAxis->SetLabelColor(kGray+3) ;

			leg->Draw() ;

			TText* text = new TText(0.1 , 0.96 , dir->GetName() ) ;
			text->SetNDC() ;
			text->SetTextAlign(11) ;
			text->SetTextColor(kGray+2) ;
			text->SetTextFont(63) ;
			text->SetTextSize(30) ;
			text->Draw() ;

			std::stringstream eT ; eT << i << " GeV" ;
			TText* text2 = new TText(0.95 , 0.96 , eT.str().c_str() ) ;
			text2->SetNDC() ;
			text2->SetTextAlign(31) ;
			text2->SetTextColor(kGray+2) ;
			text2->SetTextFont(63) ;
			text2->SetTextSize(30) ;
			text2->Draw() ;

			c->RedrawAxis() ;
			c->Update() ;

			dir->cd() ;
			c->Write() ;
		}
	}

	outputFile->cd() ;

	TCanvas* cBeam = new TCanvas("cBeam" , "cBeam" , 900 , 900) ;

	cBeam->SetTopMargin(0.05f) ;
	cBeam->SetRightMargin(0.05f) ;
	cBeam->SetFrameLineColor(kGray+3) ;
	cBeam->SetTicks() ;
	cBeam->cd() ;

	//	TH2D* histo = new TH2D( "beamRange" , ";y (mm);x (mm)" , 1 , 0 , 1000 , 1 , 0 , 1000 ) ;
	//	histo->Draw() ;

	histoBeamVec.at(0)->DrawNormalized("box") ;
	histoBeamVec.at(1)->DrawNormalized("box same") ;
	histoBeamVec.at(2)->DrawNormalized("box same") ;
	//	for ( const auto& h : histoBeamVec )
	//	{
	//		h->DrawNormalized("lego same") ;
	//	}

	cBeam->Write() ;

	outputFile->Close() ;

	return 0 ;
}

