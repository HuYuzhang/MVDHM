#ifndef __ODATA__
#define __ODATA__

#include "CommonDef.h"
#include <vector>

#define X_DIR 0
#define Y_DIR 1
class OData
{
private:
	std::vector<Float> Ox;
	std::vector<Float> Oy;
	std::vector<UInt> avaFlag;
public:
	OData();
	OData(UInt len);
	~OData();
	Float query(UInt idSrc, UInt idDst, UInt dir);
};

#endif

