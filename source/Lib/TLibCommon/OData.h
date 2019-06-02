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
	
public:
	std::map<Int, std::map<Int, cv::Mat>> mapBuffer;
	std::map<Int, std::map<Int, Float*>> avgBufferX;
	std::map<Int, std::map<Int, Float*>> avgBufferY;
	std::map<Int, std::map<Int, Int*>> avgFlag;
	std::map<UInt, OFInfo> poc2info;
	Int prevPOC;
	std::map<Int, Int> poc2flow;// This map the POC to the optial map's index
	std::map<Int, Int> poc2prev;
	std::vector<cv::Mat> ofMaps;// Each time we calcute a new optical flow, we will push it back to this vector
	const Float avgThres = 0.2;


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
	Int curX, curY, colX, colY;
	Int curPOC, curRefPOC, colPOC, colRefPOC;
	Int curCTU, colCTU;
	Void ODDump(cv::Mat tmpM);
	Float XScale;
	Float YScale;
	Float HMScale;
	UInt ctuPerFrame = 1000;
	template<typename T>T** mynew(UInt a, UInt b);
	template<typename T>Void mydelete(T** p, UInt a, UInt b);
	template<typename T>T* mynew(UInt a);
	template<typename T>Void mydelete(T* p, UInt a);
};

#endif

