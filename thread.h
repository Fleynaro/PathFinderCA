//----------------------------------------------------------
//
//
//   	PathFinder Plugin
//                  by Pamdex
//
//
//	   
//----------------------------------------------------------
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define EXIT_THREAD() { _endthread(); }
#define START_THREAD(a, b) { _beginthread( a, 0, (void *)( b ) ); }
#elif defined(LINUX)
#define EXIT_THREAD() { pthread_exit( NULL ); }
#define START_THREAD(a, b) {	pthread_t thread;\
	pthread_create(&thread, NULL, a, (void *)(b)); }
#endif
enum ThreadState
{
	WORKING = 1,
	STOPING = 2,
	STOPED = 3
};
class Thread
{
public:
	Thread(std::queue<Path*> *qPath, std::queue<callbackWorkerData*> *qCallback, Mutex *workQueue, Mutex *callbackQueue);
	~Thread();
	Mutex *workQueue;
	Mutex *callbackQueue;
	std::queue<Path*> *qPath;
	std::queue<callbackWorkerData*> *qCallback;
private:
#ifdef WIN32
	static void RunPathCalculator(void *obj);
	void PathCalculator(void *unused);
#else
	static void *RunPathCalculator(void *obj);
	void *PathCalculator(void *unused);
#endif
	void KillThread();
	bool IsAlive();
	bool IsStoped();
	Mutex *threadState;
	ThreadState state;
};