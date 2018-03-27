//----------------------------------------------------------
//
//
//   	PathFinder Plugin
//                  by Pamdex
//
//
//	   
//----------------------------------------------------------
#include <Invoke/Invoke.h>
class Controller
{
public:
	Controller(Invoke *g_Invoke);
	~Controller();
	int CreatePath(char *name);
	bool IsPathValid(int id);
	Path *GetPath(int id);
	void RemovePath(int id);
	void StartNewThread();

	//Public Data -> Main
	//http://codelessons.ru/cplusplus/ochered-queue-v-c-realizaciya-i-chto-eto-voobshhe-takoe.html - queue
	
	std::queue<Path*> *qPath;
	std::queue<callbackWorkerData*> *qCallback;
	Mutex *workQueue;
	Mutex *callbackQueue;
	Mutex *colAndreasQueue;
private:
	std::vector<Thread*> *threadList;
	std::map<int, Path*> *paths;
	Invoke *g_Invoke;
};