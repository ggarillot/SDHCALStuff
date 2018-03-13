#ifndef EnergyMinimisation_h
#define EnergyMinimisation_h

#include "Minimisation.h"
#include "Event.h"
#include "Fit.h"
#include "EventList.h"

#include <TFile.h>
#include <TTree.h>
#include <vector>
#include <array>
#include <map>
#include <string>
#include <sstream>
#include <memory>


class EnergyMinimisation : public Minimisation
{
	public :
		EnergyMinimisation(unsigned int nParam_ , std::string name_) ;
		virtual ~EnergyMinimisation() ;

		void loadEvents(const std::vector<EventList>& eventListVec ) ;


		double functionToMinimize(const double* param) ;
		virtual double estimFunc(const double* param , Event _event) const = 0 ;

		double minimize() ;

		std::string getName() { return name ; }

		void createHistos() ;
		void writeHistos() ;

		std::map<float,std::shared_ptr<TH1>> const getHistos() { return histoMap ; }

		double targetEnergy(double _energy) const ;

		void fitForCheat() ;

		void setName(std::string _name) { name = _name ; }



		void setCheatIntensity(float intensity) { cheatIntensity = intensity ; }

	protected :
		std::string name ;

		std::vector<Event> events = {} ;
		std::vector<Event> eventsForMinim = {} ;

		int nEventsPerEnergy = 3000 ;
		std::map<float,int> nEventsPerEnergyMap = {} ;

		std::map<float,std::shared_ptr<TH1>> histoMap = {} ;

		float cheatIntensity = 1.0f ;
		double cheat[4] = {0,1,0,0} ; //to improve linearity
} ;

class LinearMinimisation : public EnergyMinimisation
{
	public :
		LinearMinimisation(std::string name_ = "Linear") : EnergyMinimisation(3, name_) {}

		double estimFunc(const double* param , Event _event) const ;
} ;

class QuadMinimisation : public EnergyMinimisation
{
	public :
		QuadMinimisation(std::string name_ = "Quad") : EnergyMinimisation(9, name_)
		{
			limitsParam[8] = kPositive ;
		}

		double estimFunc(const double* param , Event _event) const ;
} ;

class LinearDensityMinimisation : public EnergyMinimisation
{
	public :
		LinearDensityMinimisation(std::string name_ = "LinearDensity") : EnergyMinimisation(27, name_)
		{
			limitsParam = std::vector<Limits>(nParam , kPositive) ;

			for ( unsigned int i = 0 ; i < 9 ; ++i )
			{
				std::stringstream alpha ; alpha << "alpha" << i ;
				std::stringstream beta ; beta << "beta" << i ;
				std::stringstream gamma ; gamma << "gamma" << i ;

				nameParam[i] = alpha.str() ;
				nameParam[i+9] = beta.str() ;
				nameParam[i+18] = gamma.str() ;
			}
		}
		//		LinearDensityMinimisation(std::string name_ = "LinearDensity") : EnergyMinimisation(12, name_)
		//		{
		//			limitsParam = std::vector<Limits>(nParam , kPositive) ;

		//			for ( unsigned int i = 0 ; i < 9 ; ++i )
		//			{
		//				std::stringstream gamma ; gamma << "d" << i+1 ;
		//				nameParam[i] = gamma.str() ;
		//			}
		//			nameParam[9] = "alpha" ;
		//			nameParam[10] = "beta" ;
		//			nameParam[11] = "gamma" ;
		//		}

		double estimFunc(const double* param , Event _event) const ;
} ;

class QuadHoughMinimisation : public EnergyMinimisation
{
	public :
		QuadHoughMinimisation(std::string name_ = "QuadHough") : EnergyMinimisation(10, name_)
		{
			limitsParam[8] = kPositive ;
		}

		double estimFunc(const double* param , Event _event) const ;
} ;

class BinaryMinimisation : public EnergyMinimisation
{
	public :
		BinaryMinimisation(std::string name_ = "Binary") : EnergyMinimisation(4, name_) {}

		double estimFunc(const double* param , Event _event) const ;
} ;


#endif // EnergyMinimisation_h
