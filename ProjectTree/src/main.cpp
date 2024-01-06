#include<iostream>

#include"glib.hpp"

#include"test.hpp"


int main(int argc, char *argv[])
{
	uint32_t result;
	std::cout << "Hello, Student" << std::endl;
	result = CalcSumm(50, 40);
	std::cout << "Sum is: " << result << std::endl;

	gint child_stdout, child_stderr;
	GPid child_pid;
	g_autoptr(GError) error = NULL;
	const gchar * const largv[] = { "\./PrintMsg.elf", NULL };
	std::cout << "Process " << getpid() << " started" << std::endl;


	if( g_spawn_async(NULL, (gchar**)largv, NULL, G_SPAWN_DEFAULT, NULL, NULL, &child_pid, &error))
	{
		std::cout << "New process " << child_pid << " created successfully" << std::endl;

		std::cout << "The No of arguments is " << argc << std::endl;
		for(int argInx = 0u; argInx < argc; argInx++)
		{
			std::cout << "The argument " << argInx <<" is " << argv[argInx] << std::endl;
		}

		return 0;
	}
	else
	{

		if (error != NULL)
		{
			g_error ("Spawning child failed: %s", error->message);
			return 1;
		}
	}
}
