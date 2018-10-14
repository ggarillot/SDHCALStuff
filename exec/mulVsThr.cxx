#include <iostream>
#include <cassert>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TGraphErrors.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLegendEntry.h>

#include <vector>
#include <sstream>

#include "DataMcCanvas.h"

int main(int , char**)
{
	std::vector<double>* thresholds = nullptr ; //= reinterpret_cast< std::vector<double>* >( file->Get("Thresholds") ) ;
	std::vector<double>* multiplicities = nullptr ;
	std::vector<double>* multiplicitiesError = nullptr ;

	int layerID ;


	TFile* file = new TFile{"mulVsThr.root" , "RECREATE"} ;
	file->cd()  ;

	TGraphErrors* mulDataGraph = new TGraphErrors ;
	TGraphErrors* mulMCGraph = new TGraphErrors ;

	mulMCGraph->SetMarkerColor(kRed-4) ;
	mulMCGraph->SetLineColor(kRed-4) ;

	mulDataGraph->SetMarkerStyle(20) ;
	mulMCGraph->SetMarkerStyle(20) ;


	std::vector<int> runs = { 730630, 730627, 730626, 730625, 730619, 730618, 730617, 730616, 730615, 730611, 730609, 730607, 730569, 730568, 730567, 730566, 730631, 730633, 730545, 730677 } ;
	std::vector<double> thr1Vec = { 188,199,210,221,232,243,254,265,276,287,299,310,321,332,343,354,365,376,387,170 } ;
	std::vector<double> thr2Vec = { 130,147,164,181,197,214,231,248,265,282,298,315,332,349,366,383,399,416,433,498 } ;
	std::vector<double> thr3Vec = { 168,185,202,220,237,254,271,288,305,323,340,357,374,391,408,425,443,460,477,342 } ;

	int iEntry = 0 ;
	int iPoint = 0 ;

	for ( unsigned int i = 0 ; i < 20 ; i++ )
	{
		double thr1 = ( (thr1Vec.at(i)-90)/700.0 ) ;
		double thr2 = ( (thr2Vec.at(i)-98)/80.0 ) ;
		double thr3 = ( (thr3Vec.at(i)-98)/16.3 ) ;
		int run = runs.at(i) ;


		std::stringstream toto ; toto << "/home/guillaume/files/MultiplicityMap/DATA/thrScan/Eff_" << run << ".root" ;
		std::cout << "Process " << toto.str() << std::endl ;
		TFile* dataFile = new TFile(toto.str().c_str() , "READ") ;
		TTree* tree = dynamic_cast<TTree*>( dataFile->Get("tree") ) ;

		tree->SetBranchAddress("LayerID" , &layerID) ;
		tree->SetBranchAddress("Multiplicities" , &multiplicities) ;
		tree->SetBranchAddress("MultiplicitiesError" , &multiplicitiesError) ;


		std::vector<double> thrs = {thr1 , thr2 , thr3} ;
		if ( run == 730677 )
		{
			thrs.clear() ;
			thrs = {thr1} ;
		}

		iEntry = 0 ;

		while ( tree->GetEntry(iEntry++) )
		{
			if ( layerID != -1 )
				continue ;


			for (unsigned int j = 0 ; j < thrs.size() ; ++j)
			{
				mulDataGraph->SetPoint(iPoint , thrs.at(j) , multiplicities->at(j) ) ;
				mulDataGraph->SetPointError(iPoint++ , 0 , multiplicitiesError->at(j) ) ;
			}
			break ;
		}

		dataFile->Close() ;
	}


	TFile* mcFile = TFile::Open("/home/guillaume/files/MultiplicityMap/Centered/Geant4.10.04/QGSP_BERT/Eff_mu-_100GeV_Analog.root" , "READ") ;

	assert( mcFile ) ;
	TTree* tree = dynamic_cast<TTree*>( mcFile->Get("tree") ) ;

	tree->SetBranchAddress("LayerID" , &layerID) ;
	tree->SetBranchAddress("Multiplicities" , &multiplicities) ;
	tree->SetBranchAddress("MultiplicitiesError" , &multiplicitiesError) ;


	thresholds = reinterpret_cast< std::vector<double>* >( mcFile->Get("Thresholds") ) ;
	iEntry = 0 ;
	iPoint = 0 ;

	while ( tree->GetEntry(iEntry++) )
	{
		if ( layerID != -1 )
			continue ;

		for (unsigned int i = 0 ; i < thresholds->size() ; ++i)
		{
			mulMCGraph->SetPoint(i , thresholds->at(i) , multiplicities->at(i) ) ;
			mulMCGraph->SetPointError(i , 0 , multiplicitiesError->at(i) ) ;
		}
		break ;
	}

	mcFile->Close() ;

	assert( mulDataGraph->GetN() == mulMCGraph->GetN() ) ;


	file->cd()  ;

	DataMcCanvas* canv = new DataMcCanvas{ "test" , {0.1,30} , {1,2} , "Threshold(pC)" , "Multiplicity" } ;
	canv->topPad()->cd() ;
	canv->topPad()->SetLogx() ;

	mulDataGraph->Draw("P same") ;
	mulMCGraph->Draw("P same") ;


	TLegend* leg = new TLegend(0.2,0.2,0.4,0.3) ;
	leg->SetBorderSize(0) ;

	TLegendEntry* dataLeg = leg->AddEntry(mulDataGraph , "DATA" , "p") ;
	TLegendEntry* mcLeg = leg->AddEntry(mulMCGraph , "MC" , "p") ;
	dataLeg->SetTextFont(62) ;
	mcLeg->SetTextFont(62) ;
	mcLeg->SetTextColor(kRed-4) ;
	leg->Draw() ;

	canv->Write() ;
	file->Close() ;

	return 0 ;
}
