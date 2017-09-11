#ifndef Minimisation_h
#define Minimisation_h

#include <vector>

class Minimisation
{
	public :
		Minimisation(unsigned int nParam_) ;
		virtual ~Minimisation() ;

		virtual double functionToMinimize(const double* param) = 0 ;
		double eval(const std::vector<double>& param) ;
		virtual double minimize() ;

		virtual void printParam() const ;


	protected :
		unsigned int nParam ;
		std::vector<double> bestParam ;
		double step = 0.1 ;

} ;

class Test : public Minimisation
{
	public :
		Test() ;

		double functionToMinimize(const double* param) ;

	protected :


} ;

#endif // Minimisation_h
