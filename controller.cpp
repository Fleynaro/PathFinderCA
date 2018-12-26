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
	//Thread (Список потоков. Это классы в векторе)
	this->threadList = new std::vector<Thread*>; //Подробнее про векторы здесь http://cppstudio.com/post/8453/


	//Worker Data (Рабочие данные)
	this->paths = new std::map<int, genPath*>;
	this->qPath = new std::queue<genPath*>;
	this->qCallback = new std::queue<callbackWorkerData*>; //переносим структуру данных callbackWorkerData в очередь queue

	//Init global mutexes
	this->workQueue = new std::mutex();
	this->callbackQueue = new std::mutex();
}
Controller::~Controller() //Выгрузка. Удаление всего и вся
{
	//Delete Threads (Удаление потоков)
	for ( std::vector<Thread*>::iterator i = this->threadList->begin(); i != this->threadList->end(); ) //итератор
	{
		delete (*i); //удаление
		i = this->threadList->erase(i); //переход к следующему
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
	//в поток переносим все необходимые данные
	Thread *tempThread = new Thread(this->qPath, this->qCallback, this->workQueue, this->callbackQueue); //!!! можно и не юзать this->
	this->threadList->push_back(tempThread); //push_back() вставляет элемент в конец вектора
}