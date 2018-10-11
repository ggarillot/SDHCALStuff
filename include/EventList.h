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
		~EventList() = default ;

		EventList( const EventList& other ) ;
		EventList& operator=(const EventList& other) = delete ;

		void reset() ;

		bool cut(const Event& event) const ;

		void setBeginTimeCut(double value) { beginTimeCut = value ; }
		void setEndTimeCut(double value) { endTimeCut = value ; }

		void loadFile(std::string fileName) ;
		void loadFile(std::string fileName , unsigned long long beginTime , unsigned long long endTime) ;

		const std::vector<std::shared_ptr<Event>>& events() const { return _events ; }
		const std::vector<std::shared_ptr<Event>>& eventsConst() const { return events() ; }
		std::vector<std::shared_ptr<Event>>& eventsNotConst() { return _events ; }

		void setGeomCut(std::array<double,3> _geomCut) { geomCut = _geomCut ; }

		enum EventType
		{
			kHadron ,
			kElectron ,
			kNothing
		} ;

		void setType( EventType type ) { eventType = type ; }


	protected :
		std::vector<std::shared_ptr<Event>> _events = {} ;
		EventType eventType = kNothing ;

		//cuts
		std::array<double,3> geomCut = {{0,0,0}} ;
		double beginTimeCut = 0 ;
		double endTimeCut = 50e6 ;
} ;

#endif //EventList_h
