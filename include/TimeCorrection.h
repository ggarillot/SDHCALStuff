#ifndef TimeCorrection_h
#define TimeCorrection_h

#include <TF1.h>
#include <TProfile.h>

#include "Event.h"
#include "HistoCreator.h"

class TimeCorrection
{
	public :
		TimeCorrection() = default ;
		~TimeCorrection() = default ;

		inline void setBeginTime(unsigned long long time) { beginTime = time ; }
		inline void setEndTime(unsigned long long time) { endTime = time ; }

		void correct(std::vector<Event>& eventList, int polOrder = 2) ;

		void correctThrDensity(std::vector<Event>& eventList, int polOrder = 2) ;

	protected :
		unsigned long long beginTime = 0 ;
		unsigned long long endTime = 50e6 ;
};

#endif //TimeCorrection_h
