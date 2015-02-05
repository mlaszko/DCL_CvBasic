/*!
 * \file
 * \brief
 * \author Michal Laszkowski
 */

#include <memory>
#include <string>

#include "TimeTrigger.hpp"
#include "Common/Logger.hpp"

#include <boost/bind.hpp>

#include "boost/date_time/posix_time/posix_time.hpp"

namespace Processors {
namespace TimeTrigger {

TimeTrigger::TimeTrigger(const std::string & name) :
		Base::Component(name) , 
		time("time", 1000), 
		on_start("on_start", true) {
	registerProperty(time);
	registerProperty(on_start);

}

TimeTrigger::~TimeTrigger() {
}

void TimeTrigger::prepareInterface() {
	// Register data streams, events and event handlers HERE!
	registerStream("out_trigger", &out_trigger);
	// Register handlers
	h_trigger.setup(boost::bind(&TimeTrigger::trigger, this));
	registerHandler("trigger", &h_trigger);
	addDependency("trigger", NULL);
	h_start.setup(boost::bind(&TimeTrigger::start, this));
	registerHandler("start", &h_start);
	h_stop.setup(boost::bind(&TimeTrigger::stop, this));
	registerHandler("stop", &h_stop);

}

bool TimeTrigger::onInit() {
    if(on_start)
        on = true;
    else
        on = false;
    lt = boost::posix_time::microsec_clock::local_time();
	return true;
}

bool TimeTrigger::onFinish() {
	return true;
}

bool TimeTrigger::onStop() {
	return true;
}

bool TimeTrigger::onStart() {
	return true;
}

void TimeTrigger::trigger() {
    CLOG(LTRACE) << "TimeTrigger::trigger";
    if(on){
        boost::posix_time::ptime tm = boost::posix_time::microsec_clock::local_time();
        if(tm > lt + boost::posix_time::microseconds(time)){
            CLOG(LDEBUG) << "triggered ";
            lt = tm;
            Base::UnitType t;
            out_trigger.write(t);
        }

    }
}

void TimeTrigger::start() {
    on = true;
}

void TimeTrigger::stop() {
    on = false;
}



} //: namespace TimeTrigger
} //: namespace Processors
