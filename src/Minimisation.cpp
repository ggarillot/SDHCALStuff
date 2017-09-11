#include "Minimisation.h"

#include <iostream>
#include <sstream>
#include <cmath>

#include <Minuit2/Minuit2Minimizer.h>
#include <Math/GSLMinimizer.h>
#include <Math/GSLSimAnMinimizer.h>
#include <Math/Functor.h>

using namespace std ;

Minimisation::Minimisation(unsigned int nParam_)
	: nParam(nParam_) ,
	  bestParam( std::vector<double>(nParam , 0) )
{

}

Minimisation::~Minimisation()
{

}

double Minimisation::eval(const std::vector<double>& param)
{
	return functionToMinimize( &param[0] ) ;
}


double Minimisation::minimize()
{
	cout << "Launching minimizing" << endl ;
	double prevMin = eval(bestParam) ;
	cout << "prevMin : " << prevMin << endl ;

//	ROOT::Math::GSLSimAnMinimizer min ;
	// ROOT::Math::GSLMinimizer min ;
//	ROOT::Minuit2::Minuit2Minimizer min ( ROOT::Minuit2::kCombined ) ;
	ROOT::Minuit2::Minuit2Minimizer min ;
	min.SetMaxFunctionCalls(1000000) ;
	min.SetMaxIterations(2) ;
	min.SetTolerance(1e-6) ;
	min.SetPrintLevel(2) ;

	ROOT::Math::Functor f(this , &Minimisation::functionToMinimize , nParam) ;

	min.SetFunction(f) ;

	for( unsigned int i = 0 ; i < nParam ; i++ )
	{
		stringstream toto ; toto << "param" << i ;
		min.SetVariable(i , toto.str().c_str() , bestParam[i] , step) ;
	}

//	min.SetLimitedVariable(1,"y",variable[1], step[1],-100,100) ;
	min.Minimize() ;

	const double* xs = min.X() ;
	double newMin = functionToMinimize(xs) ;

	if (newMin < prevMin)
	{
		for(unsigned int i = 0 ; i < nParam ; i++)
			bestParam[i] = xs[i] ;
	}

	return eval(bestParam) ;
}

void Minimisation::printParam() const
{
	for (unsigned int i = 0 ; i < nParam ; i++ )
		cout << bestParam[i] << endl ;
}

Test::Test()
	: Minimisation(2)
{
}

double Test::functionToMinimize(const double* param)
{
	double x = param[0] ;
	double y = param[1] ;
	double pi = M_PI ;

	return -cos(x)*cos(y)*exp( -( (x-pi)*(x-pi) + (y-pi)*(y-pi) ) ) ;
}
