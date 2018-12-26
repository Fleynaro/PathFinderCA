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