

#include "main.h"


//import functions from ColAndreas plugin
typedef int(*Type_CA_RayCastLine)(float, float, float, float, float, float, float &, float &, float &, int);
typedef std::mutex*(*Type_CA_GetMutex)();

#pragma once
class CA_API
{
public:
	CA_API();
	~CA_API();

	Type_CA_RayCastLine CA_RayCastLine;
	Type_CA_GetMutex CA_GetMutex;

	template <typename T_funct>
	T_funct LoadFunctsFromColAndreas(const char *funct);

	bool success = true;
private:
	
};

