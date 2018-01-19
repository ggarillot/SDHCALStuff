#include "EnergyMinimisation.h"
#include "TimeCorrection.h"

#include <iostream>
#include <sstream>
#include <set>
#include <numeric>

#include <TH1.h>
#include <TGraphErrors.h>
#include <TF1.h>

using namespace std ;

EnergyMinimisation::EnergyMinimisation(unsigned int nParam_ , std::string name_)
	: Minimisation(nParam_) , name(name_) ,
	  eventReader() ,
	  eventList() ,
	  nEventsPerEnergyMap() ,
	  fileVec() ,
	  treeVec() ,
	  fit()
{
}

EnergyMinimisation::~EnergyMinimisation()
{

}

void EnergyMinimisation::loadFile(std::string fileName)
{
	TFile* file = new TFile(fileName.c_str() , "READ") ;
	TTree* tree ;
	file->GetObject("tree" , tree) ;

	//	fileVec.push_back(file) ;
	//	treeVec.push_back(tree) ;

	eventReader.setTree(tree) ;
	//	event.setBranchAddress(tree) ;

	int iEntry = 0 ;
	while ( tree->GetEntry(iEntry++) )
	{
		Event event = eventReader.getEvent(iEntry-1) ;
		if ( !cut(event) )
			continue ;

		if ( !nEventsPerEnergyMap.count(event.energy) )
			nEventsPerEnergyMap[event.energy] = 0 ;

		if ( nEventsPerEnergyMap[event.energy] < nEventsPerEnergy )
		{
			nEventsPerEnergyMap[event.energy]++ ;
			eventList.push_back( event ) ;
		}
	}

	delete tree ;
	file->Close() ;
}

void EnergyMinimisation::loadFile(std::string fileName , unsigned long long beginTime)
{
	TimeCorrection timeCorr ;
	timeCorr.setBeginTime(beginTime) ;

	TFile* file = new TFile(fileName.c_str() , "READ") ;
	TTree* tree ;
	file->GetObject("tree" , tree) ;

	//	fileVec.push_back(file) ;
	//	treeVec.push_back(tree) ;

	eventReader.setTree(tree) ;
	//	event.setBranchAddress(tree) ;


	std::vector<Event> temp ;

	int iEntry = 0 ;
	while ( tree->GetEntry(iEntry++) )
	{
		Event event = eventReader.getEvent(iEntry-1) ;
		if ( !cut(event) )
			continue ;

		temp.push_back(event) ;
	}

	timeCorr.correct(temp) ;

	for ( std::vector<Event>::const_iterator it = temp.begin() ; it != temp.end() ; ++it )
	{
		if ( !nEventsPerEnergyMap.count(it->energy) )
			nEventsPerEnergyMap[ it->energy ] = 0 ;

		if ( nEventsPerEnergyMap[ it->energy ] < nEventsPerEnergy )
		{
			nEventsPerEnergyMap[ it->energy ]++ ;
			eventList.push_back( *it ) ;
		}
	}

	delete tree ;
	file->Close() ;
}


bool EnergyMinimisation::cut(Event event) const
{
	//	bool geomCut = std::sqrt( (event.cog[0]-600)*(event.cog[0]-600) + (event.cog[2]-530)*(event.cog[2]-530) ) < 95 ;
			bool geomCut = std::sqrt( (event.cog[0]-480)*(event.cog[0]-480) + (event.cog[2]-460)*(event.cog[2]-460) ) < 80 ;
//	bool geomCut = true ;
	bool timeCut = event.spillEventTime < 30e6 ;
	bool cut = ( event.transverseRatio > 0.05f && event.neutral == 0 /*&& event.nTrack > 0*/ && double(event.nHit)/event.nLayer > 2.2 && double(event.nInteractingLayer)/event.nLayer > 0.2 ) ;

	return ( cut && geomCut && timeCut ) ;
	//	return true ;
}

double EnergyMinimisation::functionToMinimize(const double* param)
{
	double chi2 = 0 ;

	for( vector<Event>::const_iterator it = eventList.begin() ; it != eventList.end() ; ++it )
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
	return 2*_energy - (cheat[0] + cheat[1]*_energy + cheat[2]*_energy*_energy + cheat[3]*_energy*_energy*_energy) ;
}

void EnergyMinimisation::fitForCheat()
{
	fit.loadHistos( createHistos() ) ;
	fit.fitAllHistos() ;

	TGraphErrors* lin = fit.getLin() ;

	TF1* f = new TF1("f" , "[0] + [1]*x + [2]*x*x + [3]*x*x*x") ;
	lin->Fit(f) ;

	for ( int i = 0 ; i < 4 ; i++ )
		cheat[i] = f->GetParameter(i) ;
}

double EnergyMinimisation::minimize()
{
	cout << "Minimisation over " << eventList.size() << " entries" << endl ;
	cout << nEventsPerEnergyMap.size() << " different energies" << endl ;
	cout << endl ;

	return Minimisation::minimize() ;
}

std::map<float,TH1*> EnergyMinimisation::createHistos()
{
	map<float,TH1*> histoMap ;

	for (vector<Event>::const_iterator it = eventList.begin() ; it != eventList.end() ; ++it)
	{
		Event event = *it ;
		if ( !cut(event) )
			continue ;

		if ( !histoMap.count(event.energy) )
		{
			stringstream toto ;
			toto << event.energy << "GeV" ;
			histoMap[event.energy] = new TH1D( toto.str().c_str() , toto.str().c_str() , 100 , 0 , 2*event.energy) ;
		}
		histoMap[event.energy]->Fill( estimFunc(&bestParam[0] , event) ) ;
	}

	fit.loadHistos(histoMap) ;
	return histoMap ;
}

void EnergyMinimisation::writeHistos()
{
	map<float,TH1*> histoMap = createHistos() ;

	stringstream fileName ;
	fileName << "histos_" << name << ".root" ;
	TFile* histoFile = new TFile(fileName.str().c_str() , "RECREATE") ;
	histoFile->cd() ;

	for ( map<float,TH1*>::iterator it = histoMap.begin() ; it != histoMap.end() ; ++it )
		it->second->Write() ;

	histoFile->Close() ;
}

void EnergyMinimisation::drawResults()
{
	fit.fitAllHistos() ;
	fit.drawLin() ;
	fit.drawLinDev() ;
	fit.drawResol() ;
}

double LinearMinimisation::estimFunc(const double* param , Event _event) const
{
	return param[0]*(_event.nHit1) + param[1]*(_event.nHit2) + param[2]*(_event.nHit3) ;
}
double QuadMinimisation::estimFunc(const double* param , Event _event) const
{
	double alpha = param[0] + param[1]*_event.nHit + param[2]*_event.nHit*_event.nHit ;
	double beta  = param[3] + param[4]*_event.nHit + param[5]*_event.nHit*_event.nHit ;
	double gamma = param[6] + param[7]*_event.nHit + param[8]*_event.nHit*_event.nHit ;

	return alpha*(_event.nHit1) + beta*(_event.nHit2) + gamma*(_event.nHit3) ;
}

double LinearDensityMinimisation::estimFunc(const double* param , Event _event) const
{
	//	auto list = std::vector<int>(nParam , 0) ;

	auto list = std::vector< std::vector<double> >(3 , std::vector<double>(9,0) ) ;

	assert( _event.thr.size() == _event.densityPerHit.size() ) ;

	for ( unsigned i = 0 ; i < _event.thr.size() ; ++i )
	{
		unsigned int density = _event.densityPerHit[i]-1 ;
		unsigned int thr = _event.thr[i]-1 ;

		//		list.at( thr ).at( density ) += param[density] ;
		list.at( thr ).at( density ) ++ ;
		//		list.at( _event.densityPerHit[i]-1 ) ++ ;
		//		if ( _event.thr[i] >= 2 )
		//			list.at( 9 + _event.densityPerHit[i]-1 ) ++ ;
		//		if ( _event.thr[i] >= 3 )
		//			list.at( 18 + _event.densityPerHit[i]-1 ) ++ ;
	}

	double toReturn = 0 ;
	//	for ( unsigned int i = 0 ; i < nParam ; ++i )
	//		toReturn += param[i]*list[i] ;


	//	for ( unsigned int i = 0 ; i < 3 ; ++i )
	//		toReturn += std::accumulate(list.at(i).begin() , list.at(i).end() , 0.0)*param[9+i] ;

	for ( unsigned int i = 0 ; i < 3 ; ++i )
		for ( unsigned int j = 0 ; j < 9 ; ++j )
			toReturn += list.at(i).at(j) * param[9*i+j] ;

	return toReturn ;
}

double QuadHoughMinimisation::estimFunc(const double* param , Event _event) const
{
	double alpha = param[0] + param[1]*_event.nHit + param[2]*_event.nHit*_event.nHit ;
	double beta  = param[3] + param[4]*_event.nHit + param[5]*_event.nHit*_event.nHit ;
	double gamma = param[6] + param[7]*_event.nHit + param[8]*_event.nHit*_event.nHit ;

	return alpha*(_event.nHit1-_event.nHough1) + beta*(_event.nHit2-_event.nHough2) + gamma*(_event.nHit3-_event.nHough3) + param[9]*(_event.nHough1 + _event.nHough2 + _event.nHough3) ;
}

double BinaryMinimisation::estimFunc(const double* param , Event _event) const
{
	return param[0] + param[1]*_event.nHit + param[2]*_event.nHit*_event.nHit + param[3]*_event.nHit*_event.nHit*_event.nHit ;
}
