#include <iostream>

#include "EnergyMinimisation.h"
#include "Event.h"

#include <TFile.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TLegend.h>
#include <TLine.h>

#include <sstream>
#include <string>

int main()
{
	std::string dir = "/home/garillot/files/Analysis/DATA" ;

	std::vector<std::string> aList ;
	std::vector<std::string> bList ;

	//pions SPS Oct2016
	// 1 pC 2thr
	aList.push_back( "733660" ) ;
	aList.push_back( "733665" ) ;
	aList.push_back( "733683" ) ;
	aList.push_back( "733686" ) ;
	aList.push_back( "733689" ) ;
	aList.push_back( "733693" ) ;
	aList.push_back( "733696" ) ;

	// 5 pC 2thr
	bList.push_back( "733756" ) ;
	bList.push_back( "733750" ) ;
	bList.push_back( "733724" ) ;
	bList.push_back( "733728" ) ;
	bList.push_back( "733742" ) ;
	bList.push_back( "733743" ) ;
	bList.push_back( "733754" ) ;


	//	std::string aStr("1pC") ;
	//	std::string bStr("5pC") ;



	//pions SPS Oct2015
	// 163
	//	aList.push_back( "730716" ) ;
	//	aList.push_back( "730656" ) ;
	//	aList.push_back( "730634" ) ;
	//	aList.push_back( "730657" ) ;
	//	aList.push_back( "730651" ) ;
	//	aList.push_back( "730655" ) ;
	//	aList.push_back( "730659" ) ;
	//	aList.push_back( "730677" ) ;

	//	// 214
	//	bList.push_back( "730903" ) ;
	//	bList.push_back( "730888" ) ;
	//	bList.push_back( "730886" ) ;
	//	bList.push_back( "730882" ) ;
	//	bList.push_back( "730861" ) ;
	//	bList.push_back( "730858" ) ;
	//	bList.push_back( "730851" ) ;
	//	bList.push_back( "730847" ) ;

	//	std::string aStr("163") ;
	//	std::string bStr("214") ;





	std::string aStr("QuadSim") ;
	//	std::string bStr("QuadSimCheat") ;

	QuadMinimisation a(aStr) ;
	//	QuadMinimisation b(bStr) ;

	std::string bStr("LinearSim") ;
	//		std::string bStr("LinearSimCheat") ;
	//	LinearMinimisation a(aStr) ;
	LinearMinimisation b(bStr) ;



	//	BinaryMinimisation a(aStr) ;
	//	BinaryMinimisation b(bStr) ;


	//	for ( std::vector<std::string>::const_iterator it = aList.begin() ; it != aList.end() ; ++it )
	//	{
	//		std::stringstream toto ;
	//		toto << dir << "/" << *it << ".root" ;
	//		a.loadFile(toto.str() , 5e6) ;
	//	}

	//	for ( std::vector<std::string>::const_iterator it = bList.begin() ; it != bList.end() ; ++it )
	//	{
	//		std::stringstream toto ;
	//		toto << dir << "/" << *it << ".root" ;
	//		b.loadFile(toto.str() , 5e6) ;
	//	}

	//	std::string simDir = "/home/garillot/files/Analysis/Centered/OldDigit/Geant4.9.6/FTF_BIC/" ;
	std::string simDir = "/home/garillot/files/Analysis/Centered/OldDigit/Geant4.9.6/QGSP_BERT_HP/" ;

	//very temporary
	std::string simILDDir = "/home/garillot/files/Analysis/ILD/" ;

	//	//simulation
	//	for ( int i = 10 ; i < 81 ; i += 10 )
	//	{
	//		std::stringstream toto ;
	//		toto << simDir << "single_pi-_" << i << "GeV.root" ;
	//		a.loadFile(toto.str()) ;
	//	}

	//	//simulation cheat
	//	for ( int i = 10 ; i < 81 ; i += 10 )
	//	{
	//		std::stringstream toto ;
	//		toto << simDir << "single_pi-_" << i << "GeV.root" ;
	//		b.loadFile(toto.str()) ;
	//	}

	//simulation ILD
	for ( int i = 10 ; i < 91 ; i += 10 )
	{
		std::stringstream toto ;
		toto << simILDDir << i << "GeV.root" ;
		a.loadFile(toto.str()) ;
	}

	//simulation ILD cheat
	for ( int i = 10 ; i < 91 ; i += 10 )
	{
		std::stringstream toto ;
		toto << simILDDir << i << "GeV.root" ;
		b.loadFile(toto.str()) ;
	}


	//	for ( std::vector<std::string>::const_iterator it = bList.begin() ; it != bList.end() ; ++it )
	//	{
	//		std::stringstream toto ;
	//		toto << dir << "/" << *it << ".root" ;
	//		b.loadFile(toto.str() , 5e6) ;
	//	}


	a.minimize() ;
	a.printParam() ;

		a.fitForCheat() ;
		a.minimize() ;
		a.printParam() ;

	a.createHistos() ;
	a.writeHistos() ;


	b.minimize() ;
	b.printParam() ;

		b.fitForCheat() ;
		b.minimize() ;
		b.printParam() ;

	b.createHistos() ;
	b.writeHistos() ;



	/*

	// QuadMinimisation a ;
	QuadMinimisationData a ;
	a.loadEvents(163) ;
	a.minimize() ;
	a.printParam() ;

	//		a.fitForCheat() ;
	//		a.minimize() ;
	//		a.printParam() ;

	a.createHistos() ;
	a.writeHistos() ;
	//	a.drawResults() ;

	QuadMinimisationData b ;
	b.loadEvents(214) ;
	b.minimize() ;
	b.printParam() ;

	//		a.fitForCheat() ;
	//		a.minimize() ;
	//		a.printParam() ;

	b.createHistos() ;
	b.writeHistos() ;
	//	a.drawResults() ;

	*/

	Fit& fita = a.getFit() ;
	Fit& fitb = b.getFit() ;

	fita.fitAllHistos() ;
	fitb.fitAllHistos() ;

	TFile* file = new TFile("eRecoILD.root","RECREATE") ;


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

	TGraphErrors* ga = fita.getLin() ;
	TGraphErrors* gb = fitb.getLin() ;

	ga->SetMarkerColor(kBlue-4) ;
	gb->SetMarkerColor(kRed-4) ;

	ga->SetMarkerSize(1.4f) ;
	gb->SetMarkerSize(1.4f) ;

	ga->Draw("P same") ;
	gb->Draw("P same") ;


	TLegend* linleg = new TLegend(0.18,0.75,0.6,0.88) ;
	linleg->SetBorderSize(0) ;

	linleg->AddEntry(ga , aStr.c_str() ,"p") ;
	linleg->AddEntry(gb , bStr.c_str() ,"p") ;
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

	TGraphErrors* gaDev = fita.getLinDev() ;
	TGraphErrors* gbDev = fitb.getLinDev() ;

	gaDev->SetMarkerColor(kBlue-4) ;
	gbDev->SetMarkerColor(kRed-4) ;

	gaDev->SetMarkerSize(1.4f) ;
	gbDev->SetMarkerSize(1.4f) ;


	gaDev->Draw("PZ same") ;
	gbDev->Draw("PZ same") ;

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



	//	linC1->SetTopMargin(0.05f) ;
	//	linC1->SetBottomMargin(0.02f) ;
	//	linC2->SetTopMargin(0) ;
	//	linC2->SetBottomMargin(0.15f) ;
	//	linC1->SetLeftMargin(0.15f) ;
	//	linC2->SetLeftMargin(0.15f) ;
	//	linC1->SetRightMargin(0.02f) ;
	//	linC2->SetRightMargin(0.02f) ;




	TGraphErrors* gaRes = fita.getResol() ;
	TGraphErrors* gbRes = fitb.getResol() ;


	gaRes->SetMarkerColor(kBlue-4) ;
	gbRes->SetMarkerColor(kRed-4) ;

	gaRes->SetMarkerSize(1.4f) ;
	gbRes->SetMarkerSize(1.4f) ;


	TH2D* range = new TH2D("range", ";Energy(GeV);resolution" , 1 , 0 , 85 , 1 , 0, 0.3) ;
	range->GetYaxis()->SetTitleOffset(1.5f) ;
	range->Draw() ;

	gaRes->Draw("P same") ;
	gbRes->Draw("P same") ;

	TLegend* leg = new TLegend(0.6,0.7,0.85,0.85) ;
	leg->SetLineColor(kWhite) ;
	leg->AddEntry(gaRes , aStr.c_str() , "p") ;
	leg->AddEntry(gbRes , bStr.c_str() , "p") ;

	leg->Draw() ;

	resC->Write() ;
	resC->Close() ;

	file->Close() ;

	return 0 ;
}
