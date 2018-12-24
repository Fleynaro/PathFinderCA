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
#include "main.h"
#include "genPath.h"
#include "data.h"
#include "thread.h"
#include "CA_API.h"

class Controller
{
public:
	Controller(CA_API *api);
	~Controller();
	template <typename T_path>
	int CreatePath(char *name);
	bool IsPathValid(int id);
	genPath *GetPath(int id);
	void RemovePath(int id);
	void StartNewThread();

	//Public Data -> Main
	//http://codelessons.ru/cplusplus/ochered-queue-v-c-realizaciya-i-chto-eto-voobshhe-takoe.html - queue
	
	std::queue<genPath*> *qPath;
	std::queue<callbackWorkerData*> *qCallback;
	std::mutex *workQueue;
	std::mutex *callbackQueue;
private:
	std::vector<Thread*> *threadList;
	std::map<int, genPath*> *paths;
	CA_API *api;
};


extern Controller *pController;