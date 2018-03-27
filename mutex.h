//----------------------------------------------------------
//
//
//   	PathFinder Plugin
//                  by Pamdex
//
//
//	   
//----------------------------------------------------------
#if defined(LINUX)
	#define MUTEX pthread_mutex_t
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
	#define MUTEX HANDLE
#endif

class Mutex
{
public:
	int Init();
	int Lock();
	int Unlock();
private:
	MUTEX mtx;
};