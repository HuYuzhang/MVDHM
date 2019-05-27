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
}

OData::~OData()
{
}

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