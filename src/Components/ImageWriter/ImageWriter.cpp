/*!
 * \file
 * \brief
 * \author Maciej Stefańczyk [maciek.slon@gmail.com]
 */

#include <memory>
#include <string>
#include <sstream>
#include <iomanip>

#include "ImageWriter.hpp"
#include "Common/Logger.hpp"

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace Processors {
namespace ImageWriter {

ImageWriter::ImageWriter(const std::string & name) :
		Base::Component(name),
		directory("directory", std::string(".")),
		base_name("base_name", std::string("img")),
		format("format", std::string("png")),
		digits("digits", 2),
		count("count", 1),
		prop_auto_trigger("auto_trigger", false)
{
	registerProperty(directory);
	registerProperty(base_name);
	registerProperty(format);
	registerProperty(digits);
	registerProperty(count);
	registerProperty(prop_auto_trigger);
}

ImageWriter::~ImageWriter() {
}

void ImageWriter::prepareInterface() {
	CLOG(LTRACE) << name() << "::prepareInterface()";

	// Register handlers
	Base::EventHandler2 * hand;
	for (int i = 0; i < count; ++i) {
		char id = '0'+i;
		hand = new Base::EventHandler2;
		registerHandler(std::string("write_image_")+id, boost::bind(&ImageWriter::write_image_N, this, i));

		Base::DataStreamIn<cv::Mat, Base::DataStreamBuffer::Newest> * stream = new Base::DataStreamIn<cv::Mat, Base::DataStreamBuffer::Newest>;
		in_img.push_back(stream);
		registerStream( std::string("in_img")+id, (Base::DataStreamInterface*)(in_img[i]));
		addDependency(std::string("write_image_")+id, stream);

		// Add n flags for manual trigger.
		bool tmp_flag = false;
		save_flags.push_back(tmp_flag);
	}


	// register aliases for first handler and streams
	registerHandler("write_image", boost::bind(&ImageWriter::write_image_N, this, 0));
	registerStream("in_img", in_img[0]);

	counts.resize(count, 0);

	std::string t = base_name;
	boost::split(base_names, t, boost::is_any_of(","));
	if ( (base_names.size() == 1) && (count > 1) ) base_names.clear();
	for (int i = base_names.size(); i < count; ++i) {
		char id = '0' + i;
		base_names.push_back(std::string(base_name) + id);
	}

	std::string f = format;
	boost::split(formats, f, boost::is_any_of(","));
	if ( (formats.size() == 1) && (count > 1) ) formats.clear();
	for (int i = formats.size(); i < count; ++i) {
		formats.push_back(format);
	}


	registerStream("in_save_trigger", &in_save_trigger);
    // Register handlers - save image, can be triggered manually (from GUI) or by new data present in_load_next_image_trigger dataport.
    // 1st version - manually.
    registerHandler("SaveImage", boost::bind(&ImageWriter::onSaveButtonPressed, this));

    // 2nd version - external trigger.
    registerHandler("onSaveTriggered", boost::bind(&ImageWriter::onSaveTriggered, this));
    addDependency("onSaveTriggered", &in_save_trigger);

}

bool ImageWriter::onInit() {
	return true;
}

bool ImageWriter::onFinish() {
	return true;
}

bool ImageWriter::onStop() {
	return true;
}

bool ImageWriter::onStart() {
	return true;
}


void ImageWriter::onSaveButtonPressed() {
	CLOG(LTRACE) << name() << "::onSaveButtonPressed()";
	for (int i = 0; i < count; ++i)
		save_flags[i] = true;
}

void ImageWriter::onSaveTriggered(){
	CLOG(LTRACE) << name() << "::onSaveTriggered()";
	in_save_trigger.read();
	for (int i = 0; i < count; ++i)
		save_flags[i] = true;
}



void ImageWriter::write_image_N(int n) {
	CLOG(LTRACE) << name() << "::write_image_N(" << n << ")";

	boost::posix_time::ptime tm = boost::posix_time::microsec_clock::local_time();

	// Check working mode.
	if ((!prop_auto_trigger && !save_flags[n])){
		return;
	}
	save_flags[n] = false;

	try {
		if(!in_img[n]->empty()){
			counts[n] = counts[n] + 1;
			std::stringstream ss;
			ss << std::setw(digits) << std::setfill('0') << counts[n];
			//std::string fname = std::string(directory) + "/" + base_names[n] + boost::lexical_cast<std::string>(counts[n]) + "." + formats[n];
			std::string fname = std::string(directory) + "/" + boost::posix_time::to_iso_extended_string(tm) + "_" + base_names[n] + "." + formats[n];

			// Write to file depending on the extension.
			// Write to yaml.
			if ((formats[n] == "yaml") || (formats[n] == "yml") || (formats[n] == "xml") || (formats[n] == "yml.gz") || (formats[n] == "yaml.gz") || (formats[n] == "xml.gz")){
				CLOG(LNOTICE) << "Writing "<< n <<"-th image to YAML file" << fname;
			    cv::FileStorage fs(fname, cv::FileStorage::WRITE);
				fs << "img" << in_img[n]->read();
			    fs.release();
			}
			else
			{
				CLOG(LNOTICE) << "Writing "<< n <<"-th image to file" << fname;
				cv::imwrite(fname, in_img[n]->read());

			}
		}
		else
			CLOG(LWARNING) <<"Image N(" << n << ") empty!";
	}
	catch(std::exception &ex) {
		CLOG(LERROR) << "ImageWriter::write_image_N failed: " << ex.what() << "\n";
	}
}



} //: namespace ImageWriter
} //: namespace Processors
