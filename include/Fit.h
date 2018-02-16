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
	double mean = 0 ;
	double resol = 0 ;
	double meanErr = 0 ;
	double resolErr = 0 ;
	double sigma = 0 ;
	double alpha = 0 ;
	double n = 0 ;
} ;


class Fit
{
	public :
		Fit() ;
		~Fit() ;

		void readFile(const char* file) ;
		void loadHistos(const std::map<float,std::shared_ptr<TH1>>& _histoMap) ;

		void fitAllHistos() ;

		TGraphErrors* getLin(bool restrain = false) const ;
		void drawLin() ;

		TGraphErrors* getLinDev(bool restrain = false) const ;
		void drawLinDev() ;

		TGraphErrors* getResol(bool restrain = false) const ;
		void drawResol() ;

		void drawPlot(float energy) ;

		Fit(const Fit &toCopy) = delete ;
		Fit(Fit&& toCopy) = default ;
		void operator=(const Fit &toCopy) = delete ;


	protected :
		void clear() ;
		void fitHisto(float energy) ;

		std::map<float,std::shared_ptr<TH1>> histoMap = {} ;
		std::map<float,fitResults> fitMap = {} ;

} ;

#endif //Fit_h
