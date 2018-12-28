#pragma once
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
	float genDist;
	std::string name;
	std::deque<cell> params;
	std::vector <cell> *nodeX;
	std::vector <cell> *nodeY;
	std::vector <cell> *nodeZ;
};