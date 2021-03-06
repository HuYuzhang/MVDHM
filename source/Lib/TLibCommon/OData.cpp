#include "OData.h"


OData::OData()
{
	Ox.clear();
	Oy.clear();
	avaFlag.clear();
	prev.clear();
	POCs.clear();
	POCs.push_back(0);
	prevPOC = 0;
	poc2flow.clear();
	ofMaps.clear();
	poc2prev.clear();
	poc2info.clear();
	
	prevPOC = 0;
	OFInfo tmp;
	tmp.curPOC = 0;
	tmp.prevPOC = -1;
	tmp.curIndex = 0;
	tmp.opIndex = -1;
	poc2info.insert(std::pair<Int, OFInfo>(0, tmp));
}

OData::OData(UInt len)
{
	Ox.clear();
	Oy.clear();
	avaFlag.clear();
	POCs.clear();
	
	for (UInt i = 0; i < len; ++i)
	{
		Ox.push_back(0.0);
		Oy.push_back(0.0);
		avaFlag.push_back(0);
		prev.push_back(-1);
	}
	POCs.push_back(0);
	prevPOC = 0;
	Ox[0] = 0.0;
	Oy[0] = 0.0;
	avaFlag[0] = 1;
	poc2flow.clear();
	ofMaps.clear();
	poc2prev.clear();
	poc2info.clear();
	
	prevPOC = 0;
	OFInfo tmp;
	tmp.curPOC = 0;
	tmp.prevPOC = -1;
	tmp.curIndex = 0;
	tmp.opIndex = -1;
	poc2info.insert(std::pair<Int, OFInfo>(0, tmp));
}

OData::~OData()
{
}
// should not be used, for its poor performance
Float OData::query(UInt idSrc, UInt idDst, UInt dir)
{
	UInt cur_len = Ox.size();
	if (cur_len < idSrc || cur_len < idDst)
	{
		printf("Wrong! Try to get unknown optical flow(Array out of index)\n");
		exit(1);
	}
	// Then check if the available flag is valid
	if (avaFlag[idDst] == 0 || avaFlag[idSrc] == 0)
	{
		printf("Wrong! Try to get unknown optical flow(Haven't be calculated)\n");
		exit(1);
	}
	// Then ok, we have our data~
	// And we have to find the which frame is encoed earlier
	if (idSrc == idDst)
	{
		return 0;
	}
	UInt POC_len = POCs.size();
	UInt earlyPOC, latePOC;
	for (UInt i = POC_len - 1; i >= 0; --i)
	{
		if (i == idSrc)
		{
			earlyPOC = idDst; 
			latePOC = idSrc;
			break;
		}
		else if (i == idDst)
		{
			earlyPOC = idSrc; 
			latePOC = idDst;
			break;
		}
	}
	Float retX = 0.0;
	Float retY = 0.0;
	UInt cursor = latePOC;
	while (cursor != earlyPOC)
	{
		retX += Ox[cursor];
		retY += Oy[cursor];
		cursor = prev[cursor];
	}
	switch (dir){
	case X_DIR:
		return retX;
	case Y_DIR:
		return retY;
	default:
		printf("Wrong! Try to get rather X nor Y direction's OF\n");
		exit(1);
	}
}

Void OData::set(UInt curPOC, Float x, Float y)
{
	UInt cur_len = Ox.size();
	if (cur_len < curPOC)
	{
		printf("Wrong! Try to get unknown optical flow(Array out of index)\n");
		exit(1);
	}
	Ox[curPOC] = x;
	Oy[curPOC] = y;
	avaFlag[curPOC] = 1;
	prev[curPOC] = this->prevPOC;
	POCs.push_back(curPOC);
	this->prevPOC = curPOC;
}
Int OData::getPrevPOC()
{
	return this->prevPOC;
}
// should not be used, for its poor performance
Void OData::updateMap(Int curPOC, cv::Mat ofMap)
{
	
	/*poc2prev.insert(std::pair<Int, Int>(curPOC, this->prevPOC));
	poc2flow.insert(std::pair<Int, Int>(curPOC, ofMaps.size() - 1));*/// We can use the index to get the ofMaps
	OFInfo tmp;
	tmp.curPOC = curPOC;
	tmp.prevPOC = this->prevPOC;
	tmp.curIndex = poc2info.size();
	tmp.opIndex = ofMaps.size();
	ofMaps.push_back(ofMap.clone());
	poc2info.insert(std::pair<Int, OFInfo>(curPOC, tmp));
	this->prevPOC = curPOC;
}

OFMap OData::getMap(Int curPOC, Int colPOC)// I find that the name for the param is not precise...it should be curRefPOC! rather than colPOC
{
	//// We have to desice which is the first encoded frames
	//// frist we have to get frame-level OF map between the two frames
	//Int tmpPrev = poc2prev[curPOC];
	//Int early = -1;
	//Int late = -1;
	//while (1)
	//{
	//	if (tmpPrev == colPOC)
	//	{
	//		early = colPOC;
	//		late = curPOC;
	//		break;
	//	}
	//	if (tmpPrev == 0)
	//	{
	//		break;
	//	}
	//	tmpPrev = poc2prev[tmpPrev];
	//}
	//if (early == -1 && late == -1)// We have to keep seeking
	//{
	//	tmpPrev = poc2prev[colPOC];
	//	while (1)
	//	{
	//		if (tmpPrev == curPOC)
	//		{
	//			early = curPOC;
	//			late = colPOC;
	//			break;
	//		}
	//		if (tmpPrev == 0)
	//		{
	//			break;
	//		}
	//		tmpPrev = poc2prev[tmpPrev];
	//	}
	//}
	//assert(early != -1 && late != -1);
	//// Then we get the POC index and then begin to conduct a OFMap from early to late
	//tmpPrev = poc2prev[late];
	//UInt mapIndex = poc2flow[late];
	//OFMap opMap = ofMaps[mapIndex].clone();

	//while (tmpPrev != early)
	//{

	//}
	assert(curPOC != colPOC);
	
	OFInfo cur = poc2info[curPOC];
	OFInfo col = poc2info[colPOC];
	Int early = -1, late = -1;
	// We make assert because we promise that the curPOC is always the late!
	assert(cur.curIndex > col.curIndex);
	// So the below if block is in fact of no use?
	if (cur.curIndex < col.curIndex)
	{
		early = curPOC;
		late = colPOC;
	}
	else
	{
		early = colPOC;
		late = curPOC;
	}
	
	assert(early != -1 && late != -1);
	// Begin the buffer
	cv::Mat buffer;
	if (mapBuffer.find(late) != mapBuffer.end()  && mapBuffer[late].find(early) != mapBuffer[late].end())
	{
		assert(avgFlag.find(late) != avgFlag.end()  && avgFlag[late].find(early) != avgFlag[late].end());
		assert(avgBufferX.find(late) != avgBufferX.end()  && avgBufferX[late].find(early) != avgBufferX[late].end());
		assert(avgBufferY.find(late) != avgBufferY.end()  && avgBufferY[late].find(early) != avgBufferY[late].end());
		buffer= mapBuffer[late][early];
		if (cur.curIndex < col.curIndex)
		{
			return -buffer;
		}
		else
		{
			return buffer;
		}
	}
	else
	{
		// Finish the buffer, if no, we will calculate it and then buffer it
		// I believe that will be entered for just one time! so just assert it
		OFInfo tmpinfo = poc2info[late];
		cv::Mat sum = cv::Mat(picH, picW, CV_32FC3, cv::Scalar(0));
		while (tmpinfo.curPOC != early)
		{
			cv::add(sum, ofMaps[tmpinfo.opIndex], sum);
			tmpinfo = poc2info[tmpinfo.prevPOC];
		}
		// Begin to buffer the map
		mapBuffer[late][early] = sum;
		// What's more, we should also update the avgBuffer
		Float* px = mynew<Float>(ctuPerFrame);
		Float* py = mynew<Float>(ctuPerFrame);
		Int* ip = mynew<Int>(ctuPerFrame);
		avgBufferX[late][early] = px;
		avgBufferY[late][early] = py;
		avgFlag[late][early] = ip;
		std::string tail_png = std::to_string(late) + "_" + std::to_string(early) + std::string(".png");
		std::string tail_txt = std::to_string(late) + "_" + std::to_string(early) + std::string(".txt");
		//ODDump(sum, std::string("xdiffop/") + tail_png, std::string("ydiffop/") + tail_png);
		//ODDump2(sum, std::string("xdiffop2/") + tail_txt, std::string("ydiffop2/") + tail_txt);
		// Now begin to calculate the average flow of each CTU
		// End the buffering
		if (cur.curIndex < col.curIndex)
		{
			sum = -sum;
		}

		return sum;
	}
}
Void OData::ODDump(cv::Mat tmpM)
{

	cv::Mat dumpMap = cv::Mat(picH, picW, CV_32FC3, cv::Scalar(100, 100, 100));
	cv::scaleAdd(tmpM, 5.0, dumpMap, dumpMap);
	cv::Mat dumpOK = cv::Mat(picH, picW, CV_8UC1);
	for (UInt i = 0; i < picH; ++i)
	{
		for (UInt j = 0; j < picW; ++j)
		{

			dumpOK.at<unsigned char>(i, j) = (unsigned char)dumpMap.at<cv::Vec3f>(i, j)[0];
			/*unsigned char tmp = dumpOK.at<unsigned char>(i, j);
			cout << (int)dumpOK.at<unsigned char>(i, j) << endl;*/
		}
	}
	cv::imwrite("ODDump/dumpX.png", dumpOK);
}

Void OData::ODDump(cv::Mat tmpM, std::string fName1, std::string fName2)
{

	cv::Mat dumpMap = cv::Mat(picH, picW, CV_32FC3, cv::Scalar(100, 100, 100));
	cv::scaleAdd(tmpM, 1.0, dumpMap, dumpMap);
	cv::Mat dumpOK = cv::Mat(picH, picW, CV_8UC1);
	for (UInt i = 0; i < picH; ++i)
	{
		for (UInt j = 0; j < picW; ++j)
		{

			dumpOK.at<unsigned char>(i, j) = (unsigned char)dumpMap.at<cv::Vec3f>(i, j)[0];
			/*unsigned char tmp = dumpOK.at<unsigned char>(i, j);
			cout << (int)dumpOK.at<unsigned char>(i, j) << endl;*/
		}
	}
	cv::imwrite(fName1, dumpOK);
	for (UInt i = 0; i < picH; ++i)
	{
		for (UInt j = 0; j < picW; ++j)
		{

			dumpOK.at<unsigned char>(i, j) = (unsigned char)dumpMap.at<cv::Vec3f>(i, j)[1];
			/*unsigned char tmp = dumpOK.at<unsigned char>(i, j);
			cout << (int)dumpOK.at<unsigned char>(i, j) << endl;*/
		}
	}
	cv::imwrite(fName2, dumpOK);
}
Void OData::ODDump2(cv::Mat tmpM, std::string fName1, std::string fName2)
{

	std::ofstream f(fName1);
	for (UInt i = 0; i < picH; ++i)
	{
		for (UInt j = 0; j < picW; ++j)
		{
			f << tmpM.at<cv::Vec3f>(i, j)[0] << " ";
			/*unsigned char tmp = dumpOK.at<unsigned char>(i, j);
			cout << (int)dumpOK.at<unsigned char>(i, j) << endl;*/
		}
		f << std::endl;
	}
	f.close();
	f.open(fName2);
	for (UInt i = 0; i < picH; ++i)
	{
		for (UInt j = 0; j < picW; ++j)
		{

			f << tmpM.at<cv::Vec3f>(i, j)[1] << " ";
			/*unsigned char tmp = dumpOK.at<unsigned char>(i, j);
			cout << (int)dumpOK.at<unsigned char>(i, j) << endl;*/
		}
		f << std::endl;
	}
	f.close();
}
template<typename T>T** OData::mynew(UInt a, UInt b)
{
	T** ret = new T*[a];
	for (int i = 0; i < a; ++i)
	{
		ret[i] = new T[b];
		memset(ret[i], 0, b * sizeof(T));
	}
	return ret;
}
template<typename T>Void OData::mydelete(T** p, UInt a, UInt b)
{
	for (int i = 0; i < a; ++i)
	{
		delete[] p[a];
	}
	delete[]p;
}
template<typename T>T* OData::mynew(UInt a)
{
	T* ret = new T[a];
	memset(ret, 0, a * sizeof(T));
	return ret;
}
template<typename T>Void OData::mydelete(T* p, UInt a)
{
	delete[]p;
}