/*!
 * \file
 * \brief 
 * \author Michal Laszkowski
 */

#ifndef TIMETRIGGER_HPP_
#define TIMETRIGGER_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "DataStream.hpp"
#include "Property.hpp"
#include "EventHandler2.hpp"



namespace Processors {
namespace TimeTrigger {

/*!
 * \class TimeTrigger
 * \brief TimeTrigger processor class.
 *
 * 
 */
class TimeTrigger: public Base::Component {
public:
	/*!
	 * Constructor.
	 */
	TimeTrigger(const std::string & name = "TimeTrigger");

	/*!
	 * Destructor
	 */
	virtual ~TimeTrigger();

	/*!
	 * Prepare components interface (register streams and handlers).
	 * At this point, all properties are already initialized and loaded to 
	 * values set in config file.
	 */
	void prepareInterface();

protected:

	/*!
	 * Connects source to given device.
	 */
	bool onInit();

	/*!
	 * Disconnect source from device, closes streams, etc.
	 */
	bool onFinish();

	/*!
	 * Start component
	 */
	bool onStart();

	/*!
	 * Stop component
	 */
	bool onStop();

    bool on;
    ///
    boost::posix_time::ptime lt;

	// Input data streams

	// Output data streams
	Base::DataStreamOut<Base::UnitType> out_trigger;

	// Handlers
	Base::EventHandler2 h_trigger;
	Base::EventHandler2 h_start;
	Base::EventHandler2 h_stop;

	// Properties
	Base::Property<int> time;
    Base::Property<bool> on_start;

	
	// Handlers
	void trigger();
	void start();
	void stop();

};

} //: namespace TimeTrigger
} //: namespace Processors

/*
 * Register processor component.
 */
REGISTER_COMPONENT("TimeTrigger", Processors::TimeTrigger::TimeTrigger)

#endif /* TIMETRIGGER_HPP_ */
