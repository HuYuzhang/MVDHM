#ifndef __ODATA__
#define __ODATA__

#include "CommonDef.h"
#include <vector>
#include <map>
#include <opencv2\core\core.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#define X_DIR 0
#define Y_DIR 1
typedef cv::Mat OFMap;
struct OFInfo
{
	Int prevPOC;
	Int opIndex;
	Int curPOC;
	Int curIndex;
};
class OData
{
private:
	std::vector<Float> Ox;
	std::vector<Float> Oy;
	std::vector<UInt> avaFlag;
	std::vector<Int> prev;
	std::vector<Int> POCs;
	Int prevPOC;
	std::map<Int, Int> poc2flow;// This map the POC to the optial map's index
	std::map<Int, Int> poc2prev;
	std::vector<cv::Mat> ofMaps;// Each time we calcute a new optical flow, we will push it back to this vector
	std::map<UInt, OFInfo> poc2info;
	
public:
	OData();
	OData(UInt len);
	~OData();
	Float query(UInt idSrc, UInt idDst, UInt dir);
	Void set(UInt curPOC, Float x, Float y);
	Int getPrevPOC();
	Void updateMap(Int curPOC, cv::Mat ofMap);
	OFMap getMap(Int curPOC, Int colPOC);
	UInt picH, picW;
	std::vector<int> shapes;
};

#endif

