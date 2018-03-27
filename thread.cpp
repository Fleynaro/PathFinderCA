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
#include "main.h"
#include "mutex.h"
#include "path.h"
#include "data.h"
#include "thread.h"
#include "SDK/amx/amx.h"
//----------------------------------------------------------
// Thread
//----------------------------------------------------------
Thread::Thread(std::queue<Path*> *qPath, std::queue<callbackWorkerData*> *qCallback, Mutex *workQueue, Mutex *callbackQueue)
{
	//Get queues
	this->qPath = qPath;
	this->qCallback = qCallback;

	//Get mutexes
	this->workQueue = workQueue;
	this->callbackQueue = callbackQueue;
	this->threadState = new Mutex();

	this->state = ThreadState(WORKING);

	//Start thread ЗАПУСКАЕМ ПОТОК
	START_THREAD(&Thread::RunPathCalculator, this); //fucking shit - 1h searching problem with this (NULL -> this)
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
	this->threadState->Lock();

	//Get data
	tempData = this->state;

	//Unlock data access
	this->threadState->Unlock();

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
	this->threadState->Lock();

	//Get data
	tempData = this->state;

	//Unlock data access
	this->threadState->Unlock();

	//Return data
	if (tempData == ThreadState(STOPED)) return true;
	else return false;
}

//Kill
void Thread::KillThread()
{
	//Block data access
	this->threadState->Lock();

	//Set data
	this->state = ThreadState(STOPING);

	//Unlock data access
	this->threadState->Unlock();

	//Wait for thread kill
	while(true)
	{
		if(this->IsStoped()) break;
	}
}

//Wrapper
#ifdef WIN32
void Thread::RunPathCalculator(void *obj)
#else
void *Thread::RunPathCalculator(void *obj)
#endif
{
	static_cast<Thread*>(obj)->PathCalculator(NULL);
	//return ((Thread *)obj)->PathCalculator(NULL); also working
}

//PathCalculator
#ifdef WIN32
void Thread::PathCalculator(void *unused)
#else
void *Thread::PathCalculator(void *unused)
#endif
{
	//Loop
	while(this->IsAlive())
	{
		workQueue->Lock();
		if(!qPath->empty())
		{
			//Получаем данные о пути
			Path *tempPath = this->qPath->front(); //мы передали qPath в этот класс Thread из controller
			qPath->pop();
			//Unlock
			workQueue->Unlock();
			//logprintf("find....");
			//Search
			tempPath->Find();
			//Send data - lock mutex
			callbackQueue->Lock();

			if (tempPath->status == PATH_FOUND)
			{
				//Создаем данные для отправки уже потом в павн через callback
				callbackWorkerData *tempCallbackWorker = new callbackWorkerData();
				strcpy(tempCallbackWorker->name, tempPath->callback);
				tempCallbackWorker->resultCode = PATH_FOUND;
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
				//Create empty data ;C
				callbackWorkerData *tempCallbackWorker = new callbackWorkerData();
				tempCallbackWorker->resultCode = tempPath->status;
				strcpy(tempCallbackWorker->name, tempPath->callback);
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
			callbackQueue->Unlock();

			//Clear
			tempPath->Destroy();
			SLEEP(5); //Wait ...
		}
		else //Waiting...
		{
			workQueue->Unlock();

			/*float x, y, z; int r;
			r = g_Invoke->CA_RayCastLine(-2043.0, -88.0, 36.0, -2043.0, -88.0, 0.0, &x, &y, &z);
			logprintf("THREAD x,y,z = %i, %f %f %f", r, x, y, z);*/

			SLEEP(20);
		}
	}
	
	//Block data access
	this->threadState->Lock();

	//Set data
	this->state = ThreadState(STOPED);

	//Unlock data access
	this->threadState->Unlock();
	
	EXIT_THREAD();
}