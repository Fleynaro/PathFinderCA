#include "CA_API.h"



CA_API::CA_API()
{
	CA_RayCastLine = CA_API::LoadFunctsFromColAndreas<Type_CA_RayCastLine>("CA_RayCastLine");
	CA_GetMutex = CA_API::LoadFunctsFromColAndreas<Type_CA_GetMutex>("CA_GetMutex");
	if (!CA_RayCastLine || !CA_GetMutex) {
		success = false;
	}
}

template <typename T_funct>
T_funct CA_API::LoadFunctsFromColAndreas(const char *funct)
{
	#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
		std::string name = "ColAndreas.dll";
		HMODULE CA = GetModuleHandle(std::wstring(name.begin(), name.end()).c_str());
		if (CA) {
			return (T_funct)GetProcAddress(CA, funct);
		}
		else {
			logprintf("PathFinder plugin error: could not load ColAndreas.dll. Please, make ColAndreas load before this plugin.");
		}
	#elif defined(LINUX)
		void *CA = dlopen("ColAndreas.so", RTLD_LAZY);
		if (CA) {
			return (T_funct)dlsym(CA, funct);
		else {
			logprintf("PathFinder plugin error: could not load ColAndreas.so. Please, make ColAndreas load before this plugin.");
		}
	#endif
	return NULL;
}

CA_API::~CA_API()
{
}
