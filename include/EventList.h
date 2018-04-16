#ifndef EventList_h
#define EventList_h

#include <vector>
#include <string>
#include <memory>

#include "Event.h"

class EventList
{
	public :
		EventList() = default ;
		~EventList() ;

		bool cut(const Event& event) const ;

		void loadFile(std::string fileName) ;
		void loadFile(std::string fileName , unsigned long long beginTime , unsigned long long endTime) ;

		const std::vector<std::shared_ptr<Event>>& events() const { return _events ; }

		void setGeomCut(std::array<double,3> _geomCut) { geomCut = _geomCut ; }

	protected :
		std::vector<std::shared_ptr<Event>> _events = {} ;

		//cuts
		std::array<double,3> geomCut = {{0,0,0}} ;
		double beginTimeCut = 0 ;
		double endTimeCut = 50e6 ;
} ;

#endif //EventList_h
