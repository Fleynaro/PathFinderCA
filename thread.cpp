//----------------------------------------------------------
//
//
//   	PathFinder Plugin
//                  by Pamdex
//
//
//	   
//----------------------------------------------------------
//                         INCLUDES
//----------------------------------------------------------
#include "thread.h"
//----------------------------------------------------------
// Thread
//----------------------------------------------------------
Thread::Thread(std::queue<genPath*> *qPath, std::queue<callbackWorkerData*> *qCallback, std::mutex *workQueue, std::mutex *callbackQueue)
{
	//Get queues
	this->qPath = qPath;
	this->qCallback = qCallback;

	//Get mutexes
	this->workQueue = workQueue;
	this->callbackQueue = callbackQueue;
	
	this->state = ThreadState(WORKING);
	std::thread th(&Thread::PathCalculator, this);
	th.detach();
}

Thread::~Thread()
{
	this->KillThread();
}

//Alive
bool Thread::IsAlive()
{
	//Temp array
	ThreadState tempData;

	//Block data access
	this->threadState.lock();

	//Get data
	tempData = this->state;

	//Unlock data access
	this->threadState.unlock();

	//Return data
	if (tempData == ThreadState(WORKING)) return true;
	else return false;
}

//Stoped
bool Thread::IsStoped()
{
	//Temp array
	ThreadState tempData;

	//Block data access
	this->threadState.lock();

	//Get data
	tempData = this->state;

	//Unlock data access
	this->threadState.unlock();

	//Return data
	if (tempData == ThreadState(STOPED)) return true;
	else return false;
}

//Kill
void Thread::KillThread()
{
	//Block data access
	this->threadState.lock();

	//Set data
	this->state = ThreadState(STOPING);

	//Unlock data access
	this->threadState.unlock();

	//Wait for thread kill
	while(true)
	{
		if(this->IsStoped()) break;
	}
}

//PathCalculator
void Thread::PathCalculator()
{
	//Loop
	while(this->IsAlive())
	{
		//logprintf("std::this_thread::get_id() = %i", std::this_thread::get_id());
		workQueue->lock();
		if(!qPath->empty())
		{
			//Получаем данные о пути
			genPath *tempPath = this->qPath->front(); //мы передали qPath в этот класс Thread из controller
			qPath->pop();
			//Unlock
			workQueue->unlock();
			//logprintf("find.... %i", tempPath->uID);
			//Search
			tempPath->Find();
			//logprintf("found.... %i", tempPath->uID);
			//Send data - lock mutex
			callbackQueue->lock();

			if (tempPath->status == Path::state::FOUND)
			{
				//logprintf("success found... %i", tempPath->uID);
				//Создаем данные для отправки уже потом в павн через callback
				callbackWorkerData *tempCallbackWorker = new callbackWorkerData();
				tempCallbackWorker->name = tempPath->callback;
				tempCallbackWorker->resultCode = Path::state::FOUND;
				tempCallbackWorker->params = tempPath->params;
				
				//Init
				tempCallbackWorker->nodeX = new std::vector<cell>;
				tempCallbackWorker->nodeY = new std::vector<cell>;
				tempCallbackWorker->nodeZ = new std::vector<cell>;

				//Get nodes (Из pathData переносим инфу во временную tempCallbackWorker->node, чтобы потом в постоянную qCallback->push(tempCallbackWorker) и потом это все PAWN)
				while (!tempPath->pathData->empty())
				{
					float tempX = tempPath->pathData->back()->x;
					float tempY = tempPath->pathData->back()->y;
					float tempZ = tempPath->pathData->back()->z;
					tempCallbackWorker->nodeX->push_back(amx_ftoc(tempX));
					tempCallbackWorker->nodeY->push_back(amx_ftoc(tempY));
					tempCallbackWorker->nodeZ->push_back(amx_ftoc(tempZ));
					tempPath->pathData->pop_back();
					//logprintf("rewrite tempX,tempY,tempZ=%f,%f,%f(%f)", tempX, tempY, tempZ, amx_ctof(amx_ftoc(tempZ)));
				}

				qCallback->push(tempCallbackWorker);
			}
			else //ЕСЛИ ПУТЬ НЕ НАЙДЕН, ТО ВОТ ТАК ПОСТУПАЕМ:
			{
				//logprintf("not found... %i", tempPath->uID);
				//Create empty data ;C
				callbackWorkerData *tempCallbackWorker = new callbackWorkerData();
				tempCallbackWorker->resultCode = tempPath->status;
				tempCallbackWorker->name = tempPath->callback;
				tempCallbackWorker->params = tempPath->params;
				
				//Init
				tempCallbackWorker->nodeX = new std::vector<cell>;
				tempCallbackWorker->nodeY = new std::vector<cell>;
				tempCallbackWorker->nodeZ = new std::vector<cell>;
				
				tempCallbackWorker->nodeX->push_back(0);
				tempCallbackWorker->nodeY->push_back(0);
				tempCallbackWorker->nodeZ->push_back(0);

				qCallback->push(tempCallbackWorker);
			}

			//Unlock
			callbackQueue->unlock();

			//Clear
			tempPath->Destroy();
			SLEEP(5); //Wait ...
		}
		else //Waiting...
		{
			workQueue->unlock();
			SLEEP(20);
			counter++;
		}
	}
	
	//Block data access
	this->threadState.lock();

	//Set data
	this->state = ThreadState(STOPED);

	//Unlock data access
	this->threadState.unlock();
}