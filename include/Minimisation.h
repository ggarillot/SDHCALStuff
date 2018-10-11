#ifndef Minimisation_h
#define Minimisation_h

#include <vector>
#include <string>

class Minimisation
{
	public :
		Minimisation(unsigned int nParam_) ;
		virtual ~Minimisation() = default ;

		virtual double functionToMinimize(const double* param) = 0 ;
		double eval(const std::vector<double>& param) ;
		virtual double minimize() ;

		virtual void printParam() const ;

		enum Limits { kNoLimit , kNegative , kPositive } ;

		std::vector<double> getParams() const { return bestParam ; }

		void setParams(std::vector<double> values) ;


	protected :
		unsigned int nParam ;
		std::vector<double> bestParam = {} ;
		std::vector<Limits> limitsParam = {} ;
		std::vector<std::string> nameParam = {} ;
		double step = 0.2 ;

} ;

class Test : public Minimisation
{
	public :
		Test() ;

		double functionToMinimize(const double* param) ;

	protected :


} ;

#endif // Minimisation_h
