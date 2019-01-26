#pragma once

#include "main.h"


#include <shared_mutex>
namespace API
{
	struct Vector {
	public:
		float x, y, z;
		Vector() {
			Vector(0.0, 0.0, 0.0);
		}
		Vector(float x, float y, float z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}
	};

	struct hitPoint
	{
		Vector pos;
		int modelId;
	};



	//Interface
	class IEColAndreas
	{
	public:
		virtual std::shared_mutex *getMutex() = 0;
		virtual int performRayTest(Vector& pos1, Vector& pos2, Vector& pos, int world) = 0;
		virtual int performRayTestExtraID(Vector& pos1, Vector& pos2, Vector& pos, int type, uint32_t& data, uint16_t& model, int world = 0) = 0;
		virtual int performRayTestID(Vector& pos1, Vector& pos2, Vector& pos, uint32_t& index, int world = 0) = 0;
		virtual int performRayTestAll(Vector& pos1, Vector& pos2, hitPoint *data, int size, int world = 0) = 0;
	};
	typedef IEColAndreas*(*CA_getAPI)();

	//Class to work with the interface
	class CA
	{
	public:
		static IEColAndreas* getInstance() {
			static IEColAndreas  *instance = NULL;
			if (instance == NULL) {
				instance = API::CA::getInstanceAdress();
			}
			return instance;
		}
	private:
		static IEColAndreas* API::CA::getInstanceAdress()
		{
			#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
				std::string name = "ColAndreas.dll";
				HMODULE CA = GetModuleHandle(std::wstring(name.begin(), name.end()).c_str());
				if (CA) {
					CA_getAPI getInterfaceClass = (CA_getAPI)GetProcAddress(CA, "CA_getAPI");
					return getInterfaceClass();
				}
				else {
					logprintf("PathFinder plugin error: could not load ColAndreas.dll. Please, make ColAndreas load before this plugin.");
				}
			#elif defined(LINUX)
				void *CA = dlopen("ColAndreas.so", RTLD_LAZY);
				if (CA) {
					CA_getAPI getInterfaceClass = (CA_getAPI)dlsym(CA, "CA_getAPI");
					return getInterfaceClass();
				else {
					logprintf("PathFinder plugin error: could not load ColAndreas.so. Please, make ColAndreas load before this plugin.");
				}
			#endif

				return NULL;
		}
	};
};