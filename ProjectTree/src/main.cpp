#include<iostream>
#include <cstdint>


#include"glib.hpp"

#define LOAD		(1000u)
uint32_t SharedResurse = 0ul;
void Thread1_Func( gpointer data );
GMutex *mutex;


typedef enum{
	MAIN_PROCESS = 0,
	CHILD_PROCESS,
	TEST_THREAD,
	TEST_THREADANDMUTEX,

	UNSUPORTED_GOAL
}TE_GOAL;

const gchar ua8_ChildProcess[] = "CHILD";
const gchar ua8_Thread[] = "THREADS";
const gchar ua8_ThreadMx[] = "THREADS_MUTEX";
void MainProcess(void);
void ChildProcess(void);
void TestMultiThread(uint8_t useMutex);

TE_GOAL FindGoal(const char* Flag);

int main( int argc, char **argv )
{

	int RetVal = 0;
	std::cout << "The argc is: " << argc <<std::endl;
	if(1 == argc)
	{/* Main process */
		MainProcess();
	}
	else
	{
		if(2 == argc)
		{
			TE_GOAL lcase = FindGoal(argv[1]);
			switch(lcase)
			{
				case CHILD_PROCESS:
					ChildProcess();
				break;

				case TEST_THREAD:
					TestMultiThread(0u);
				break;

				case TEST_THREADANDMUTEX:
					TestMultiThread(1u);
				break;

				default:
					std::cout << "Flag is not supported." << std::endl;
					RetVal = 2;
			}
		}
		else
		{
			std::cout << "Incorect no of flags." << std::endl;
			RetVal = 1;
		}
	}

	return RetVal;
}

void MainProcess(void)
{
	std::cout << "Main process was called." << std::endl;
}

void ChildProcess(void)
{
	std::cout << "Child process was called." << std::endl;
}

void TestMultiThread(uint8_t useMutex)
{
	if(0u != useMutex )
	{
	mutex = g_new(GMutex, 1);
	g_mutex_init(mutex);
	}
	GThread * Thread1 = g_thread_new(NULL, (GThreadFunc)Thread1_Func, &useMutex);

	for(uint32_t i = 0u; i < LOAD; i++)
	{
		if(0u != useMutex )
		{
			g_mutex_lock(mutex);
		}
		SharedResurse++;
		std::cout << "MainThread:" << SharedResurse << std::endl;
		if(0u != useMutex )
		{
			g_mutex_unlock(mutex);
		}
	}
	
	g_thread_join(Thread1);
}

void Thread1_Func( gpointer data)
{

	uint8_t useMutex = *((uint8_t*)data);

	for(uint32_t i = 0u; i < LOAD; i++)
	{
		if(0u != useMutex )
		{
			g_mutex_lock(mutex);
		}
		SharedResurse++;
		std::cout << "Thread1:" << SharedResurse << std::endl;
		if(0u != useMutex )
		{
			g_mutex_unlock(mutex);
		}
	}
}

TE_GOAL FindGoal(const char* Flag)
{
	TE_GOAL retVal = UNSUPORTED_GOAL;
	GString* sFlag = g_string_new((const gchar*) Flag);

	sFlag = g_string_ascii_up(sFlag);

	GString* ls_ChildProcess = g_string_new(ua8_ChildProcess);
	GString* ls_Thread = g_string_new(ua8_Thread);
	GString* ls_ThreadMx = g_string_new(ua8_ThreadMx);

	if(g_string_equal(sFlag, ls_ChildProcess) )
	{
		retVal = CHILD_PROCESS;
	}
	else if( g_string_equal(sFlag, ls_Thread) )
	{
		retVal = TEST_THREAD;
	}
	else if( g_string_equal(sFlag, ls_ThreadMx) )
	{
		retVal = TEST_THREADANDMUTEX;
	}
	else {/* Misra */}

	return retVal;

}
