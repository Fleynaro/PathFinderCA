//----------------------------------------------------------
//
//
//   	PathFinder Plugin
//                  by Pamdex
//
//
//	   
//----------------------------------------------------------

class Controller
{
public:
	Controller(CA_API *api);
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
	std::mutex *workQueue;
	std::mutex *callbackQueue;
private:
	std::vector<Thread*> *threadList;
	std::map<int, Path*> *paths;
	CA_API *api;
};