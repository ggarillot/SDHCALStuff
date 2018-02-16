#include <iostream>

#include "json.hpp"

#include "EnergyMinimisation.h"
#include "Event.h"

#include <TFile.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TLegend.h>
#include <TLine.h>

#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cassert>

int main(int argc , char** argv)
{
	if ( argc < 1 )
	{
		std::cerr << "ERROR : Please provide a json configuration file" << std::endl ;
		return 1 ;
	}

	std::vector<EnergyMinimisation*> minimizersVec {} ;
	std::vector<bool> cheatVec {} ;
	std::vector<Color_t> colorVec {kRed-4 , kBlue-4 , kGreen+2 , kCyan+2} ;

	std::string jsonFileName = argv[1] ;


	std::ifstream jsonFile(jsonFileName) ;
	auto json = nlohmann::json::parse(jsonFile) ;

	std::string graphName = json.at("graphFileName") ;
	auto minimizersList = json.at("minimizers") ;

	for ( const auto& i : minimizersList )
	{
		EnergyMinimisation* minimizer = nullptr ;

		std::string method = i.at("method") ;
		std::string name = i.at("name") ;

		if ( method == "Linear" )
			minimizer = new LinearMinimisation(name) ;
		else if ( method == "Quadratic" )
			minimizer = new QuadMinimisation(name) ;
		else if ( method == "LinearDensity" )
			minimizer = new LinearDensityMinimisation(name) ;

		if ( i.count("geomCut") )
		{
			auto geomCut = i.at("geomCut") ;
			minimizer->setGeomCut({{ geomCut.at("x") , geomCut.at("y") , geomCut.at("radius") }}) ;
		}

		auto files = i.at("files") ;

		std::string dir = "" ;
		if ( files.count("dir") )
			dir = files.at("dir") ;

		auto files2 = files.at("files") ;

		for ( const auto& file : files2 )
		{
			std::string fileName = file.at("file") ;
			if ( file.count("spillBegin") )
				minimizer->loadFile( dir + "/" + fileName , file.at("spillBegin") , file.at("spillEnd") ) ;
			else
				minimizer->loadFile( dir + "/" + fileName ) ;
		}

		if ( i.count("parameters") )
		{
			auto paramVec = i.at("parameters") ;

			assert( paramVec.size() == minimizer->getParams().size() ) ;
			minimizer->setParams(paramVec) ;
		}

		minimizersVec.push_back( minimizer ) ;

		if ( i.count("cheat") )
		{
			if (i.at("cheat") == "true")
				cheatVec.push_back(true) ;
			else
				cheatVec.push_back(false) ;
		}
		else
			cheatVec.push_back(false) ;
	}

	jsonFile.close() ;

	assert( minimizersVec.size() == cheatVec.size() ) ;

	for ( unsigned int i = 0 ; i < minimizersVec.size() ; ++i )
	{
		minimizersVec.at(i)->minimize() ;

		if ( cheatVec.at(i) == true )
		{
			minimizersVec.at(i)->createHistos() ;
			minimizersVec.at(i)->fitForCheat() ;
			minimizersVec.at(i)->minimize() ;
		}

		minimizersVec.at(i)->createHistos() ;
		minimizersVec.at(i)->writeHistos() ;
	}

	std::vector<Fit> fitVec {} ;
	for ( unsigned int i = 0 ; i < minimizersVec.size() ; ++i )
	{
		fitVec.push_back( Fit() ) ;
		fitVec.back().loadHistos(minimizersVec.at(i)->getHistos()) ;
		fitVec.back().fitAllHistos() ;
	}

	TFile* file = new TFile(graphName.c_str() , "RECREATE") ;


	TCanvas* linC = new TCanvas("linCanv" , "linCanv"  , 700 , 900) ;
	linC->cd() ;

	gStyle->SetOptStat(0) ;

	TPad* linC1 = new TPad("linC1","linC1" , 0 , 0.3 , 1.0 , 1.0) ;
	TPad* linC2 = new TPad("linC2","linC2" , 0 , 0 , 1.0 , 0.3) ;
	linC1->SetTicks() ;
	linC2->SetTicks() ;
	linC1->Draw() ;
	linC2->Draw() ;
	linC1->SetTopMargin(0.05f) ;
	linC1->SetBottomMargin(0.02f) ;
	linC2->SetTopMargin(0) ;
	linC2->SetBottomMargin(0.15f) ;
	linC1->SetLeftMargin(0.15f) ;
	linC2->SetLeftMargin(0.15f) ;
	linC1->SetRightMargin(0.02f) ;
	linC2->SetRightMargin(0.02f) ;

	linC1->cd() ;
	TH2D* linRange = new TH2D("lin" , ";Energy (GeV);<Ereco>" , 1 , 0 , 85 , 1 , 0 , 85) ;
	linRange->GetXaxis()->SetLabelSize(0) ;

	linRange->GetYaxis()->SetTitleOffset(1.8f) ;
	linRange->Draw() ;

	std::vector<TGraphErrors*> linGraphVec {} ;

	for ( unsigned int i = 0 ; i < fitVec.size() ; ++i )
		linGraphVec.push_back( fitVec.at(i).getLin() ) ;

	for ( unsigned int i = 0 ; i < linGraphVec.size() ; ++i )
	{
		linGraphVec.at(i)->SetMarkerColor( colorVec.at(i) ) ;
		linGraphVec.at(i)->SetMarkerSize(1.4f) ;
		linGraphVec.at(i)->Draw("P same") ;
	}



	TLegend* linleg = new TLegend(0.18,0.75,0.6,0.88) ;
	linleg->SetBorderSize(0) ;

	for ( unsigned int i = 0 ; i < linGraphVec.size() ; ++i )
		linleg->AddEntry(linGraphVec.at(i) , minimizersVec.at(i)->getName().c_str() ,"p") ;

	linleg->Draw() ;


	linC2->cd() ;
	TH2D* linDevRange = new TH2D("linDev" , ";Energy (GeV);(E_{reco}-E_{beam})/E_{beam}   " , 1 , 0 , 85 , 1 , -0.149 , 0.149) ;
	linDevRange->GetXaxis()->SetTickLength(0.075f) ;
	linDevRange->GetXaxis()->SetLabelSize(0.075f) ;
	linDevRange->GetXaxis()->SetTitleSize(0.075f) ;
	linDevRange->GetXaxis()->SetTitleOffset(0.9f) ;
	linDevRange->GetYaxis()->SetLabelSize(0.075f) ;
	linDevRange->GetYaxis()->SetNdivisions(5,5,0) ;
	linDevRange->GetYaxis()->SetTitleSize(0.075f) ;
	linDevRange->GetYaxis()->SetTitleOffset(0.8f) ;
	linDevRange->Draw() ;

	TLine* linDevLine = new TLine(0,0,85,0) ;
	linDevLine->SetLineColor(kBlack) ;
	linDevLine->SetLineWidth(2) ;
	linDevLine->Draw() ;

	TLine* linDevUp = new TLine(0,0.05,85,0.05) ;
	linDevUp->SetLineColor(kGray) ;
	linDevUp->SetLineWidth(2) ;
	linDevUp->Draw() ;

	TLine* linDevDown = new TLine(0,-0.05,85,-0.05) ;
	linDevDown->SetLineColor(kGray) ;
	linDevDown->SetLineWidth(2) ;
	linDevDown->Draw() ;

	std::vector<TGraphErrors*> linDevGraphVec {} ;

	for ( unsigned int i = 0 ; i < fitVec.size() ; ++i )
		linDevGraphVec.push_back( fitVec.at(i).getLinDev() ) ;

	for ( unsigned int i = 0 ; i < linDevGraphVec.size() ; ++i )
	{
		linDevGraphVec.at(i)->SetMarkerColor( colorVec.at(i) ) ;
		linDevGraphVec.at(i)->SetMarkerSize(1.4f) ;
		linDevGraphVec.at(i)->Draw("PZ same") ;
	}

	linC->Write() ;
	linC->Close() ;


	TCanvas* resC = new TCanvas("resC" , "resC" , 800 , 800) ;
	resC->cd() ;

	gStyle->SetOptStat(0) ;

	TPad* resC1 = new TPad("resC1","resC1" , 0 , 0 , 1.0 , 1.0) ;

	resC1->SetTicks() ;
	resC1->Draw() ;
	resC1->SetTopMargin(0.05f) ;
	resC1->SetBottomMargin(0.09f) ;
	resC1->SetLeftMargin(0.12f) ;
	resC1->SetRightMargin(0.02f) ;
	resC1->cd() ;


	TH2D* range = new TH2D("range", ";Energy(GeV);resolution" , 1 , 0 , 85 , 1 , 0, 0.3) ;
	range->GetYaxis()->SetTitleOffset(1.5f) ;
	range->Draw() ;

	std::vector<TGraphErrors*> resolGraphVec {} ;

	for ( unsigned int i = 0 ; i < fitVec.size() ; ++i )
		resolGraphVec.push_back( fitVec.at(i).getResol() ) ;


	for ( unsigned int i = 0 ; i < resolGraphVec.size() ; ++i )
	{
		resolGraphVec.at(i)->SetMarkerColor( colorVec.at(i) ) ;
		resolGraphVec.at(i)->SetMarkerSize(1.4f) ;
		resolGraphVec.at(i)->Draw("P same") ;
	}


	TLegend* leg = new TLegend(0.6,0.7,0.85,0.85) ;
	leg->SetLineColor(kWhite) ;

	for ( unsigned int i = 0 ; i < resolGraphVec.size() ; ++i )
		leg->AddEntry(resolGraphVec.at(i) , minimizersVec.at(i)->getName().c_str() ,"p") ;

	leg->Draw() ;

	resC->Write() ;
	resC->Close() ;

	file->Close() ;

	std::cout << "delete minimizers" << std::endl ;
	for ( auto minimizer : minimizersVec )
		delete minimizer ;

	std::cout << "after delete minimizers" << std::endl ;

	return 0 ;
}
