# PathFinderCA
PathFinder allow you to calculate a route from A to B. There are a a lot of functions for exact adjustment.


native PF_Init (thread = 1); // Initiation at the beginning of the mod
native PF_Create (callback [], Float: cellSize = 1.5, path_size = 2000, params [] = "", {Float, _}: ...); // Create a path in memory (in the general queue). cellSize - cell (1.5 x 1.5 m), path_size - maxim. number of cells analyzed by the search algorithm, then - their data for transmission to the callback
native PF_Remove (id); // Delete path from memory

native PF_SetPlaneSize (id, width, height); // Size of the search area
native PF_SetWallSize (id, Float: up, Float: down); // set the dimensions of the obstacles (so that if the wall is not large, the bot jumps up (up) or jumped down from it (down))
native PF_SetBeginRelativeCoord (id, Float: x, Float: y, width = 10, height = 10);

native PF_SetStart (id, Float: startX, Float: startY, Float: startZ); // Set start point
native PF_SetFinalAsPoint (id, Float: endX, Float: endY, Float: endZ); // Set the finish point
native PF_SetFinalAsCircle (id, Float: endX, Float: endY, Float: radius); // Set the finish circle (not the sphere!)
native PF_SetFinalAsLine (id, Float: lineX, Float: lineY, Float: lineAngle); // not working yet

native PF_Find (id); // Start the search in a separate thread. After using this function, the path information created by PF_Create is deleted from memory. Then callback is called
native PF_FindNow (id); // Start a search in the shared thread
native PF_GetPoint (id, & Float: x, & Float: y, & Float: z); // Get the coordinates of the id point (used in conjunction with PF_FindNow (id))

native PF_ColAndreasLock ();
native PF_ColAndreasUnlock ();




Example of use:
PF_Init (1); // initialization with number of threads 1

new path = PF_Create ("BOT_OnPathFound", 1.2, 3000, "iii", botid, value1, value2); // create a path in memory (but not search for a path yet!)
PF_SetStart (path, bx, by, bz); //Start
PF_SetFinal (path, px, py, pz); //end
PF_Find (path); // start the search. Next, call BOT_OnPathFound

public BOT_OnPathFound (code, Float: nodesX [], Float: nodesY [], Float: nodesZ [], nodesSize, botid, value1, value2)
{
for (new i; i <nodesSize; i ++) {
nodesZ [i] + = 0.3;
Log ("pos =% f,% f,% f", nodesX [i], nodesY [i], nodesZ [i]); // get coordinates
}
return 1;
}
