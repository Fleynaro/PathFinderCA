//----------------------------------------------------------
//
//
//   	PathFinder Plugin
//                  by Pamdex and Fleynaro
//
//
//	   
//----------------------------------------------------------
//                         INCLUDES
//----------------------------------------------------------
#include "controller.h"

//----------------------------------------------------------
//                         Class
//----------------------------------------------------------
Controller::Controller()
{
	//Thread (������ �������. ��� ������ � �������)
	this->threadList = new std::vector<Thread*>; //��������� ��� ������� ����� http://cppstudio.com/post/8453/


	//Worker Data (������� ������)
	this->paths = new std::map<int, genPath*>;
	this->qPath = new std::queue<genPath*>;
	this->qCallback = new std::queue<callbackWorkerData*>; //��������� ��������� ������ callbackWorkerData � ������� queue

	//Init global mutexes
	this->workQueue = new std::mutex();
	this->callbackQueue = new std::mutex();
	this->movePathContoller = new Controller::MovePath();
}
Controller::~Controller() //��������. �������� ����� � ���
{
	//Delete Threads (�������� �������)
	for ( std::vector<Thread*>::iterator i = this->threadList->begin(); i != this->threadList->end(); ) //��������
	{
		delete (*i); //��������
		i = this->threadList->erase(i); //������� � ����������
	}
	
	//Delete other stuffs
	delete qPath;
	delete qCallback;
	delete workQueue;
	delete callbackQueue;
	delete callbackQueue;
	delete movePathContoller;
}

bool Controller::IsPathValid(int id)
{
	return (this->paths->find(id) != this->paths->end());
}

void Controller::RemovePath(int id)
{
	//delete this->GetPath(id);
	this->paths->erase(id);
	//logprintf("delete id %i", id);
}

void Controller::StartNewThread()
{
	//� ����� ��������� ��� ����������� ������
	Thread *tempThread = new Thread(this->qPath, this->qCallback, this->workQueue, this->callbackQueue); //!!! ����� � �� ����� this->
	this->threadList->push_back(tempThread); //push_back() ��������� ������� � ����� �������
}




int Controller::MovePath::CreatePath()
{
	if (this->movePaths.size() >= MAX_MOVE_PATHS_CREATED) return 0;
	int id = 1;

	while (id < MAX_MOVE_PATHS_CREATED) {
		if (!this->IsPathValid(id)) {
			break;
		}
		id++;
	}

	::MovePath *movepath = new ::MovePath();
	this->movePaths.insert(std::make_pair(id, movepath));
	return id;
}

bool Controller::MovePath::IsPathValid(int id)
{
	return (this->movePaths.find(id) != this->movePaths.end());
}

void Controller::MovePath::RemovePath(int id)
{
	auto movePath = this->movePaths.find(id);
	if (movePath == this->movePaths.end()) {
		return;
	}

	delete movePath->second;
	this->movePaths.erase(id);
}
