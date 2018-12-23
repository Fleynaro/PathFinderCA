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
#include <mutex>
#include <thread>
#include "main.h"
#include "path.h"
#include "data.h"
#include "SDK/amx/amx.h"

enum ThreadState
{
	WORKING = 1,
	STOPING = 2,
	STOPED = 3
};
class Thread
{
public:
	Thread(std::queue<Path*> *qPath, std::queue<callbackWorkerData*> *qCallback, std::mutex *workQueue, std::mutex *callbackQueue);
	~Thread();
	std::mutex *workQueue;
	std::mutex *callbackQueue;
	std::queue<Path*> *qPath;
	std::queue<callbackWorkerData*> *qCallback;
	int counter = 0;
private:
	void PathCalculator();
	void KillThread();
	bool IsAlive();
	bool IsStoped();
	std::mutex threadState;
	ThreadState state;
};