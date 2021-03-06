#include "EnergyMinimisation.h"
#include "TimeCorrection.h"

#include <iostream>
#include <sstream>
#include <set>
#include <numeric>
#include <limits>
#include <array>

#include <TH1.h>
#include <TGraphErrors.h>
#include <TF1.h>

EnergyMinimisation::EnergyMinimisation(unsigned int nParam_ , std::string name_)
	: Minimisation(nParam_) , name(name_) ,
	  eventReader()
//	  fit()
{
	std::cout << "Initialize " << name << " energy minimizer" << std::endl ;
}

EnergyMinimisation::~EnergyMinimisation()
{
}

void EnergyMinimisation::loadFile(std::string fileName)
{
	std::cout << "Load file : " << fileName << std::endl ;
	TFile* file = new TFile(fileName.c_str() , "READ") ;
	TTree* tree ;
	file->GetObject("tree" , tree) ;

	eventReader.setTree(tree) ;

	int iEntry = 0 ;
	while ( tree->GetEntry(iEntry++) )
	{
		Event event = eventReader.getEvent(iEntry-1 , true) ;
		if ( !cut(event) )
			continue ;

		if ( !nEventsPerEnergyMap.count(event.energy) )
			nEventsPerEnergyMap[event.energy] = 0 ;

		if ( nEventsPerEnergyMap[event.energy] < nEventsPerEnergy )
		{
			nEventsPerEnergyMap[event.energy]++ ;
			eventListForMinim.push_back( event ) ;
		}

		eventList.push_back( event ) ;
	}

	delete tree ;
	file->Close() ;
}

void EnergyMinimisation::loadFile(std::string fileName , unsigned long long beginTime, unsigned long long endTime)
{
	std::cout << "Load file : " << fileName << ", and correct time with begin = " << beginTime << " to end = " << endTime << std::endl ;
	TimeCorrection timeCorr ;
	timeCorr.setBeginTime(beginTime) ;
	timeCorr.setEndTime(endTime) ;

	TFile* file = new TFile(fileName.c_str() , "READ") ;
	TTree* tree ;
	file->GetObject("tree" , tree) ;

	eventReader.setTree(tree) ;

	beginSpillCut = beginTime ;
	endSpillCut = endTime ;

	std::vector<Event> temp ;

	int iEntry = 0 ;
	while ( tree->GetEntry(iEntry++) )
	{
		Event event = eventReader.getEvent(iEntry-1 , true) ;
		if ( !cut(event) )
			continue ;

		temp.push_back(event) ;
	}

	timeCorr.correctThrDensity(temp) ;

	for ( std::vector<Event>::const_iterator it = temp.begin() ; it != temp.end() ; ++it )
	{
		if ( !nEventsPerEnergyMap.count(it->energy) )
			nEventsPerEnergyMap[ it->energy ] = 0 ;

		if ( nEventsPerEnergyMap[ it->energy ] < nEventsPerEnergy )
		{
			nEventsPerEnergyMap[ it->energy ]++ ;
			eventListForMinim.push_back( *it ) ;
		}

		eventList.push_back( *it ) ;
	}

	delete tree ;
	file->Close() ;
}


bool EnergyMinimisation::cut(Event event) const
{
	bool beamCut = false ;

	if (geomCut.at(2) < std::numeric_limits<double>::epsilon())
		beamCut = true ;
	else
		beamCut = ( event.cog[0]-geomCut.at(0) )*( event.cog[0]-geomCut.at(0) ) + ( event.cog[2]-geomCut.at(1) )*( event.cog[2]-geomCut.at(1) ) < geomCut.at(2)*geomCut.at(2) ;

	bool timeCut = event.spillEventTime >= beginSpillCut && event.spillEventTime < endSpillCut ;

	bool cut = ( event.transverseRatio > 0.05f && event.neutral == 0 && event.nTrack > 0 && double(event.nHit)/event.nLayer > 2.2 && double(event.nInteractingLayer)/event.nLayer > 0.2 ) ;

	bool elecCut = ( event.begin > 5 || event.nLayer > 30 ) ;

	return ( cut && beamCut && timeCut && elecCut ) ;
}

double EnergyMinimisation::functionToMinimize(const double* param)
{
	double chi2 = 0 ;

	for( std::vector<Event>::const_iterator it = eventListForMinim.begin() ; it != eventListForMinim.end() ; ++it )
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
	std::cout << "Minimisation over " << eventListForMinim.size() << " entries" << std::endl ;
	std::cout << nEventsPerEnergyMap.size() << " different energies" << std::endl ;
	std::cout << std::endl ;

	return Minimisation::minimize() ;
}

void EnergyMinimisation::createHistos()
{
	histoMap.clear() ;

	for (std::vector<Event>::const_iterator it = eventList.begin() ; it != eventList.end() ; ++it)
	{
		Event event = *it ;
		if ( !cut(event) )
			continue ;

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
