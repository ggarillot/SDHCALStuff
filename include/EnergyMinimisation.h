#ifndef EnergyMinimisation_h
#define EnergyMinimisation_h

#include "Minimisation.h"
#include "Event.h"
#include "Fit.h"

#include <TFile.h>
#include <TTree.h>
#include <vector>
#include <map>
#include <string>

class EnergyMinimisation : public Minimisation
{
	public :
		EnergyMinimisation(unsigned int nParam_ , std::string name_) ;
		virtual ~EnergyMinimisation() ;

		virtual bool cut(Event event) const ;

		void loadFile(std::string fileName) ;
		void loadFile(std::string fileName , unsigned long long beginTime) ;


		double functionToMinimize(const double* param) ;
		virtual double estimFunc(const double* param , Event _event) const = 0 ;

		double minimize() ;

		std::string getName() { return name ; }

		std::map<float,TH1*> createHistos() ;
		void writeHistos() ;

		void drawResults() ;

		double targetEnergy(double _energy) const ;

		void fitForCheat() ;

		inline Fit& getFit() { return fit ; }


		void setName(std::string _name) { name = _name ; }

	protected :
		std::string name ;

		EventReader eventReader ;
		std::vector<Event> eventList ;

		int nEventsPerEnergy = 300000000 ;
		std::map<float,int> nEventsPerEnergyMap ;

		std::vector<TFile*> fileVec ;
		std::vector<TTree*> treeVec ;

		Fit fit ;

		double cheat[4] = {0,1,0,0} ; //to improve linearity


} ;

class LinearMinimisation : public EnergyMinimisation
{
	public :
		LinearMinimisation(std::string name_ = "Linear") : EnergyMinimisation(3, name_) {}

		// bool cut() const {return true ;}
		double estimFunc(const double* param , Event _event) const ;
} ;

class QuadMinimisation : public EnergyMinimisation
{
	public :
		QuadMinimisation(std::string name_ = "Quad") : EnergyMinimisation(9, name_) {}

		// bool cut() const {return true ;}
		double estimFunc(const double* param , Event _event) const ;
} ;

class QuadHoughMinimisation : public EnergyMinimisation
{
	public :
		QuadHoughMinimisation(std::string name_ = "QuadHough") : EnergyMinimisation(10, name_) {}

		// bool cut() const {return true ;}
		double estimFunc(const double* param , Event _event) const ;
} ;

class BinaryMinimisation : public EnergyMinimisation
{
	public :
		BinaryMinimisation(std::string name_ = "Binary") : EnergyMinimisation(4, name_) {}

		double estimFunc(const double* param , Event _event) const ;
} ;


#endif // EnergyMinimisation_h
