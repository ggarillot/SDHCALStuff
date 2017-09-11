#ifndef HistoCreator_h
#define HistoCreator_h

#include <string>

#include <TProfile.h>
#include <TColor.h>
#include <Rtypes.h>

#include "Event.h"


class HistoCreator
{
	public :
		HistoCreator() ;
		~HistoCreator() ;

		TProfile* getNHitOverTimeProfile(std::string profName , double timeRef) const ;
		TProfile* getNHit1OverTimeProfile(std::string profName , double timeRef) const ;
		TProfile* getNHit2OverTimeProfile(std::string profName , double timeRef) const ;
		TProfile* getNHit3OverTimeProfile(std::string profName , double timeRef) const ;

		TProfile* getMeanRadiusOverTimeProfile(std::string profName , double timeRef) const ;

		double getMeanRadius() const ;

		TH1D* getNHit(std::string histName) const ;
		TH1D* getLongiProfile(std::string histName) const ;
		TH1D* getRadiProfile(std::string histName) const ;

		inline void setEventList(const std::vector<Event>& eList) { eventList = eList ; }

		inline void setColor(Color_t col) { color = gROOT->GetColor(col) ; }
		inline void setFillColor(Color_t col) { fillColor = gROOT->GetColor(col) ; }

		inline void setDataStyle(bool isData) { dataStyle = isData ; }


		HistoCreator(const HistoCreator &toCopy) = delete ;
		void operator=(const HistoCreator &toCopy) = delete ;


	protected :

		std::vector<Event> eventList ;
		TColor* color = gROOT->GetColor(kBlack) ;
		TColor* fillColor = gROOT->GetColor(kBlack) ;
		bool dataStyle = false ;

} ;


#endif //HistoCreator_h
