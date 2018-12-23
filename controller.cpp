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
Controller::Controller(CA_API *api)
{
	//Thread (������ �������. ��� ������ � �������)
	this->threadList = new std::vector<Thread*>; //��������� ��� ������� ����� http://cppstudio.com/post/8453/


	//Worker Data (������� ������)
	this->paths = new std::map<int, Path*>;
	this->qPath = new std::queue<Path*>;
	this->qCallback = new std::queue<callbackWorkerData*>; //��������� ��������� ������ callbackWorkerData � ������� queue

	//Init global mutexes
	this->workQueue = new std::mutex();
	this->callbackQueue = new std::mutex();

	this->api = api;
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
	//delete mapAndreas;
}

int Controller::CreatePath(char *name)
{
	if (this->paths->size() >= MAX_PATHS_CREATED) return 0;
	int id = 1;

	while ( id < MAX_PATHS_CREATED) {
		if (!this->IsPathValid(id)) {
			break;
		}
		id++;
	}
	
	Path *path = new Path(this->api);
	path->uID = rand();
	//logprintf("created path id %i", path->uID);
	
	path->callback = name;
	this->paths->insert(std::make_pair(id, path));
	
	//logprintf("path id = %s", GetPath(id)->callback);
	return id;
}

bool Controller::IsPathValid(int id)
{
	return (this->paths->find(id) != this->paths->end());
}

Path *Controller::GetPath(int id)
{
	return this->paths->find(id)->second;
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