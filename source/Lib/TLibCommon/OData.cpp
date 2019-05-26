#include "OData.h"


OData::OData()
{
	Ox.clear();
	Oy.clear();
	avaFlag.clear();
}

OData::OData(UInt len)
{
	Ox.clear();
	Oy.clear();
	avaFlag.clear();
	for (UInt i = 0; i < len; ++i)
	{
		Ox.push_back(0.0);
		Oy.push_back(0.0);
		avaFlag.push_back(0);
	}
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
	Float ret = 0.0;
	switch (dir){
	case X_DIR:
		ret = Ox[idDst] - Ox[idSrc];
		break;
	case Y_DIR:
		ret = Oy[idDst] - Oy[idSrc];
		break;
	default:
		printf("Wrong! Try to get rather X nor Y direction's OF\n");
		exit(1);
	}
	return ret;
}