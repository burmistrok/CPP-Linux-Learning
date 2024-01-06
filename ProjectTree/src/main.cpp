#include<iostream>

#include"glib.hpp"

#include"test.hpp"


int main(int argc, char *argv[])
{
	GSubprocess * childProcess = NULL;
	GError * lerror = NULL;
	const gchar * const largv[] = { "\./PrintMsg.elf", NULL };
	GSubprocessLauncher *handlelaunch = NULL;

	std::cout << "Process " << getpid() << " started" << std::endl;


	handlelaunch = g_subprocess_launcher_new((GSubprocessFlags)(G_SUBPROCESS_FLAGS_STDOUT_PIPE | G_SUBPROCESS_FLAGS_STDERR_MERGE));

	std::cout << "g_subprocess_launcher_new passed "<< std::endl;

	childProcess = g_subprocess_launcher_spawn ( handlelaunch, &lerror, (const gchar*)largv, NULL );

	if(lerror != NULL)
	{
		g_error ("Spawning child failed: %s", lerror->message);
		return 1;
	}
}
