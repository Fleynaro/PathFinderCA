//----------------------------------------------------------
//
//
//   	PathFinder Plugin
//                  by Pamdex and Fleynaro
//
//
//	   
//----------------------------------------------------------
//                         INCLUDES
//----------------------------------------------------------
#include "main.h"
#include "path.h"
#include "data.h"
#include "thread.h"
#include "controller.h"
#include "CA_API.h"
//----------------------------------------------------------
// SDK Includes
//----------------------------------------------------------
#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"

extern void *pAMXFunctions;
logprintf_t logprintf;
//----------------------------------------------------------
// Main Plugin Data
//----------------------------------------------------------
bool inited = false;
int ticked = 0;
int maxTicked = 10;
std::vector<AMX*> amxList;
Controller *pController;


int pathCallbackIndex = -1;
cell tickAmxAddress[3] = { NULL };
//----------------------------------------------------------
// Main Plugin Functions
//----------------------------------------------------------
PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() 
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load( void **ppData ) 
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = reinterpret_cast<logprintf_t>(ppData[PLUGIN_DATA_LOGPRINTF]);

	//Message
	logprintf("=========================================");
	logprintf("PathFinderCA Plugin %s", PATHFINDER_VERSION);
	logprintf("         by Pamdex and Fleynaro");
	logprintf("=========================================");
	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	//Unload controller and other stuffs
	
	inited = false; //VERY IMPORTANT !!!

	delete pController;
	logprintf( "PathFinder Plugin %s Unloaded!", PATHFINDER_VERSION);
}

template <typename T_funct>
T_funct LoadFunctsFromColAndreas(const char *funct)
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


// native PathFinder_Init(threads);
cell AMX_NATIVE_CALL N_PF_Init(AMX* amx, cell* params)
{
	if(!inited)
	{
		CA_API *api = new CA_API();
		if (!api->success) {
			logprintf("PathFinder Plugin not loaded.");
			return 0;
		}
		pController = new Controller(api);

		//Start threads
		for(int i = 0; i < params[1]; i++)
		{
			logprintf("PathFinder Plugin -> Creating New Thread");
			pController->StartNewThread();
		}

		inited = true;
	}
	return 1;
}

//PF_Create(callback[], Float: cellSize = 1.5, path_size = 2000, params[] = "", {Float,_}:...)
cell AMX_NATIVE_CALL N_PF_Create(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		char *name;
		amx_StrParam(amx, params[1], name);
		int id = pController->CreatePath(name);
		if ( id == 0 ) {
			return 0;
		}

		Path *path = pController->GetPath(id);
		path->cellSize = amx_ctof(params[2]);
		path->PATH_SIZE = params[3];

		char *format;
		amx_StrParam(amx, params[4], format);
		if (format) {
			std::deque<cell> deq_params;
			cell *addr_params;

			const int offset = 5;
			for (int i = strlen(format); --i != -1;) {
				switch (format[i])
				{
				case 'b':
				case 'B':
				case 'c':
				case 'C':
				case 'd':
				case 'D':
				case 'i':
				case 'I':
				case 'f':
				case 'F':
					amx_GetAddr(amx, params[offset + i], &addr_params);
					deq_params.push_back(*addr_params);
					break;
				case 's':
				case 'S':
					deq_params.push_back(params[offset + i]);
					break;
				default:
					logprintf("[PathFinder] Unknown format specifer: '%c'", format[i]);
				}
			}
			path->params = deq_params;
		}
		return id;
	}
}
//PF_Remove(id)
cell AMX_NATIVE_CALL N_PF_Remove(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		pController->RemovePath(id);
		return 1;
	}
}
//PF_SetPlaneSize(id, width, height)
cell AMX_NATIVE_CALL N_PF_SetPlaneSize(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		Path *path = pController->GetPath(id);
		path->SetPlaneSize(params[2], params[3]);
		return 1;
	}
}
//PF_SetWorld(id, world)
cell AMX_NATIVE_CALL N_PF_SetWorld(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		Path *path = pController->GetPath(id);
		path->SetWorld(params[2]);
		return 1;
	}
}
//PF_SetWallSize(id, Float:up, Float:down)
cell AMX_NATIVE_CALL N_PF_SetWallSize(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		Path *path = pController->GetPath(id);
		path->WALL_UP = amx_ctof(params[2]);
		path->WALL_DOWN = amx_ctof(params[3]);
		return 1;
	}
}
//PF_SetBeginRelativeCoord(id, Float:x, Float:y, width = 10, height = 10)
cell AMX_NATIVE_CALL N_PF_SetBeginRelativeCoord(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		Path *path = pController->GetPath(id);
		path->SetBeginRelativeCoord(amx_ctof(params[2]), amx_ctof(params[3]), params[4], params[5]);
		return 1;
	}
}
//PF_SetStart(id, Float:startX, Float:startY, Float:startZ)
cell AMX_NATIVE_CALL N_PF_SetStart(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		Path *path = pController->GetPath(id);
		path->startX = amx_ctof(params[2]);
		path->startY = amx_ctof(params[3]);
		path->startZ = amx_ctof(params[4]);
		//logprintf("[PathFinder] path->start = %f(%f),%f,%f", path->startX, amx_ctof(params[2]), path->startY, path->startZ);
		return 1;
	}
}
//PF_SetFinalAsPoint(id, Float:endX, Float:endY, Float:endZ)
cell AMX_NATIVE_CALL N_PF_SetFinalAsPoint(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		Path *path = pController->GetPath(id);
		path->type = PATH_TYPE_POINT;
		path->endX = amx_ctof(params[2]);
		path->endY = amx_ctof(params[3]);
		path->endZ = amx_ctof(params[4]);
		return 1;
	}
}
//PF_SetFinalAsCircle(id, Float:endX, Float:endY, Float:radius)
cell AMX_NATIVE_CALL N_PF_SetFinalAsCircle(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		Path *path = pController->GetPath(id);
		path->type = PATH_TYPE_CIRCLE;
		path->endX = amx_ctof(params[2]);
		path->endY = amx_ctof(params[3]);
		path->endZ = amx_ctof(params[4]);
		return 1;
	}
}
//PF_SetFinalAsLine(id, Float:lineX, Float:lineY, Float:lineAngle)
cell AMX_NATIVE_CALL N_PF_SetFinalAsLine(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		Path *path = pController->GetPath(id);
		path->type = PATH_TYPE_LINE;
		//to do
		return 1;
	}
}

//PF_Find(id)
cell AMX_NATIVE_CALL N_PF_Find(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}

		//Lock
		pController->workQueue->lock();

		//Send (Добавляем в очередь)
		pController->qPath->push(pController->GetPath(id));
		//logprintf("name %s(%i)", pController->GetPath(id)->callback, id);

		//Unlock
		pController->workQueue->unlock();

		pController->RemovePath(id);
		return 1;
	}
}

//PF_FindNow(id, &code, &Float:nodesX[], &Float:nodesY[], &Float:nodesZ[], &nodesSize)
cell AMX_NATIVE_CALL N_PF_FindNow(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		Path *path = pController->GetPath(id);
		path->Find();

		/*if (path->status == PATH_FOUND)
		{
			cell *pAddress = NULL;
			amx_GetAddr(amx, params[2], &pAddress);
			*pAddress = PATH_FOUND;

			std::vector<cell> *nodeX, *nodeY, *nodeZ;
			while (!path->pathData->empty())
			{
				float tempX = path->pathData->back()->x;
				float tempY = path->pathData->back()->y;
				float tempZ = path->pathData->back()->z;
				nodeX->push_back(amx_ftoc(tempX));
				nodeY->push_back(amx_ftoc(tempY));
				nodeZ->push_back(amx_ftoc(tempZ));
				path->pathData->pop_back();
			}

			cell *rawDataZ = new cell[nodeX->size() + 1];//создаем массивы
			cell *rawDataY = new cell[nodeX->size() + 1];
			cell *rawDataX = new cell[nodeX->size() + 1];
			copy(nodeZ->begin(), nodeZ->end(), rawDataZ);//копируем в эти массивы данные из tempCallbackWorker->node
			copy(nodeY->begin(), nodeY->end(), rawDataY);
			copy(nodeX->begin(), nodeX->end(), rawDataX);

			amx_GetAddr(amx, params[3], &pAddress);
			*pAddress = *rawDataX;
			amx_GetAddr(amx, params[4], &pAddress);
			*pAddress = *rawDataY;
			amx_GetAddr(amx, params[5], &pAddress);
			*pAddress = *rawDataZ;
			amx_GetAddr(amx, params[6], &pAddress);
			*pAddress = nodeX->size() + 1;
		}
		else {
			cell *pAddress = NULL;
			amx_GetAddr(amx, params[2], &pAddress);
			*pAddress = path->status;
		}
		delete path;*/
		if (path->status == PATH_FOUND && !path->pathData->empty()) {
			return 1;
		}
		else {
			path->Destroy();
			pController->RemovePath(id);
			return 0;
		}
	}
}

//PF_GetPoint(id, &Float: x, &Float: y, &Float: z)
cell AMX_NATIVE_CALL N_PF_GetPoint(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		Path *path = pController->GetPath(id);

		if (path->status != PATH_FOUND || path->pathData->empty()) {
			path->Destroy();
			pController->RemovePath(id);
			return 0;
		}

		cell *pAddress = NULL;
		amx_GetAddr(amx, params[2], &pAddress);
		*pAddress = amx_ftoc(path->pathData->back()->x);
		amx_GetAddr(amx, params[3], &pAddress);
		*pAddress = amx_ftoc(path->pathData->back()->y);
		amx_GetAddr(amx, params[4], &pAddress);
		*pAddress = amx_ftoc(path->pathData->back()->z);
		path->pathData->pop_back();
		return 1;
	}
}

// native PathFinder_SetTickRate(rate = 10);
cell AMX_NATIVE_CALL N_PF_SetTickRate(AMX* amx, cell* params)
{
	ticked = 0;
	if(params[1] <= 0) maxTicked = 1;
	else maxTicked = params[1];
	return 1;
}

AMX_NATIVE_INFO pathFinderNatives[] =
{
	{ "PF_Init",					N_PF_Init },
	{ "PF_Create",					N_PF_Create },
	{ "PF_Remove",					N_PF_Remove },
	{ "PF_SetPlaneSize",			N_PF_SetPlaneSize },
	{ "PF_SetWorld",				N_PF_SetWorld },
	{ "PF_SetWallSize",				N_PF_SetWallSize },
	{ "PF_SetBeginRelativeCoord",	N_PF_SetBeginRelativeCoord },
	{ "PF_SetStart",				N_PF_SetStart },
	{ "PF_SetFinalAsPoint",			N_PF_SetFinalAsPoint },
	{ "PF_SetFinalAsCircle",		N_PF_SetFinalAsCircle },
	{ "PF_SetFinalAsLine",			N_PF_SetFinalAsLine },
	{ "PF_Find",					N_PF_Find },
	{ "PF_FindNow",					N_PF_FindNow },
	{ "PF_GetPoint",				N_PF_GetPoint },
	{ "PF_SetTickRate",				N_PF_SetTickRate },
	{ 0,                        0 }
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad( AMX *amx ) 
{
	amxList.push_back(amx);
	return amx_Register(amx, pathFinderNatives, -1);
}


PLUGIN_EXPORT int PLUGIN_CALL AmxUnload( AMX *amx ) 
{
	for(std::vector<AMX*>::iterator i = amxList.begin(); i != amxList.end();++i)
	{
		if(*i == amx)
		{
			amxList.erase(i);
			break;
		}
	}
	return AMX_ERR_NONE;
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
	if(ticked == maxTicked) //Работа с потоком
	{
		if (inited)
		{
			/*float x, y, z; int r;
			r = g_Invoke->CA_RayCastLine(-2043.0, -84.0, 38.0, -2043.0, -88.0, 0.0, &x, &y, &z);
			logprintf("TICKKK x,y,z = %i, %f %f %f", r, x, y, z);*/

			//Lock - creating data for callback is very fast but...
			pController->callbackQueue->lock();
			
			//Send data to PAWN :)
			while (!pController->qCallback->empty())
			{
				//logprintf("New data appeared to send ones to callback");
				callbackWorkerData *tempCallbackWorker = pController->qCallback->front(); //первый в очереди
				cell * rawDataZ;
				cell * rawDataY;
				cell * rawDataX;
				
				//logprintf("---> %f,%f", amx_ctof(tempCallbackWorker->nodeX->front()), amx_ctof(tempCallbackWorker->nodeZ->front()));

				//Prepare data
				rawDataZ = new cell[tempCallbackWorker->nodeX->size() + 1];//создаем массивы
				rawDataY = new cell[tempCallbackWorker->nodeX->size() + 1];
				rawDataX = new cell[tempCallbackWorker->nodeX->size() + 1];
				copy(tempCallbackWorker->nodeZ->begin(), tempCallbackWorker->nodeZ->end(), rawDataZ);//копируем в эти массивы данные из tempCallbackWorker->node
				copy(tempCallbackWorker->nodeY->begin(), tempCallbackWorker->nodeY->end(), rawDataY);
				copy(tempCallbackWorker->nodeX->begin(), tempCallbackWorker->nodeX->end(), rawDataX);

				//logprintf("(%i)---> %i, %i", tempCallbackWorker->extraValues[0], sizeof(tempCallbackWorker->extraValues), sizeof(tempCallbackWorker->extraValues) / sizeof(*tempCallbackWorker->extraValues));

				//Send data
				for (std::vector<AMX *>::iterator a = amxList.begin(); a != amxList.end(); ++a)
				{
					//tempCallbackWorker->name заместо OnPathCalculated
					if (!amx_FindPublic(*a, tempCallbackWorker->name, &pathCallbackIndex)) //вызываем callback OnPathCalculated и узнаем его адрес &pathCallbackIndex
					{
						for (const auto params : tempCallbackWorker->params)
							if (amx_Push(*a, params) != AMX_ERR_NONE)
								return;

						//nodesSize
						amx_Push(*a, (int)tempCallbackWorker->nodeX->size()); //Добавляем размер tempCallbackWorker->node

						//nodesX, nodesY, nodesZ
						amx_PushArray(*a, &tickAmxAddress[2], 0, rawDataZ, tempCallbackWorker->nodeX->size() + 1); //добавляем массив
						amx_PushArray(*a, &tickAmxAddress[1], 0, rawDataY, tempCallbackWorker->nodeX->size() + 1);
						amx_PushArray(*a, &tickAmxAddress[0], 0, rawDataX, tempCallbackWorker->nodeX->size() + 1);

						//other data
						amx_Push(*a, tempCallbackWorker->resultCode);
						amx_Exec(*a, NULL, pathCallbackIndex);

						//free
						amx_Release(*a, tickAmxAddress[0]);
						amx_Release(*a, tickAmxAddress[1]);
						amx_Release(*a, tickAmxAddress[2]);
					}
				}
				//удаляем все ненужное
				delete rawDataX;
				delete rawDataY;
				delete rawDataZ;
				delete pController->qCallback->front()->nodeX;
				delete pController->qCallback->front()->nodeY;
				delete pController->qCallback->front()->nodeZ;
				delete tempCallbackWorker;
				pController->qCallback->pop(); //следующий в очереди
			}

			//Unlock
			pController->callbackQueue->unlock();
		}
		//-------
		ticked = 0;
	}
	ticked++;
}