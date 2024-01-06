#include<iostream>

#include"glib.hpp"

#include"test.hpp"

#define LOAD		(1000u)


uint32_t SharedResurse = 0ul;

void Thread1_Func(void);

GMutex *mutex;

int main()
{
	uint32_t result;
	std::cout << "Hello, Student" << std::endl;
	result = CalcSumm(50, 40);
	std::cout << "Sum is: " << result << std::endl;



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

	return 0;
}


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