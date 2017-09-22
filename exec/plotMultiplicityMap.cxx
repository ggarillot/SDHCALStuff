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

#include <tuple>
#include <map>
#include <sstream>
#include <string>

struct IJ
{
		IJ(int _i , int _j) : i(_i) , j(_j) {}
		int i = 0 ;
		int j = 0 ;

		bool operator<(const IJ& other) const
		{
			if (i != other.i)
				return i < other.i ;
			else
				return j < other.j ;
		}
} ;

int main(int argc , char** argv)
{
	std::string fileName = argv[1] ;

	TFile* inputFile = TFile::Open( fileName.c_str() ) ;

	if (!inputFile)
	{
		std::cerr << "Error : " << fileName << " not existing" << std::endl ;
		return 1 ;
	}

	TTree* tree = dynamic_cast<TTree*>( inputFile->Get("tree") ) ;

	int padID ;
	int asicID ;
	int difID ;
	int nTracks ;
	std::vector<double>* efficiencies = nullptr ;
	double multiplicity ;
	std::vector<double>* position = nullptr ;

	tree->SetBranchAddress("PadID" , &padID) ;
	tree->SetBranchAddress("DifID" , &difID) ;
	tree->SetBranchAddress("AsicID" , &asicID) ;
	tree->SetBranchAddress("Efficiencies" , &efficiencies) ;
	tree->SetBranchAddress("Multiplicity" , &multiplicity) ;
	tree->SetBranchAddress("Position" , &position) ;
	tree->SetBranchAddress("Ntrack" , &nTracks) ;


	std::map<IJ , int> globalNMap ;
	std::map<IJ , std::tuple<unsigned int,double>> globalMulMap ;
	std::map<IJ , std::tuple<unsigned int,double>> globalEff3Map ;

	std::map<std::pair<int,int> , std::tuple<unsigned int,double>> asicMulMap ;
	std::map<std::pair<int,int> , std::tuple<unsigned int,double>> asicEff3Map ;

	int iEntry = 0 ;
	while ( tree->GetEntry(iEntry++) )
	{
		if ( padID > -1 )
			continue ;
		if ( asicID == -1 )
			continue ;

		if ( efficiencies->at(0) < 0.05 )
			continue ;

		double d = difID ;
		double a = asicID ;

		std::get<0>( asicMulMap[ {d,a} ] ) ++ ;
		std::get<1>( asicMulMap[ {d,a} ] ) += multiplicity ;

		std::get<0>( asicEff3Map[ {d,a} ] ) ++ ;
		std::get<1>( asicEff3Map[ {d,a} ] ) += efficiencies->at(2) ;
	}

	iEntry = 0 ;
	while ( tree->GetEntry(iEntry++) )
	{
		if ( padID == -1 )
			continue ;

		int i = static_cast<int>( std::round(position->at(0)/10.408) ) ;
		int j = static_cast<int>( std::round(position->at(1)/10.408) ) ;

		globalNMap[IJ(i,j)] += nTracks ;

		//		int i = static_cast<int>( 1 + (position->at(0)-10.408)/(8*10.408) ) ;
		//		int j = static_cast<int>( 1 + (position->at(1)-10.408)/(8*10.408) ) ;

		if ( efficiencies->at(0) < 0.05 )
		{
			if ( asicMulMap.find({difID , asicID}) == asicMulMap.end() )
				continue ;

			std::get<0>( globalMulMap[IJ(i,j)] ) ++ ;
			std::get<1>( globalMulMap[IJ(i,j)] ) += std::get<1>( asicMulMap.at( {difID , asicID} ) ) ;

			std::get<0>( globalEff3Map[IJ(i,j)] ) ++ ;
			std::get<1>( globalEff3Map[IJ(i,j)] ) += std::get<1>( asicEff3Map.at( {difID , asicID} ) ) ;
			continue ;

		}

		//		if ( efficiencies->at(0) < 0.05 )
		//			continue ;

		std::get<0>( globalMulMap[IJ(i,j)] ) ++ ;
		std::get<0>( globalEff3Map[IJ(i,j)] ) ++ ;
		std::get<1>( globalMulMap[IJ(i,j)] ) += multiplicity ;
		std::get<1>( globalEff3Map[IJ(i,j)] ) += efficiencies->at(2) ;
	}

	TH2D* histoMul = new TH2D("histoMul" , ";I;J" , 96 , 1 , 97 , 96 , 1 , 97) ;
	TH2D* histoEff3 = new TH2D("histoEff3" , ";I;J" , 96 , 1 , 97 , 96 , 1 , 97) ;
	TH2D* histoN = new TH2D("histoN" , ";I;J" , 100 , 0 , 100 , 100 , 0 , 100) ;

	//	TH2D* histoMul = new TH2D("histoMul" , ";I;J" , 12 , 1 , 13 , 12 , 1 , 13) ;
	//	TH2D* histoEff3 = new TH2D("histoMul" , ";I;J" , 12 , 1 , 13 , 12 , 1 , 13) ;

	histoMul->SetMaximum(2.65) ;
	histoMul->SetMinimum(1.25) ;

	histoEff3->SetMaximum(0.14) ;
	histoEff3->SetMinimum(0) ;

	//	TH2D* histoN = new TH2D("histoN" , ";I;J" , 96 , 1 , 97 , 96 , 1 , 97) ;

	for (const auto& it : globalMulMap)
	{
		unsigned int n ;
		double m ;
		std::tie(n ,  m) = it.second ;
		histoMul->Fill( it.first.i , it.first.j , m/n) ;
	}

	for (const auto& it : globalEff3Map)
	{
		unsigned int n ;
		double m ;
		std::tie(n ,  m) = it.second ;
		histoEff3->Fill( it.first.i , it.first.j , m/n) ;
	}

	for (const auto& it : globalNMap)
		histoN->Fill( it.first.i , it.first.j , it.second) ;


	histoMul->Smooth() ;
	histoEff3->Smooth() ;

//	std::cout << histoMul->GetMinimum() << std::endl ;
//	std::cout << histoMul->GetMaximum() << std::endl ;

	gStyle->SetOptStat(0) ;
	TCanvas* cMul = new TCanvas("cMul" , "cMul" , 900 , 900) ;
	cMul->SetLeftMargin(0.08f) ;
	cMul->SetRightMargin(0.12f) ;
	cMul->cd() ;
	histoMul->SetContour(99) ;
	histoMul->Draw("colz") ;
	cMul->SaveAs("mul.png") ;

	TCanvas* cEff3 = new TCanvas("cEff3" , "cEff3" , 900 , 900) ;
	cEff3->SetLeftMargin(0.08f) ;
	cEff3->SetRightMargin(0.12f) ;
	cEff3->cd() ;
	histoEff3->SetContour(99) ;
	histoEff3->Draw("colz") ;
	cEff3->SaveAs("eff3.png") ;

	TCanvas* cN = new TCanvas("cN" , "cN" , 900 , 900) ;
	cN->SetLeftMargin(0.08f) ;
	cN->SetRightMargin(0.12f) ;
	cN->cd() ;
	histoN->SetContour(99) ;
	histoN->Draw("colz") ;
	cN->SaveAs("n.png") ;

	inputFile->Close() ;

	return 0 ;
}
