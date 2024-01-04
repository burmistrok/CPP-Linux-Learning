#include<iostream>

#include"glib.hpp"

#include"test.hpp"

#include <sys/types.h>
#include <sys/wait.h>

#define LOAD		(1000u)


#ifdef USE_THREAD_MY
uint32_t SharedResurse = 0ul;

void Thread1_Func(void);

GMutex *mutex;
#endif

int main(int argc, char *argv[])
{
	uint32_t result;

	int chid = fork();
	if(0 > chid)
	{
		return 1;
	}

	std::cout << "Hello, Student" << std::endl;
	result = CalcSumm(50, 40);
	std::cout << "Sum is: " << result << std::endl;



	std::cout << "The No of arguments is " << argc << std::endl;
	for(int argInx = 0u; argInx < argc; argInx++)
	{
		std::cout << "The argument " << argInx <<" is " << argv[argInx] << std::endl;
	}
	if(0 == chid)
	{
		exit(5);
	}

	if(0 != chid)
	{
		std::cout << "The child pid is: " << chid << std::endl;
		int status;
		wait(&status);
		std::cout << "The result from " << chid << " is: " << status << std::endl;
	}

#ifdef USE_THREAD_MY
	mutex = g_new(GMutex, 1);
	g_mutex_init(mutex);
	GThread * Thread1 = g_thread_new(NULL, (GThreadFunc) Thread1_Func, NULL);

	for(uint32_t i = 0u; i < LOAD; i++)
	{
		g_mutex_lock(mutex);
		SharedResurse++;
		std::cout << "MainThread:" << SharedResurse << std::endl;
		g_mutex_unlock(mutex);
	}
	
	g_thread_join(Thread1);
#endif
	return 0;
}

#ifdef USE_THREAD_MY
void Thread1_Func(void)
{

	for(uint32_t i = 0u; i < LOAD; i++)
	{		
		g_mutex_lock(mutex);
		SharedResurse++;
		std::cout << "Thread1:" << SharedResurse << std::endl;
		g_mutex_unlock(mutex);
	}
}
#endif