#include<iostream>

#include"glib.hpp"

#include"test.hpp"

#include <sys/types.h>
#include <sys/wait.h>


class test{

public: 
	int Data;
	test()
	{
		std::cout << "Class test created in :" << getpid() << " process" << std::endl;
	}
	~test()
	{
		std::cout << "Destructor called for class test from :" << getpid() << " process" << std::endl;
	}

};

int main(int argc, char *argv[])
{
	uint32_t result;

	char *HeapData = (char*)malloc(100);
	unsigned long int DataFromPtr = (unsigned long int)HeapData;

	test *NewObject = new test();
	NewObject->Data = 5;

	int chid = fork();
	if(0 > chid)
	{
		return 1;
	}

	std::cout << "Hello, Student" << std::endl;
	result = CalcSumm(50, 40);
	std::cout << "Sum is: " << result << std::endl;

	std::cout << "The address of local variable created before fork is: " << &result << std::endl;



	std::cout << "The No of arguments is " << argc << std::endl;
	for(int argInx = 0u; argInx < argc; argInx++)
	{
		std::cout << "The argument " << argInx <<" is " << argv[argInx] << std::endl;
	}
	if(0 == chid)
	{

		char Normalmsg[] = "Write to stdout of child process\r\n";
		char Errmsg[] = "Write to stderr of child process\r\n";
		for (int i = 0u; (Normalmsg[i] != '\0'); i++)
		{
			HeapData[i] = Normalmsg[i];
		}
		std::cout << "The address of pointer is: " << &HeapData << std::endl;
		std::cout << "The value of pointer is: " << HeapData << std::endl;
		std::cout << "The content of pointer is: " << *HeapData << std::endl;
		std::cout << "The pointer to pointer is: " << DataFromPtr << std::endl;
		std::cout << "\n\n" << std::endl;
		write(STDOUT_FILENO, Normalmsg, sizeof(Normalmsg));
		write(STDERR_FILENO, Errmsg, sizeof(Errmsg));

		delete(NewObject);

		exit(5);
	}
	else
	{
		std::cout << "The child pid is: " << chid << std::endl;
		int status;
		wait(&status);
		status = WEXITSTATUS(status);
		std::cout << "The result from " << chid << " is: " << status << std::endl;
		std::cout << "The address of pointer is: " << &HeapData << std::endl;
		std::cout << "The value of pointer is: " << HeapData << std::endl;
		std::cout << "The content of pointer is: " << *HeapData << std::endl;
		std::cout << "The pointer to pointer is: " << DataFromPtr << std::endl;
		std::cout << "\n\n" << std::endl;


		delete(NewObject);

	}

	return 0;
}
