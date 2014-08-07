/*!
 * \file CvFindCirclesGrid_Processor.hpp
 * \brief Circles grid localization component.
 * \date Apr 29, 2014
 * \author lzmuda
 */

#ifndef CVFINDCIRCLESGRID_PROCESSOR_HPP_
#define CVFINDCIRCLESGRID_PROCESSOR_HPP_

#include <boost/shared_ptr.hpp>
#include "Component_Aux.hpp"
#include "Types/Objects3D/Chessboard.hpp"
#include "Types/ImagePosition.hpp"
#include "Types/Drawable.hpp"
#include "Timer.hpp"
#include "Property.hpp"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace Processors {
namespace CvFindCirclesGrid {

using namespace cv;


#define ELEMS BOOST_PP_TUPLE_TO_LIST(5, (NEAREST, LINEAR, AREA, CUBIC, LANCZOS4))
GENERATE_ENUM_TRANSLATOR(InterpolationTranslator, int, ELEMS, INTER_);

class CvFindCirclesGrid_Processor: public Base::Component
{
public:
	CvFindCirclesGrid_Processor(const std::string & name = "");
	virtual ~CvFindCirclesGrid_Processor();

	/*!
	 * Prepares communication interface.
	 */
	virtual void prepareInterface();


protected:
	/*!
	 * Method called when component is started
	 * \return true on success
	 */
	virtual bool onStart();

	/*!
	 * Method called when component is stopped
	 * \return true on success
	 */
	virtual bool onStop();

	/*!
	 * Method called when component is initialized
	 * \return true on success
	 */
	virtual bool onInit();

	/*!
	 * Method called when component is finished
	 * \return true on success
	 */
	virtual bool onFinish();

	/*!
	 * Method called when step is called
	 * \return true on success
	 */
	virtual bool onStep();


private:
	void onNewImage();

	void initChessboard();

	/** New image event handler. */
	Base::EventHandler <CvFindCirclesGrid_Processor> h_onNewImage;
	/** Image stream. */
	Base::DataStreamIn <cv::Mat> in_img;
	/** Chessboard stream. */
	Base::DataStreamOut <Types::Objects3D::Chessboard> out_chessboard;
	Base::DataStreamOut <Types::ImagePosition> out_imagePosition;

	Base::DataStreamOut <cv::Mat> out_img;

	/** Located corners.*/
	std::vector<cv::Point2f> corners;

	int findChessboardCornersFlags;

	Common::Timer timer;

	boost::shared_ptr<Types::Objects3D::Chessboard> chessboard;

	cv::Mat sub_img;

	Base::Property<int> prop_width;
	Base::Property<int> prop_height;
	Base::Property<float> prop_square_size;

	Base::Property<bool> prop_inverse;

	Base::Property<int, InterpolationTranslator> prop_interpolation_type;

	// TODO: add unit types: found and not found

	void sizeCallback(int old_value, int new_value);
};

} // namespace CvFindCirclesGrid {

} // namespace Processors {

REGISTER_COMPONENT("CvFindCirclesGrid", Processors::CvFindCirclesGrid::CvFindCirclesGrid_Processor)

#endif /* CVFINDCIRCLESGRID_PROCESSOR_HPP_ */