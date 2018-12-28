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
#include "Natives.h"
//----------------------------------------------------------
// SDK Includes
//----------------------------------------------------------
#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"

extern void *pAMXFunctions;
logprintf_t logprintf;
bool inited = false;
int ticked = 0;
int maxTicked = 10;
std::vector<AMX*> amxList;
Controller *pController;
CA_API *api;
//----------------------------------------------------------
// Main Plugin Data
//----------------------------------------------------------

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




AMX_NATIVE_INFO pathFinderNatives[] =
{
	{ "PF_Init",					Natives::PF_Init },
	{ "PF_Create",					Natives::PF_Create },
	{ "PF_Remove",					Natives::PF_Remove },
	{ "PF_SetPlaneSize",			Natives::PF_SetPlaneSize },
	{ "PF_SetWorld",				Natives::PF_SetWorld },
	{ "PF_SetWallSize",				Natives::PF_SetWallSize },
	{ "PF_SetBeginRelativeCoord",	Natives::PF_SetBeginRelativeCoord },
	{ "PF_SetStart",				Natives::PF_SetStart },
	{ "PF_SetFinalAsPoint",			Natives::PF_SetFinalAsPoint },
	{ "PF_SetFinalAsCircle",		Natives::PF_SetFinalAsCircle },
	{ "PF_SetFinalAsLine",			Natives::PF_SetFinalAsLine },
	{ "PF_Find",					Natives::PF_Find },
	{ "PF_FindNow",					Natives::PF_FindNow },
	{ "PF_GetPoint",				Natives::PF_GetPoint },
	{ "PF_SetTickRate",				Natives::PF_SetTickRate },

	{ "ROAD_Create",				RoadNatives::ROAD_Create },
	{ "ROAD_SetStartNode",			RoadNatives::ROAD_SetStartNode },
	{ "ROAD_SetFinalNode",			RoadNatives::ROAD_SetFinalNode },
	{ "ROAD_Remove",				RoadNatives::ROAD_Remove },
	{ "ROAD_Find",					RoadNatives::ROAD_Find },
	{ "ROAD_FindNow",				RoadNatives::ROAD_FindNow },
	{ "ROAD_GetPoint",				RoadNatives::ROAD_GetPoint },
	{ "ROAD_AddNode",				RoadNatives::ROAD_AddNode },
	{ "ROAD_GetNode",				RoadNatives::ROAD_GetNode },
	{ "ROAD_GetNormalPointToRoad_",	RoadNatives::ROAD_GetNormalPointToRoad },
	{ "ROAD_FindNearbyRoad_",		RoadNatives::ROAD_FindNearbyRoad },
	{ "ROAD_FindInvisibleNode",		RoadNatives::ROAD_FindInvisibleNode },
	{ "ROAD_GetMultipleNode",		RoadNatives::ROAD_GetMultipleNode },
	{ "ROAD_FixRoads",				RoadNatives::ROAD_FixRoads },
	{ "ROAD_Get",					RoadNatives::ROAD_Get },
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
					if (!amx_FindPublic(*a, tempCallbackWorker->name.c_str(), &pathCallbackIndex)) //вызываем callback OnPathCalculated и узнаем его адрес &pathCallbackIndex
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
						amx_Push(*a, amx_ftoc(tempCallbackWorker->genDist));
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