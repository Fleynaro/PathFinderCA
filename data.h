//----------------------------------------------------------
//
//
//   	PathFinder Plugin
//                  by Pamdex
//
//
//	   
//----------------------------------------------------------
struct callbackWorkerData
{
	int resultCode;
	char name[30];
	std::deque<cell> params;
	std::vector <cell> *nodeX;
	std::vector <cell> *nodeY;
	std::vector <cell> *nodeZ;
};