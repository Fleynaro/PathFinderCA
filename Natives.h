#pragma once

#include "main.h"
#include "CA_API.h"
#include "path.h"
#include "data.h"
#include "controller.h"

extern bool inited;
extern int ticked;
extern int maxTicked;
extern std::vector<AMX*> amxList;
extern CA_API *api;

class Natives
{
public:
	Natives() {};
	~Natives() {};
	
	static cell AMX_NATIVE_CALL PF_CreateCombinedPath(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL PF_AddPathToCombinedPath(AMX* amx, cell* params);

	static cell AMX_NATIVE_CALL PF_Init(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL PF_Create(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL PF_Remove(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL PF_SetPlaneSize(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL PF_SetWorld(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL PF_SetWallSize(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL PF_SetBeginRelativeCoord(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL PF_SetStart(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL PF_SetFinalAsPoint(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL PF_SetFinalAsCircle(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL PF_SetFinalAsLine(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL PF_Find(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL PF_FindNow(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL PF_GetPoint(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL PF_SetTickRate(AMX* amx, cell* params);
};

class RoadNatives
{
public:
	RoadNatives() {};
	~RoadNatives() {};

	static cell AMX_NATIVE_CALL ROAD_Create(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL ROAD_SetStartNode(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL ROAD_SetFinalNode(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL ROAD_Remove(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL ROAD_Find(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL ROAD_FindNow(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL ROAD_GetPoint(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL ROAD_AddNode(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL ROAD_GetNode(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL ROAD_RemoveNode(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL ROAD_FixRoads(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL ROAD_GetNormalPointToRoad(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL ROAD_FindNearbyRoad(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL ROAD_FindInvisibleNode(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL ROAD_GetMultipleNode(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL ROAD_GetNearbyMultipleNode(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL ROAD_IsMultipleNode(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL ROAD_IsEndNode(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL ROAD_Get(AMX* amx, cell* params);


	class Vehicle
	{
	public:
		Vehicle() {};
		~Vehicle() {};
		static cell AMX_NATIVE_CALL ROAD_Veh_Create(AMX* amx, cell* params);
		static cell AMX_NATIVE_CALL ROAD_Veh_SetStartNode(AMX* amx, cell* params);
		static cell AMX_NATIVE_CALL ROAD_Veh_SetStartPos(AMX* amx, cell* params);
		static cell AMX_NATIVE_CALL ROAD_Veh_SetFinalNode(AMX* amx, cell* params);
		static cell AMX_NATIVE_CALL ROAD_Veh_SetFinalPos(AMX* amx, cell* params);
	};
};
