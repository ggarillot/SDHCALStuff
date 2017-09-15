#include <iostream>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>

#include <TH1.h>
#include <THStack.h>
#include <TCanvas.h>

#include <TimeCorrection.h>
#include <Event.h>

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

struct NormalOrShift
{
		std::map<int,int> runMap = {{}} ;
		std::map<int,TH1*> histoMap = {{}} ;

		std::string name = "Normal" ;

		Color_t col = kRed ;

		bool (*cut)(const Event& event) = &cutNormal ;
} ;

int main()
{
	std::string dir = "/home/garillot/files/Analysis/DATA/" ;

	NormalOrShift normal ;
	normal.name = "Normal" ;
	normal.cut = &cutNormal ;
	normal.col = kRed ;
		normal.runMap.insert( {80,736543} ) ;
//	normal.runMap.insert( {80,736532} ) ;
	normal.histoMap.insert( {80 , nullptr} ) ;

	NormalOrShift shift ;
	shift.name = "Shift" ;
	shift.cut = &cutShift ;
	shift.col = kBlue+2 ;
		shift.runMap.insert( {80,736535} ) ;
//	shift.runMap.insert( {80,736536} ) ;
	shift.histoMap.insert( {80 , nullptr} ) ;


	EventReader eventReader ;

	std::vector<NormalOrShift> posVec = {normal,shift} ;

	//	for ( const int i : {40,50,60,80} )
	for ( const int i : {80} )
	{
		std::stringstream tutu ; tutu << "c" << i << "GeV" ;
		TCanvas* c = new TCanvas( tutu.str().c_str() , tutu.str().c_str() , 900 , 900) ;

		THStack* stack = new THStack(tutu.str().c_str() , ";Nhit;") ;

		for ( auto pos = posVec.begin() ; pos < posVec.end() ; ++pos )
		{
			std::stringstream toto ; toto << dir << pos->runMap.at(i) << ".root" ;

			std::cout << "Open " << toto.str().c_str() << std::endl ;
			TFile* file = new TFile( toto.str().c_str() , "READ") ;
			TTree* tree = dynamic_cast<TTree*>( file->Get("tree") ) ;

			eventReader.setTree(tree) ;

			std::vector<Event> temp ;

			for ( int iEntry = 0 ; iEntry < tree->GetEntries() ; ++iEntry )
			{
				Event event = eventReader.getEvent(iEntry) ;

				if (!pos->cut(event) )
					continue ;

				event.energy = i ;

				temp.push_back(event) ;
			}

			file->Close() ;

			TimeCorrection timeCorr ;
			timeCorr.setBeginTime(5e6) ;

			timeCorr.correct(temp) ;

			HistoCreator histCreator ;
			histCreator.setDataStyle(false) ;
			histCreator.setColor(pos->col) ;
			histCreator.setEventList(temp) ;

			std::stringstream titi ; titi << pos->name << i << "GeV" ;

			TH1* histo = histCreator.getNHit( titi.str() ) ;
			histo->SetDirectory(0) ;
			histo->Scale( 1.0/histo->Integral() ) ;
			pos->histoMap.at(i) = histo ;

			stack->Add(histo) ;
		}

		c->cd() ;
		stack->Draw("nostack") ;

		c->SaveAs("toto.root") ;
	}

	return 0 ;
}

