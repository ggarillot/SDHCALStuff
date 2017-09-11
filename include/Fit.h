#ifndef Fit_h
#define Fit_h

#include <TROOT.h>
#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TGraphErrors.h>

#include <RooConstVar.h>
#include <RooRealVar.h>
#include <RooDataSet.h>
#include <RooDataHist.h>
#include <RooGaussian.h>
#include <RooCBShape.h>
#include <RooPlot.h>

#include <vector>
#include <map>

typedef struct fitResults fitResults ;
struct fitResults
{
	double mean ;
	double resol ;
	double meanErr ;
	double resolErr ;
} ;


class Fit
{
	public :
		Fit() ;
		~Fit() ;

		void readFile(const char* file) ;
		void loadHistos(std::map<float,TH1*> _histoMap) { histoMap = _histoMap ; }

		void fitHisto(float energy) ;
		void fitAllHistos() ;

		void drawPlot(float energy) ;

		TGraphErrors* getLin(bool restrain = false) const ;
		void drawLin() ;

		TGraphErrors* getLinDev(bool restrain = false) const ;
		void drawLinDev() ;

		TGraphErrors* getResol(bool restrain = false) const ;
		void drawResol() ;


		Fit(const Fit &toCopy) = delete ;
		void operator=(const Fit &toCopy) = delete ;


	protected :

		TFile* histoFile = nullptr ;

		std::map<float,TH1*> histoMap ;
		std::map<float,fitResults> fitMap ;
		std::map<float,RooPlot*> plotMap ;
		std::map<float,TCanvas*> canvasMap ;

} ;

#endif //Fit_h
