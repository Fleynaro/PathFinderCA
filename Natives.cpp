#include "Natives.h"

Natives::Natives()
{
}

// native PathFinder_Init(threads);
cell AMX_NATIVE_CALL Natives::PF_Init(AMX* amx, cell* params)
{
	if (!inited)
	{
		CA_API *api = new CA_API();
		if (!api->success) {
			logprintf("PathFinder Plugin not loaded.");
			return 0;
		}
		pController = new Controller(api);

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
		int id = pController->CreatePath(name);
		if (id == 0) {
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
		Path *path = pController->GetPath(id);
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
		Path *path = pController->GetPath(id);
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
		Path *path = pController->GetPath(id);
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
		Path *path = pController->GetPath(id);
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
		Path *path = pController->GetPath(id);
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
		Path *path = pController->GetPath(id);
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
		Path *path = pController->GetPath(id);
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
		Path *path = pController->GetPath(id);
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
		pController->qPath->push(pController->GetPath(id));
		//logprintf("name %s(%i)", pController->GetPath(id)->callback, id);

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
cell AMX_NATIVE_CALL Natives::PF_GetPoint(AMX* amx, cell* params)
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
