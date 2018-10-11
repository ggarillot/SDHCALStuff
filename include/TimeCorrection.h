#ifndef TimeCorrection_h
#define TimeCorrection_h

#include <TF1.h>
#include <TProfile.h>

#include <memory>
#include <string>

#include "EventList.h"
#include "HistoCreator.h"

class TimeCorrection
{
	public :
		TimeCorrection() = default ;
		~TimeCorrection() = default ;

		inline void setBeginTime(unsigned long long time) { beginTime = time ; }
		inline void setEndTime(unsigned long long time) { endTime = time ; }

		void correctHits(EventList& eventList, int polOrder = 2) ;
		void correctProfiles(EventList& eventList, int polOrder = 2) ;

		void correctThrDensity(std::vector<std::shared_ptr<Event>>& events, int polOrder = 2) ;
		void correctThrDensity(EventList& eventList, int polOrder = 2) ;

	protected :
		std::string getFuncStr(int polOrder) const ;

		unsigned long long beginTime = 0 ;
		unsigned long long endTime = 50e6 ;
};

#endif //TimeCorrection_h
