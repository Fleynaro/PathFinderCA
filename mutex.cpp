//----------------------------------------------------------
//
//
//   	PathFinder Plugin
//                  by Pamdex
//
//
//	   https://habrahabr.ru/post/182610/ - работа с потоками
//----------------------------------------------------------
//                         INCLUDES
//----------------------------------------------------------
#include "main.h"
#include "mutex.h"
//----------------------------------------------------------
//                         Class
//----------------------------------------------------------
int Mutex::Init()
{
    #if defined(LINUX)
		return pthread_mutex_init (&this->mtx, NULL);
    #elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
        this->mtx = CreateMutex(0, FALSE, 0);
		return (this->mtx == 0);
    #endif
    return -1;
}

int Mutex::Lock()
{
    #if defined(LINUX)
		return pthread_mutex_lock(&this->mtx);
    #elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
	return (WaitForSingleObject(this->mtx, INFINITE) == WAIT_FAILED ? 1 : 0);
    #endif
    return -1;
}

int Mutex::Unlock()
{
    #if defined(LINUX)
		return pthread_mutex_unlock(&this->mtx);
    #elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
	return (ReleaseMutex(this->mtx) == 0);
    #endif
    return -1;
}