#if defined PathFinderCA
  #endinput
#endif
#define PathFinderCA


#define ROAD:: ROAD_
#define PF:: PF_

#define PF_PATH_FOUND 0
#define PF_PATH_NOT_FOUND 1

#define PF_SetFinal PF_SetFinalAsPoint

native PF_Init(thread = 1); //Иницилизация в начале мода
native PF_Create(callback[], Float: cellSize = 1.5, path_size = 2000, params[] = "", {Float,_}:...); //Создать путь в памяти(в общей очереди). cellSize - клетка(1.5 x 1.5м), path_size - максим. кол-во клеток, анализируемых алгоритмом поиска, далее - свои данные для передачи их в callback
native PF_Remove(path); //Удалить путь из памяти

native PF_SetPlaneSize(path, width, height); //Размеры зоны поиска
native PF_SetWorld(path, world); //Мир
native PF_SetWallSize(path, Float:up, Float:down); //установить размеры препятствий(чтобы, если стена не большая, бот перепрыгивал её(up) или спрыгивал с неё(down))
native PF_SetBeginRelativeCoord(path, Float:x, Float:y, width = 10, height = 10);

native PF_SetStart(path, Float:startX, Float:startY, Float:startZ); //Установить точку старта
native PF_SetFinalAsPoint(path, Float:endX, Float:endY, Float:endZ); //Установить точку финиша
native PF_SetFinalAsCircle(path, Float:endX, Float:endY, Float:radius); //Установить окружность финиша(не сферу!)
native PF_SetFinalAsLine(path, Float:lineX, Float:lineY, Float:lineAngle); //not working yet

native PF_Find(path); //Начать поиск в отдельном потоке. После использования оной функции, удаляется из памяти информация о пути, созданная посредством PF_Create. Потом вызывается callback
native PF_FindNow(path); //Начать поиск в общем потоке
native PF_GetPoint(path, &Float: x, &Float: y, &Float: z); //Получить координаты точки id(используется вкупе с PF_FindNow(id))


native PF_CreateCombinedPath(callback[], params[] = "", {Float,_}:...);
native PF_AddPathToCombinedPath(path, combinedPath);


const
	ROAD_NOT = 0xFFFF,
	ROAD_MODE_DEFAULT = 0,
	ROAD_MODE_SMOOTH = 1;
native ROAD_Create(callback[], mode = ROAD_MODE_DEFAULT, path_size = 1500, params[] = "", {Float,_}:...);
native ROAD_SetStartNode(path, index);
native ROAD_SetFinalNode(path, index);
native ROAD_Remove(path);
native ROAD_Find(path);
native ROAD_FindNow(path);
native ROAD_GetPoint(path, &Float: x, &Float: y, &Float: z);
native ROAD_AddNode(index, Float: x, Float: y, Float: z, child1, child2 = ROAD_NOT, child3 = ROAD_NOT, child4 = ROAD_NOT);
native ROAD_GetNode(index, &Float: x, &Float: y, &Float: z, childs[]);
native ROAD_RemoveNode(index);
native ROAD_FixRoads();
native ROAD_GetNormalPointToRoad_(startNode, finalNode, Float: X, Float: Y, &Float: x, &Float: y, &Float: z);
native ROAD_FindNearbyRoad_(Float: X, Float: Y, Float: Z, Float: radius, &Float: x, &Float: y, &Float: z, &startNode, &finalNode, Float: minRadius = 0.0);
native ROAD_FindInvisibleNode(index, Float:points_x[], Float:points_y[], Float:points_z[], const size = sizeof(points_x));
native ROAD_Get(node, child);
native ROAD_GetMultipleNode(index, child);
native ROAD_GetNearbyMultipleNode(index);
native ROAD_IsMultipleNode(index);
native ROAD_IsEndNode(index);

#define ROAD_Veh_First 0b1
#define ROAD_Veh_Last 0b10
native ROAD_Veh_Create(callback[], settings = 0, params[] = "", {Float,_}:...);
native ROAD_Veh_SetStartNode(path, index);
native ROAD_Veh_SetStartPos(path, Float: x, Float: y, Float: z);
native ROAD_Veh_SetFinalNode(path, index);
native ROAD_Veh_SetFinalPos(path, Float: x, Float: y, Float: z);



#define ROAD_ParentNode(%0) (%0 & 0xFFFF)
#define ROAD_ChildNode(%0) (%0 >> 16 & 0xFFFF)
stock ROAD_GetNode2(index, &Float: x, &Float: y, &Float: z)
{
	new childs[4];
	return ROAD_GetNode(ROAD_ParentNode(index), x, y, z, childs);
}
stock ROAD_GetNormalPointToRoad(road, Float: X, Float: Y, &Float: x, &Float: y, &Float: z)
{
	return ROAD_GetNormalPointToRoad_(ROAD_ParentNode(road), ROAD_ChildNode(road), X, Y, x, y, z);
}
stock ROAD_FindNearbyRoad(Float: X, Float: Y, Float: Z, Float: radius, &Float: x, &Float: y, &Float: z, Float: minRadius = 0.0)
{
	new startNode, finalNode;
	if ( ROAD_FindNearbyRoad_(X, Y, Z, radius, x, y, z, startNode, finalNode, minRadius) ) {
		return startNode | (finalNode << 16);
	}
	return ROAD_NOT;
}
stock ROAD_GetInvisibleNodeForPlayer(playerid, node)
{
	new Float: points_x[1],
		Float: points_y[1],
		Float: points_z[1];
	GetPlayerPos(playerid, points_x[0], points_y[0], points_z[0]);
	return ROAD_FindInvisibleNode(node, points_x, points_y, points_z);
}

/*
	
	Пример использования:
	PF_Init(1); //иницилизация с кол-вом потоков 1
	
	
	
	new	path = PF_Create("BOT_OnPathFound", 1.2, 3000, "iii", botid, value1, value2); //создание пути в памяти(но не поиск пути еще!)
	PF_SetStart(path, bx, by, bz); //начало
	PF_SetFinal(path, px, py, pz); //конец
	PF_Find(path); //начало поиска. Далее - вызов BOT_OnPathFound
	
	
	
	public BOT_OnPathFound(code, Float:nodesX[], Float:nodesY[], Float:nodesZ[], nodesSize, 	botid, value1, value2)
	{
		for ( new i; i < nodesSize; i ++ ) {
			nodesZ[i] += 0.3;
			Log("pos = %f,%f,%f", nodesX[i], nodesY[i], nodesZ[i]); //получение координат
		}
		return 1;
	}
*/