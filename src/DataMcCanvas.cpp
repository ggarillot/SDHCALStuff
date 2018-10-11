#include "DataMcCanvas.h"
#include "TLine.h"

DataMcCanvas::DataMcCanvas(std::string name , std::pair<double, double> xAxis, std::pair<double, double> yAxis, std::pair<double, double> devAxis, std::string xName, std::string yName)
	: TCanvas(name.c_str() , name.c_str() , 800 , 1000) ,
	  _name( name )
{
	gStyle->SetOptStat(0) ;
	cd() ;

	std::string topPadName = name + "top" ;
	std::string bottomPadName = name + "bottom" ;

	_topPad = new TPad(topPadName.c_str() , topPadName.c_str() , 0 , 0.25 , 1.0 , 1.0) ;
	_bottomPad = new TPad(bottomPadName.c_str() , bottomPadName.c_str() , 0 , 0 , 1.0 , 0.25) ;
	_topPad->SetTicks() ;
	_bottomPad->SetTicks() ;
	_topPad->Draw() ;
	_bottomPad->Draw() ;
	_topPad->SetTopMargin(0.05f) ;
	_topPad->SetBottomMargin(0.02f) ;
	_bottomPad->SetTopMargin(0) ;
	_bottomPad->SetBottomMargin(0.20f) ;
	_topPad->SetLeftMargin(0.108f) ;
	_bottomPad->SetLeftMargin(0.108f) ;
	_topPad->SetRightMargin(0.02f) ;
	_bottomPad->SetRightMargin(0.02f) ;

	yName = ";;" + yName ;
	_topPad->cd() ;
	_topRange = new TH2D(topPadName.c_str(), yName.c_str() , 1 , xAxis.first , xAxis.second , 1 , yAxis.first , yAxis.second) ;
	_topRange->GetXaxis()->SetLabelSize(0) ;
	_topRange->GetYaxis()->SetTitleOffset(1.55f) ;
	_topRange->Draw() ;

	xName = ";" + xName + ";MC/DATA" ;
	_bottomPad->cd() ;
	_bottomRange = new TH2D(bottomPadName.c_str() , xName.c_str() , 1 , xAxis.first , xAxis.second , 1 , devAxis.first*(1+1e-4) , devAxis.second*(1-1e-4)) ;
	_bottomRange->GetXaxis()->SetTickLength( _bottomRange->GetXaxis()->GetTickLength()*3.3f) ;
	_bottomRange->GetXaxis()->SetLabelSize( _bottomRange->GetXaxis()->GetLabelSize()*2.5f) ;
	_bottomRange->GetXaxis()->SetTitleSize( _bottomRange->GetXaxis()->GetTitleSize()*2.5f ) ;
	_bottomRange->GetXaxis()->SetTitleOffset(1.0f) ;
	_bottomRange->GetXaxis()->SetLabelOffset(0.01f) ;
	_bottomRange->GetYaxis()->SetLabelSize( _bottomRange->GetYaxis()->GetLabelSize()*2.5f) ;
	_bottomRange->GetYaxis()->SetNdivisions(5,5,0) ;
	_bottomRange->GetYaxis()->SetTitleSize( _bottomRange->GetYaxis()->GetTitleSize()*2.5f ) ;
	_bottomRange->GetYaxis()->SetTitleOffset(0.55f) ;
	_bottomRange->Draw() ;

	TLine* nHitDevLine = new TLine(xAxis.first , 1 , xAxis.second , 1) ;
	nHitDevLine->SetLineColor(kBlack) ;
	nHitDevLine->SetLineWidth(2) ;
	nHitDevLine->Draw() ;
}

DataMcCanvas::DataMcCanvas(std::string name , std::string xName , std::string yName)
	: TCanvas(name.c_str() , name.c_str() , 1000 , 1000) ,
	  _name( name )
{
	gStyle->SetOptStat(0) ;
	cd() ;

	std::string topPadName = name + "top" ;

	_topPad = new TPad(topPadName.c_str() , topPadName.c_str() , 0 , 0 , 1.0 , 1.0) ;
	_topPad->SetTicks() ;
	_topPad->Draw() ;
	_topPad->SetTopMargin(0.05f) ;
	_topPad->SetRightMargin(0.05f) ;

//	_topPad->SetTopMargin(0.044f) ;
//	_topPad->SetBottomMargin(0.084f) ;
//	_topPad->SetLeftMargin(0.084f) ;
//	_topPad->SetRightMargin(0.044f) ;
}
