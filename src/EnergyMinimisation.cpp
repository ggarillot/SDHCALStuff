#include "EnergyMinimisation.h"
#include "TimeCorrection.h"

#include <iostream>
#include <sstream>
#include <set>
#include <numeric>
#include <limits>
#include <array>
#include <algorithm>

#include <TH1.h>
#include <TGraphErrors.h>
#include <TF1.h>

EnergyMinimisation::EnergyMinimisation(unsigned int nParam_ , std::string name_)
	: Minimisation(nParam_) ,
	  name(name_)
{
	std::cout << "Initialize " << name << " energy minimizer" << std::endl ;
}

EnergyMinimisation::~EnergyMinimisation()
{
}


void EnergyMinimisation::loadEvents( const std::vector<EventList>& eventListVec )
{
	events.clear() ;
	eventsForMinim.clear() ;

	for ( const auto& eventList : eventListVec )
		events.insert( events.end() , eventList.events().begin() , eventList.events().end() ) ;

	std::random_shuffle( events.begin() , events.end() ) ;

	for ( const auto& event : events )
	{
		if ( !nEventsPerEnergyMap.count(event.energy) )
			nEventsPerEnergyMap[ event.energy ] = 0 ;

		if ( nEventsPerEnergyMap[ event.energy ] < nEventsPerEnergy )
		{
			nEventsPerEnergyMap[ event.energy ]++ ;
			eventsForMinim.push_back( event ) ;
		}
	}
}


double EnergyMinimisation::functionToMinimize(const double* param)
{
	double chi2 = 0 ;

	for( std::vector<Event>::const_iterator it = eventsForMinim.begin() ; it != eventsForMinim.end() ; ++it )
	{
		const Event& event = *it ;
		double estimEnergy = estimFunc(param , event) ;
		double target = targetEnergy(event.energy) ;
		chi2 += (1.0/nEventsPerEnergyMap[event.energy])*( target - estimEnergy )*( target - estimEnergy )/target ;
		//		chi2 += ( target - estimEnergy )*( target - estimEnergy )/target ;
		//		chi2 += std::abs( target - estimEnergy )/target ;
	}
	//		chi2 /= ( eventList.size()-1 ) ;
	chi2 /= nEventsPerEnergyMap.size() ;

	return chi2 ;
}

double EnergyMinimisation::targetEnergy(double _energy) const
{
	return (1.f+cheatIntensity)*_energy - cheatIntensity*(cheat[0] + cheat[1]*_energy + cheat[2]*_energy*_energy + cheat[3]*_energy*_energy*_energy) ;
}

void EnergyMinimisation::fitForCheat()
{
	std::cout << "Minimizer " << name << " fit for cheat with intensity : " << cheatIntensity << std::endl ;
	Fit fita ;
	fita.loadHistos(histoMap) ;
	fita.fitAllHistos() ;

	TGraphErrors* lin = fita.getLin() ;

	TF1* f = new TF1("f" , "[0] + [1]*x + [2]*x*x + [3]*x*x*x") ;
	lin->Fit(f) ;

	for ( int i = 0 ; i < 4 ; i++ )
		cheat[i] = f->GetParameter(i) ;
}

double EnergyMinimisation::minimize()
{
	std::cout << "Minimisation over " << eventsForMinim.size() << " entries" << std::endl ;
	std::cout << nEventsPerEnergyMap.size() << " different energies" << std::endl ;
	std::cout << std::endl ;

	return Minimisation::minimize() ;
}

void EnergyMinimisation::createHistos()
{
	histoMap.clear() ;

	for (std::vector<Event>::const_iterator it = events.begin() ; it != events.end() ; ++it)
	{
		Event event = *it ;

		if ( !histoMap.count(event.energy) )
		{
			std::stringstream toto ;
			toto << event.energy << "GeV" ;
			histoMap[event.energy] = std::shared_ptr<TH1>( new TH1D( toto.str().c_str() , toto.str().c_str() , 100 , 0 , 2*event.energy) ) ;
			histoMap[event.energy]->SetDirectory(0) ;
		}
		histoMap[event.energy]->Fill( estimFunc(&bestParam[0] , event) ) ;
	}
}

void EnergyMinimisation::writeHistos()
{
	std::stringstream fileName ;
	fileName << "histos_" << name << ".root" ;
	TFile* histoFile = new TFile(fileName.str().c_str() , "RECREATE") ;
	histoFile->cd() ;

	for ( auto it = histoMap.begin() ; it != histoMap.end() ; ++it )
		it->second->Write() ;

	histoFile->Close() ;
}


double LinearMinimisation::estimFunc(const double* param , Event _event) const
{
	return param[0]*(_event.hitThrDensity.at(1).at(0)) + param[1]*(_event.hitThrDensity.at(2).at(0)) + param[2]*(_event.hitThrDensity.at(3).at(0)) ;
}
double QuadMinimisation::estimFunc(const double* param , Event _event) const
{
	int nHit = _event.hitThrDensity.at(0).at(0) ;
	double alpha = param[0] + param[1]*nHit + param[2]*nHit*nHit ;
	double beta  = param[3] + param[4]*nHit + param[5]*nHit*nHit ;
	double gamma = param[6] + param[7]*nHit + param[8]*nHit*nHit ;

	return alpha*(_event.hitThrDensity.at(1).at(0)) + beta*(_event.hitThrDensity.at(2).at(0)) + gamma*(_event.hitThrDensity.at(3).at(0)) ;
}

double LinearDensityMinimisation::estimFunc(const double* param , Event _event) const
{
	double toReturn = 0 ;

	for ( unsigned int i = 1 ; i < 4 ; ++i )
		for ( unsigned int j = 1 ; j < 10 ; ++j )
			toReturn += _event.hitThrDensity.at(i).at(j) * param[9*(i-1)+j-1] ;

	return toReturn ;
}

double QuadHoughMinimisation::estimFunc(const double* param , Event _event) const
{
	int nHit = _event.hitThrDensity.at(0).at(0) ;
	double alpha = param[0] + param[1]*nHit + param[2]*nHit*nHit ;
	double beta  = param[3] + param[4]*nHit + param[5]*nHit*nHit ;
	double gamma = param[6] + param[7]*nHit + param[8]*nHit*nHit ;

	return alpha*(_event.hitThrDensity.at(1).at(0)-_event.nHough1) + beta*(_event.hitThrDensity.at(2).at(0)-_event.nHough2) + gamma*(_event.hitThrDensity.at(3).at(0)-_event.nHough3) + param[9]*(_event.nHough1 + _event.nHough2 + _event.nHough3) ;
}

double BinaryMinimisation::estimFunc(const double* param , Event _event) const
{
	int nHit = _event.hitThrDensity.at(0).at(0) ;
	return param[0] + param[1]*nHit + param[2]*nHit*nHit + param[3]*nHit*nHit*nHit ;
}
