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
#include "MovePath.h"

class Controller
{
public:
	Controller();
	~Controller();
	template <typename T_path> int CreatePath(char *name) {
		if (this->paths->size() >= MAX_PATHS_CREATED) return 0;
		int id = 1;

		while (id < MAX_PATHS_CREATED) {
			if (!this->IsPathValid(id)) {
				break;
			}
			id++;
		}

		genPath *path = new T_path();
		path->uID = rand();

		path->callback = name;
		this->paths->insert(std::make_pair(id, path));
		return id;
	};
	template <typename T_path> T_path *GetPath(int id) {
		return dynamic_cast<T_path*>(this->paths->find(id)->second);
	};
	bool IsPathValid(int id);
	void RemovePath(int id);
	void StartNewThread();

	//Public Data -> Main
	//http://codelessons.ru/cplusplus/ochered-queue-v-c-realizaciya-i-chto-eto-voobshhe-takoe.html - queue
	
	std::queue<genPath*> *qPath;
	std::queue<callbackWorkerData*> *qCallback;
	std::mutex *workQueue;
	std::mutex *callbackQueue;

	class MovePath
	{
	public:
		MovePath() {}
		~MovePath();
		int CreatePath();
		bool IsPathValid(int id);
		void RemovePath(int id);
		::MovePath *GetPath(int id) {
			return this->movePaths.find(id)->second;
		};
	private:
		std::map<int, ::MovePath*> movePaths;
	};

	Controller::MovePath *movePathContoller;
private:
	std::vector<Thread*> *threadList;
	std::map<int, genPath*> *paths;
};


extern Controller *pController;