#include "Opt.h"


Opt::Opt()
{
}


Opt::~Opt()
{
}
template<typename T>Void Opt::fill(T** src, T** p, UInt h, UInt w) {
	p[0][0] = src[0][0];
	for (UInt i = 1; i < w; ++i)
	{
		p[0][i] = p[0][i - 1] + src[0][i];
	}
	for (UInt i = 1; i < h; ++i)
	{
		p[i][0] = p[i - 1][0] + src[i][0];
	}
	for (UInt i = 1; i < h; ++i)
	{
		for (UInt j = 1; j < w; ++j)
		{
			p[i][j] = p[i - 1][j] + p[i][j - 1] - p[i - 1][j - 1] + src[i][j];
		}
	}
	return;
}

Void Opt::updateMap(UInt srcPOC, UInt dstPOC)
{
	// We only store the map from before POC to the after POC
	assert(srcPOC < dstPOC);
	// First check if we have already have the optical info
	if (POC2Map.find(srcPOC) != POC2Map.end() && POC2Map[srcPOC].find(dstPOC) != POC2Map[srcPOC].end())
	{
		// We already have it and don't have to calculate it again!
		assert(0);
		return;
	}
	// else we have to calculate it...
	if (POC2Map.find(srcPOC) == POC2Map.end())
	{
		POC2Map[srcPOC] = std::map < UInt, cv::Mat > ();
		//avgBufferX[srcPOC] = std::map<UInt, Float*>();
		//avgBufferY[srcPOC] = std::map<UInt, Float*>();
		//avgFlag[srcPOC] = std::map<UInt, Int*>();
		avgPUX[srcPOC] = std::map<UInt, Float**>();
		avgPUY[srcPOC] = std::map<UInt, Float**>();
	}
	// Then begin to calculate the optical flow
	std::string srcName = std::to_string(srcPOC) + ".png ", dstName = std::to_string(dstPOC) + ".png ";
	std::string modelName = "pwcnet.ckpt-595000";
	std::string cmd_string = "D:\\hyz\\Anaconda3\\envs\\pwc\\python.exe tfoptflow\\pwcnet_predict_from_img_pairs.py ";
	cmd_string += srcName;
	cmd_string += dstName;
	cmd_string += modelName;
	cmd_string += " 2>&1";
	//FILE* fp = _popen(cmd_string.c_str(), "r");
	FILE* fp = _popen(cmd_string.c_str(), "r");
	char buf[255];
	while (fgets(buf, 255, fp) != NULL)
	{
		//std::cout << buf;
	}
	_pclose(fp);
	Float Fx, Fy;
	//Then we should use the optical file to store
	std::string xFlowName = "x.flow";
	std::string yFlowName = "y.flow";
	std::ifstream xFin(xFlowName);
	std::ifstream yFin(yFlowName);
	cv::Mat opMap = cv::Mat(height, width, CV_32FC3, cv::Scalar(0));// In fact we only use the first channel

	Float** srcx = mynew<Float>(height, width);
	Float** srcy = mynew<Float>(height, width);
	for (UInt i = 0; i < height; ++i)
	{
		for (UInt j = 0; j < width; ++j)
		{
			xFin >> Fx;
			yFin >> Fy;
			opMap.at<cv::Vec3f>(i, j)[0] = Fx;
			opMap.at<cv::Vec3f>(i, j)[1] = Fy;
			srcx[i][j] = Fx;
			srcy[i][j] = Fy;
		}
	}
	xFin.close();
	yFin.close();
	
	Float** px = mynew<Float>(height, width);
	Float** py = mynew<Float>(height, width);
	//Int* ip = mynew<Int>(ctuPerFrame);


	// Init the array part
	POC2Map[srcPOC][dstPOC] = opMap;
	avgPUX[srcPOC][dstPOC] = px;
	avgPUY[srcPOC][dstPOC] = py;
	// And finally init the px and py array
	fill(srcx, px, height, width);
	fill(srcy, py, height, width);
	mydelete(srcx, height, width);
	mydelete(srcy, height, width);
	curPOC = dstPOC;
	return;
}

cv::Mat Opt::queryMap(UInt srcPOC, UInt dstPOC)
{
	assert(srcPOC < dstPOC);
	// I have to promise that we have this Map, but here I don't check this point...
	updateMap(srcPOC, dstPOC);
	if (srcPOC < dstPOC)
	{
		return POC2Map[srcPOC][dstPOC];
	}
	else
	{
		return -POC2Map[dstPOC][srcPOC];
	}
}

Void Opt::queryFlow(UInt srcPOC, UInt dstPOC, UInt CTU)
{
	assert(srcPOC < dstPOC);
	// Here we should not use this function
	assert(0);
	Float curAvgX = 0.0, curAvgY = 0.0, colAvgX = 0.0, colAvgY = 0.0;
	if (POC2Map.find(srcPOC) != POC2Map.end() && POC2Map[srcPOC].find(dstPOC) != POC2Map[srcPOC].end())
	{
		// We already have its optical flow map
		if (avgFlag[srcPOC][dstPOC][CTU] == 1)
		{
			fx = avgBufferX[srcPOC][dstPOC][CTU];
			fy = avgBufferY[srcPOC][dstPOC][CTU];
			return;
		}
		else
		{
			// We will calculate it and then store it...
			cv::Mat curOF = queryMap(srcPOC, dstPOC);
			Int Xlow, Ylow, Xup, Yup;
			Xlow = (CTU % FrameWidthInCtus) * maxCUWidth;
			Ylow = (CTU / FrameWidthInCtus) * maxCUHeight;
			// Later remember to init the X/Y low
			/*Xlow = globalOData.curX;
			Ylow = globalOData.curY;*/
			Xup = Xlow + 64;
			Yup = Ylow + 64;
			if (Xup > picW)
			{
				Xup = picW;
			}
			if (Yup > picH)
			{
				Yup = picH;
			}
			for (UInt i = Xlow; i < Xup; ++i)
			{
				for (UInt j = Ylow; j < Yup; ++j)
				{
					curAvgX += curOF.at<cv::Vec3f>(j, i)[0];
					curAvgY += curOF.at<cv::Vec3f>(j, i)[1];
				}
			}
			curAvgX /= ((Xup - Xlow) * (Yup - Ylow));
			curAvgY /= ((Xup - Xlow) * (Yup - Ylow));
			// And then is the buffer
			avgBufferX[srcPOC][dstPOC][CTU] = curAvgX;
			avgBufferY[srcPOC][dstPOC][CTU] = curAvgY;
			avgFlag[srcPOC][dstPOC][CTU] = 1;
			
			fx = avgBufferX[srcPOC][dstPOC][CTU];
			fy = avgBufferY[srcPOC][dstPOC][CTU];
			//std::cout << srcPOC << " " << " " << dstPOC << " " << Xlow << " " << Ylow << " " << fx << " " << fy << std::endl;
			return;
		}
	}
	else
	{
		// Here I assert(0) to exit the program... Before I want that each time I call queryFlow, We already calculated it
		assert(0);
	}
}

Void Opt::updatePU(int ctu, int x, int y, int w, int h)
{
	//lx = (ctu % FrameWidthInCtus) * maxCUHeight + x;
	//ly = (ctu / FrameWidthInCtus) * maxCUHeight + y;
	lx = x;
	ly = y;
	rx = lx + w;
	ry = ly + h;
	curCtu = ctu;
}
PUPos Opt::getPU()
{
	return PUPos(lx, ly, rx, ry);
}

OptInfo Opt::queryFlow(UInt srcPOC, UInt dstPOC, UInt lx, UInt ly, UInt rx, UInt ry)
{
	assert(srcPOC < dstPOC);
	Float curAvgX = 0.0, curAvgY = 0.0, colAvgX = 0.0, colAvgY = 0.0;
	if (POC2Map.find(srcPOC) != POC2Map.end() && POC2Map[srcPOC].find(dstPOC) != POC2Map[srcPOC].end())
	{
		// If we have finish calculate the OPT flow map, we can be confidence that we have finish initig the sum matrix
		Float** px = avgPUX[srcPOC][dstPOC];
		Float** py = avgPUY[srcPOC][dstPOC];
		Float factor = (ry - ly) * (rx - lx);
		return OptInfo((px[ry - 1][rx - 1] - px[ry - 1][lx] - px[ly][rx - 1] + px[ly][lx]) / factor, (py[ry - 1][rx - 1] - py[ry - 1][lx] - py[ly][rx - 1] + py[ly][lx]) / factor);
	}
	else
	{
		// Here I assert(0) to exit the program... Before I want that each time I call queryFlow, We already calculated it
		assert(0);
	}
}
template<typename T>T** Opt::mynew(UInt a, UInt b)
{
	T** ret = new T*[a];
	for (int i = 0; i < a; ++i)
	{
		ret[i] = new T[b];
		memset(ret[i], 0, b * sizeof(T));
	}
	return ret;
}
template<typename T>Void Opt::mydelete(T** p, UInt a, UInt b)
{
	for (int i = 0; i < a; ++i)
	{
		delete[] p[i];
	}
	delete[]p;
}
template<typename T>T* Opt::mynew(UInt a)
{
	T* ret = new T[a];
	memset(ret, 0, a * sizeof(T));
	return ret;
}
template<typename T>Void Opt::mydelete(T* p, UInt a)
{
	delete[]p;
}