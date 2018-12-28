#include "Natives.h"

Natives::Natives()
{
}

// native PathFinder_Init(threads);
cell AMX_NATIVE_CALL Natives::PF_Init(AMX* amx, cell* params)
{
	if (!inited)
	{
		api = new CA_API();
		if (!api->success) {
			logprintf("PathFinder Plugin not loaded.");
			return 0;
		}
		pController = new Controller();

		//Start threads
		for (int i = 0; i < params[1]; i++)
		{
			logprintf("PathFinder Plugin -> Creating New Thread");
			pController->StartNewThread();
		}

		inited = true;
	}
	return 1;
}

//PF_Create(callback[], Float: cellSize = 1.5, path_size = 2000, params[] = "", {Float,_}:...)
cell AMX_NATIVE_CALL Natives::PF_Create(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		char *name;
		amx_StrParam(amx, params[1], name);
		int id = pController->CreatePath<Path>(name);
		if (id == 0) {
			return 0;
		}

		Path *path = pController->GetPath<Path>(id);
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
cell AMX_NATIVE_CALL Natives::PF_Remove(AMX* amx, cell* params)
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
cell AMX_NATIVE_CALL Natives::PF_SetPlaneSize(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		Path *path = pController->GetPath<Path>(id);
		path->SetPlaneSize(params[2], params[3]);
		return 1;
	}
}
//PF_SetWorld(id, world)
cell AMX_NATIVE_CALL Natives::PF_SetWorld(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		genPath *path = pController->GetPath<genPath>(id);
		path->SetWorld(params[2]);
		return 1;
	}
}
//PF_SetWallSize(id, Float:up, Float:down)
cell AMX_NATIVE_CALL Natives::PF_SetWallSize(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		Path *path = pController->GetPath<Path>(id);
		path->WALL_UP = amx_ctof(params[2]);
		path->WALL_DOWN = amx_ctof(params[3]);
		return 1;
	}
}
//PF_SetBeginRelativeCoord(id, Float:x, Float:y, width = 10, height = 10)
cell AMX_NATIVE_CALL Natives::PF_SetBeginRelativeCoord(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		Path *path = pController->GetPath<Path>(id);
		path->SetBeginRelativeCoord(amx_ctof(params[2]), amx_ctof(params[3]), params[4], params[5]);
		return 1;
	}
}
//PF_SetStart(id, Float:startX, Float:startY, Float:startZ)
cell AMX_NATIVE_CALL Natives::PF_SetStart(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		Path *path = pController->GetPath<Path>(id);
		path->startX = amx_ctof(params[2]);
		path->startY = amx_ctof(params[3]);
		path->startZ = amx_ctof(params[4]);
		//logprintf("[PathFinder] path->start = %f(%f),%f,%f", path->startX, amx_ctof(params[2]), path->startY, path->startZ);
		return 1;
	}
}
//PF_SetFinalAsPoint(id, Float:endX, Float:endY, Float:endZ)
cell AMX_NATIVE_CALL Natives::PF_SetFinalAsPoint(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		Path *path = pController->GetPath<Path>(id);
		path->type = PATH_TYPE_POINT;
		path->endX = amx_ctof(params[2]);
		path->endY = amx_ctof(params[3]);
		path->endZ = amx_ctof(params[4]);
		return 1;
	}
}
//PF_SetFinalAsCircle(id, Float:endX, Float:endY, Float:radius)
cell AMX_NATIVE_CALL Natives::PF_SetFinalAsCircle(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		Path *path = pController->GetPath<Path>(id);
		path->type = PATH_TYPE_CIRCLE;
		path->endX = amx_ctof(params[2]);
		path->endY = amx_ctof(params[3]);
		path->endZ = amx_ctof(params[4]);
		return 1;
	}
}
//PF_SetFinalAsLine(id, Float:lineX, Float:lineY, Float:lineAngle)
cell AMX_NATIVE_CALL Natives::PF_SetFinalAsLine(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		Path *path = pController->GetPath<Path>(id);
		path->type = PATH_TYPE_LINE;
		//to do
		return 1;
	}
}

//PF_Find(id)
cell AMX_NATIVE_CALL Natives::PF_Find(AMX* amx, cell* params)
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
		//logprintf("name %s(%i)", pController->GetPath<genPath>(id)->callback, id);
		pController->qPath->push(pController->GetPath<genPath>(id));

		//Unlock
		pController->workQueue->unlock();

		pController->RemovePath(id);
		return 1;
	}
}

//PF_FindNow(id, &code, &Float:nodesX[], &Float:nodesY[], &Float:nodesZ[], &nodesSize)
cell AMX_NATIVE_CALL Natives::PF_FindNow(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		if (!pController->IsPathValid(id)) {
			return 0;
		}
		genPath *path = pController->GetPath<genPath>(id);
		path->Find();

		if (path->status == Path::FOUND && !path->pathData->empty()) {
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
cell AMX_NATIVE_CALL Natives::PF_GetPoint(AMX* amx, cell* params)
{
	if (!inited) return 0;
	else
	{
		int id = params[1];
		genPath *path = pController->GetPath<genPath>(id);

		if (path->status != Path::FOUND || path->pathData->empty()) {
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
cell AMX_NATIVE_CALL Natives::PF_SetTickRate(AMX* amx, cell* params)
{
	ticked = 0;
	if (params[1] <= 0) maxTicked = 1;
	else maxTicked = params[1];
	return 1;
}

Natives::~Natives()
{
}

//PF_CreateCombinedPath(callback[], params[] = "", {Float,_}:...)
cell AMX_NATIVE_CALL Natives::PF_CreateCombinedPath(AMX * amx, cell * params)
{
	if (!inited) return 0;
	else
	{
		char *name;
		amx_StrParam(amx, params[1], name);
		int id = pController->CreatePath<combinedPath>(name);
		if (id == 0) {
			return 0;
		}

		combinedPath *path = pController->GetPath<combinedPath>(id);

		char *format;
		amx_StrParam(amx, params[2], format);
		if (format) {
			std::deque<cell> deq_params;
			cell *addr_params;

			const int offset = 3;
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

//PF_AddPathToCombinedPath(path, combinedPath)
cell AMX_NATIVE_CALL Natives::PF_AddPathToCombinedPath(AMX * amx, cell * params)
{
	int id = params[1];
	if (!pController->IsPathValid(id)) {
		return 0;
	}
	int id2 = params[2];
	if (!pController->IsPathValid(id2)) {
		return 0;
	}

	genPath *path = pController->GetPath<genPath>(id);
	combinedPath *combPath = pController->GetPath<combinedPath>(id2);
	combPath->addPath(path);
	return 1;
}


#include "RoadPath.h"

RoadNatives::RoadNatives()
{
}

RoadNatives::~RoadNatives()
{
}

//ROAD_Create(callback[], mode = 0, path_size = 1500, params[] = "", {Float,_}:...)
cell AMX_NATIVE_CALL RoadNatives::ROAD_Create(AMX * amx, cell * params)
{
	if (!inited) return 0;
	else
	{
		char *name;
		amx_StrParam(amx, params[1], name);
		int id = pController->CreatePath<RoadPath>(name);
		if (id == 0) {
			return 0;
		}

		RoadPath *path = pController->GetPath<RoadPath>(id);
		path->Mode = static_cast<RoadPath::mode>(params[2]);
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

//ROAD_SetStartNode(id, index)
cell AMX_NATIVE_CALL RoadNatives::ROAD_SetStartNode(AMX * amx, cell * params)
{
	nodeId index = road::getNode(params[2]);
	if (!(0 <= index && index < RoadPath::size())) return 0;
	roadNode *node = RoadPath::getNode(index);
	if (node->getId() == ROAD_NOT) return 0;

	int id = params[1];
	if (!pController->IsPathValid(id)) {
		return 0;
	}
	RoadPath *path = pController->GetPath<RoadPath>(id);
	path->setStart(index);
	return 1;
}

//ROAD_SetFinalNode(id, index)
cell AMX_NATIVE_CALL RoadNatives::ROAD_SetFinalNode(AMX * amx, cell * params)
{
	nodeId index = road::getNode(params[2]);
	if (!(0 <= index && index < RoadPath::size())) return 0;
	roadNode *node = RoadPath::getNode(index);
	if (node->getId() == ROAD_NOT) return 0;

	int id = params[1];
	if (!pController->IsPathValid(id)) {
		return 0;
	}
	RoadPath *path = pController->GetPath<RoadPath>(id);
	path->setFinal(index);
	return 1;
}

//ROAD_Remove(id)
cell AMX_NATIVE_CALL RoadNatives::ROAD_Remove(AMX * amx, cell * params)
{
	return Natives::PF_Remove(amx, params);
}

//ROAD_Find(id)
cell AMX_NATIVE_CALL RoadNatives::ROAD_Find(AMX * amx, cell * params)
{
	return Natives::PF_Find(amx, params);
}

//ROAD_FindNow(id, &code, &Float:nodesX[], &Float:nodesY[], &Float:nodesZ[], &nodesSize)
cell AMX_NATIVE_CALL RoadNatives::ROAD_FindNow(AMX * amx, cell * params)
{
	return Natives::PF_FindNow(amx, params);
}

//ROAD_GetPoint(id, &Float: x, &Float: y, &Float: z)
cell AMX_NATIVE_CALL RoadNatives::ROAD_GetPoint(AMX * amx, cell * params)
{
	return Natives::PF_GetPoint(amx, params);;
}

//ROAD_AddNode(index, Float: x, Float: y, Float: z, child1, child2, child3, child4)
cell AMX_NATIVE_CALL RoadNatives::ROAD_AddNode(AMX * amx, cell * params)
{
	nodeId index = road::getNode(params[1]);
	float
		x = amx_ctof(params[2]),
		y = amx_ctof(params[3]),
		z = amx_ctof(params[4]);

	roadNode node(index, x, y, z);
	for (int i = 0; i < 4; i++) {
		if (params[5 + i] == -1) break;
		node.setChild(i, params[5 + i]);
	}
	RoadPath::addNode(index, node);

	return 1;
}

//ROAD_GetNode(index, &Float: x, &Float: y, &Float: z, childs[])
cell AMX_NATIVE_CALL RoadNatives::ROAD_GetNode(AMX * amx, cell * params)
{
	nodeId index = road::getNode(params[1]);
	if (!(0 <= index && index < RoadPath::size())) return 0;
	roadNode *node = RoadPath::getNode(index);
	if (node->getId() == ROAD_NOT) return 0;
	float
		x = node->getX(),
		y = node->getY(),
		z = node->getZ();
	cell *pAddress = NULL;
	amx_GetAddr(amx, params[2], &pAddress);
	*pAddress = amx_ftoc(x);
	amx_GetAddr(amx, params[3], &pAddress);
	*pAddress = amx_ftoc(y);
	amx_GetAddr(amx, params[4], &pAddress);
	*pAddress = amx_ftoc(z);
	amx_GetAddr(amx, params[5], &pAddress);
	for (int i = 0; i < 4; i ++) {
		cell id = ROAD_NOT;
		if (node->isChild(i)) {
			id = node->getChild(i)->getId();
		}
		*(pAddress + i) = id;
	}
	return 1;
}

//ROAD_RemoveNode(index)
cell AMX_NATIVE_CALL RoadNatives::ROAD_RemoveNode(AMX * amx, cell * params)
{
	nodeId index = road::getNode(params[1]);
	if (!(0 <= index && index < RoadPath::size())) return 0;
	roadNode *node = RoadPath::getNode(index);
	if (node->getId() == ROAD_NOT) return 0;
	RoadPath::removeNode(index);
	return 1;
}

cell AMX_NATIVE_CALL RoadNatives::ROAD_FixRoads(AMX * amx, cell * params)
{
	RoadPath::fixRoads();
	return 1;
}

//ROAD_GetNormalPointToRoad(startNode, finalNode, Float: X, Float: Y, &Float: x, &Float: y, &Float: z)
cell AMX_NATIVE_CALL RoadNatives::ROAD_GetNormalPointToRoad(AMX * amx, cell * params)
{
	nodeId
		startNode = road::getNode(params[1]),
		finalNode = road::getNode(params[2]);
	if (!(0 <= startNode && startNode < RoadPath::size() && 0 <= finalNode && finalNode < RoadPath::size())) return 0;
	road Road = road(RoadPath::getNode(startNode), RoadPath::getNode(finalNode));
	if (!Road.isValid()) return 0;
	//logprintf("ROAD_GetNormalPointToRoad: startNode=%i,finalNode=%i", startNode, finalNode);

	float
		X = amx_ctof(params[3]),
		Y = amx_ctof(params[4]),
		x, y, z;
	bool result = Road.getNormalPoint(X, Y, x, y, z);

	cell *pAddress = NULL;
	amx_GetAddr(amx, params[5], &pAddress);
	*pAddress = amx_ftoc(x);
	amx_GetAddr(amx, params[6], &pAddress);
	*pAddress = amx_ftoc(y);
	amx_GetAddr(amx, params[7], &pAddress);
	*pAddress = amx_ftoc(z);
	return result;
}

//ROAD_FindNearbyRoad(Float: X, Float: Y, Float: Z, Float: radius, &Float: x, &Float: y, &Float: z, &startNode, &finalNode, Float: minRadius = 0.0)
cell AMX_NATIVE_CALL RoadNatives::ROAD_FindNearbyRoad(AMX * amx, cell * params)
{
	float
		X = amx_ctof(params[1]),
		Y = amx_ctof(params[2]),
		Z = amx_ctof(params[3]),
		radius = amx_ctof(params[4]),
		minRadius = amx_ctof(params[10]),
		x = 0.0, y = 0.0, z = 0.0;

	road Road = road::findNearbyRoad(X, Y, Z, radius, x, y, z, minRadius);
	if ( Road.isValid() ) {
		cell *pAddress = NULL;
		amx_GetAddr(amx, params[5], &pAddress);
		*pAddress = amx_ftoc(x);
		amx_GetAddr(amx, params[6], &pAddress);
		*pAddress = amx_ftoc(y);
		amx_GetAddr(amx, params[7], &pAddress);
		*pAddress = amx_ftoc(z);

		amx_GetAddr(amx, params[8], &pAddress);
		*pAddress = Road.getParentNode()->getId();
		amx_GetAddr(amx, params[9], &pAddress);
		*pAddress = Road.getNextNode()->getId();
		return 1;
	}
	return 0;
}

//ROAD_FindInvisibleNode(index, Float:points_x[], Float:points_y[], Float:points_z[], const size = sizeof(points_x))
cell AMX_NATIVE_CALL RoadNatives::ROAD_FindInvisibleNode(AMX * amx, cell * params)
{
	nodeId
		startNode = road::getNode(params[1]);
	if (!(0 <= startNode && startNode < RoadPath::size())) return ROAD_NOT;
	std::vector <Point3D*> points;

	cell *pAddressX = NULL;
	cell *pAddressY = NULL;
	cell *pAddressZ = NULL;
	amx_GetAddr(amx, params[2], &pAddressX);
	amx_GetAddr(amx, params[3], &pAddressY);
	amx_GetAddr(amx, params[4], &pAddressZ);

	int iSize = static_cast<int>(params[5]);
	for (int i = 0; i < iSize; i++) {
		float
			x = amx_ctof(*(pAddressX + i)),
			y = amx_ctof(*(pAddressY + i)),
			z = amx_ctof(*(pAddressZ + i));
		points.push_back(&Point3D(x, y, z));
	}

	roadNode *node = roadNode::getInvisibleNode(&points, RoadPath::getNode(startNode));
	if (node == NULL) return ROAD_NOT;
	return node->getId();
}

//ROAD_GetMultipleNode(index, child)
cell AMX_NATIVE_CALL RoadNatives::ROAD_GetMultipleNode(AMX * amx, cell * params)
{
	nodeId
		node = road::getNode(params[1]);
	if (!(0 <= node && node < RoadPath::size())) return ROAD_NOT;
	return RoadPath::getNode(node)->getMultipleNode(params[2]).get();
}

//ROAD_GetNearbyMultipleNode(index)
cell AMX_NATIVE_CALL RoadNatives::ROAD_GetNearbyMultipleNode(AMX * amx, cell * params)
{
	nodeId
		node = road::getNode(params[1]);
	if (!(0 <= node && node < RoadPath::size())) return ROAD_NOT;
	return RoadPath::getNode(node)->getNearbyMultipleNode().getParentNode()->getId();
}

//ROAD_IsMultipleNode(index)
cell AMX_NATIVE_CALL RoadNatives::ROAD_IsMultipleNode(AMX * amx, cell * params)
{
	nodeId
		node = road::getNode(params[1]);
	if (!(0 <= node && node < RoadPath::size())) return 0;
	return RoadPath::getNode(node)->isMultiple();
}

//ROAD_IsEndNode(index)
cell AMX_NATIVE_CALL RoadNatives::ROAD_IsEndNode(AMX * amx, cell * params)
{
	nodeId
		node = road::getNode(params[1]);
	if (!(0 <= node && node < RoadPath::size())) return 0;
	return RoadPath::getNode(node)->isEnd();
}

//ROAD_Get(node, child)
cell AMX_NATIVE_CALL RoadNatives::ROAD_Get(AMX * amx, cell * params)
{
	nodeId index = road::getNode(params[1]);
	int child = params[2];
	cell Road = 0xFFFFFFFF;
	if (!(0 <= index && index < RoadPath::size())) return Road;
	roadNode *node = RoadPath::getNode(index);
	if (node->getId() == ROAD_NOT || !node->isChild(child)) return Road;

	Road = index | (node->getChild(child)->getId() << 16);
	return Road;
}


