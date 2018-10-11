#ifndef DataMcCanvas_h
#define DataMcCanvas_h

#include <TCanvas.h>
#include <TPad.h>
#include <TH2.h>

#include <TStyle.h>

class DataMcCanvas : public TCanvas
{
	public :
		DataMcCanvas(std::string name , std::pair<double, double> xAxis , std::pair<double, double> yAxis , std::pair<double, double> devAxis , std::string xName , std::string yName) ;
		DataMcCanvas(std::string name , std::string xName = "" , std::string yName = "") ;

		TPad* topPad() { return _topPad ; }
		TPad* bottomPad() { return _bottomPad ; }


		DataMcCanvas(const DataMcCanvas &toCopy) = delete ;
		void operator=(const DataMcCanvas &toCopy) = delete ;

	protected :
		TPad* _topPad = nullptr ;
		TPad* _bottomPad = nullptr ;
		TH2* _topRange = nullptr ;
		TH2* _bottomRange = nullptr ;

		std::string _name = "" ;

		std::string _xAxisName = "" ;
		std::string _yAxisName = "" ;

} ;


#endif //DataMcCanvas_h
