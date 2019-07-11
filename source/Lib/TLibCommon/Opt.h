#ifndef __OPT__
#define __OPT__
#include <map>
#include <string>
#include <opencv2\core\core.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "CommonDef.h"
//#include "TComDataCU.h"
#include<fstream>
#include<iostream>
#define X_DIR 0
#define Y_DIR 1
// Here I decide to store the optical map for each two frame!...
// This is the truly bound of our result
struct OptInfo{
	Float fx, fy;
	OptInfo(Float xx, Float yy) : fx(xx), fy(yy) {}
	OptInfo()  {}
};
struct PUPos{
	UInt lx, ly, rx, ry;
	PUPos(UInt lxx, UInt lyy, UInt rxx, UInt ryy) : lx(lxx), ly(lyy), rx(rxx), ry(ryy){}
	PUPos() {}
};
class Opt
{
private:
	std::map<UInt, std::map<UInt, cv::Mat>> POC2Map;
	std::map<UInt, std::map<UInt, Float*>> avgBufferX;
	std::map<UInt, std::map<UInt, Float*>> avgBufferY;
	std::map<UInt, std::map<UInt, Int*>> avgFlag;
	std::map<UInt, std::map<UInt, Float**>> avgPUX;
	std::map<UInt, std::map<UInt, Float**>> avgPUY;
	int lx, ly, rx, ry;
public:
	Opt();
	~Opt();
	template<typename T>Void fill(T** src, T** p, UInt h, UInt w);
	Void updatePU(int ctu, int x, int y, int w, int h);
	PUPos getPU();
	Void updateMap(UInt srcPOC, UInt dstPOC);
	cv::Mat queryMap(UInt srcPOC, UInt dstPOC);
	Void queryFlow(UInt srcPOC, UInt dstPOC, UInt CTU);
	OptInfo queryFlow(UInt srcPOC, UInt dstPOC, UInt lx, UInt ly, UInt rx, UInt ry);
	UInt height, width;
	template<typename T>T** mynew(UInt a, UInt b);
	template<typename T>Void mydelete(T** p, UInt a, UInt b);
	template<typename T>T* mynew(UInt a);
	template<typename T>Void mydelete(T* p, UInt a);
	const UInt ctuPerFrame = 1000;
	// ------------- All below variables should be inited at the very beginning --------------------
	Int picW, picH, curCtu;
	Int curPOC;
	UInt maxCUWidth, maxCUHeight, FrameWidthInCtus;
	// ------------- ---------------------------------------------------------- --------------------
	Float fx, fy;
	//std::map<UInt, TComDataCU**> ctuBak;
	//Void bakCTU(UInt poc, TComDataCU** p, UInt beg, UInt tail);
};
#endif