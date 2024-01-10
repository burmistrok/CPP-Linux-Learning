#include<iostream>
#include <cstdint>
#include"C_HeaderFiles.hpp"

/* Defines */
#define LOAD					(1000u)
#define LOCAL_BUFFER_SIZE		(1000u)
#define CUSTMPORT 				(9000u)
#define HOSTNAME 				"127.0.0.1"
#define MSG_TO_SEND 			"Hello from 2024, Happy new year!!! Update the status. Let's go on holidays to The Universitare Psychiatrische Dienste (UPD)."

/* User-defined data type  */

typedef enum{
	MAIN_PROCESS = 0,
	CHILD_PROCESS,
	TEST_THREAD,
	TEST_THREADANDMUTEX,

	UNSUPORTED_GOAL
}TE_GOAL;

typedef enum{
	NONE = 0,
	SERVER_SIDE,
	CLIENT_SIDE
}TE_CONNECTION;

typedef struct{
	TE_CONNECTION conectionType;
	GSocketConnection* connectionPtr;
	GInputStream* istreamPtr;
	GOutputStream* ostreamPtr;
}TS_CONNECITON;

typedef struct{
	GPid childPid;
	TS_CONNECITON* Connection;
}TS_PROCESS_CFG;

/*Functions*/
void MainProcess(void);
void ChildProcess(void);
void TestMultiThread(uint8_t useMutex);
void Thread1_Func( gpointer data );
gboolean OpenSocket(TS_CONNECITON* connPtr);
gboolean WriteToSocket(GOutputStream* ostreamptr, guint8* data, gsize size);
void ReadFromSocket(GInputStream* istreamptr, guint8* data, gsize size, gsize* readBytes);
GSocketConnection* CreateServer(void);
GSocketConnection* Connect2Server(void);
void ChildBackend(void);
void ParentBackend(void);
guint CountStrinStr(GString* str, gchar* find);

static gboolean b_UseSocket = FALSE;
static gboolean b_UseTread = FALSE;
static gboolean b_UseTreadWMx = FALSE;

static GOptionEntry entries[] =
{
	{ "UseSocket", 'S', 0, G_OPTION_ARG_NONE, &b_UseSocket, "Transmit data through sockets", NULL },
	{ "UseThread", 'T', 0, G_OPTION_ARG_NONE, &b_UseTread, "The threads increment and print the same global variable", NULL },
	{ "UseThreadWithMutex", 'M', 0, G_OPTION_ARG_NONE, &b_UseTread, "The threads increment and print the same global variable", NULL },
	{ NULL }
};

/* Variabiles */
uint32_t SharedResurse = 0ul;
GMutex *mutex;

TS_CONNECITON serverConn = 
{
	.conectionType = SERVER_SIDE,
};

TS_CONNECITON s_clientConn = 
{
	.conectionType = CLIENT_SIDE,
};


TS_PROCESS_CFG MainProcessCfg = 
{
	.Connection = &s_clientConn,
};


int main( int argc, char **argv )
{
	int RetVal = 0;

	GError *error = NULL;
	GOptionContext *context;

	context = g_option_context_new ("- test tree model performance");
	g_option_context_add_main_entries (context, entries, "Test");
	g_option_context_add_group (context, gtk_get_option_group (TRUE));
	if (!g_option_context_parse (context, &argc, &argv, &error))
	{
		g_error ("option parsing failed: %s\n", error->message);
		RetVal = 1;
	}


	if(0 == RetVal){
		if(FALSE != b_UseSocket){
			int chid = fork();
			if(0 > chid){
				g_error ("Error appear at fork processn");
				RetVal = 2;
			}
			else if(0 == chid){
				ChildProcess();
			}
			else{
				std::cout << "The child pid is: " << chid << std::endl;
				MainProcess();
				int status;
				wait(&status);
				std::cout << "The result from " << chid << " is: " << status << std::endl;
			}
		}
		else if(FALSE != b_UseTread){
			TestMultiThread(0u);
		}
		else if(FALSE != b_UseTreadWMx){
			TestMultiThread(1u);
		}
		else{
			g_error ("Incorect option transmited\n");
			RetVal = 2;
		}
	}
	std::cout << "Exit code from " << getpid() <<" process is "<< RetVal << std::endl;
	return RetVal;
}

void MainProcess(void)
{

	std::cout << "Main " << getpid() <<" process was called" << std::endl;

	if( OpenSocket(&s_clientConn) )
	{
		std::cout << "Conection created successfully, in " << getpid() <<" process(main)." << std::endl;

		ParentBackend();
	}

	g_object_unref(s_clientConn.connectionPtr);
}

void ChildProcess(void)
{
	std::cout << "Child process " << getpid() << " was called." << std::endl;

	if(OpenSocket(&serverConn))
	{
		std::cout << "Conection created successfully, in " << getpid() <<" process(child)." << std::endl;

		ChildBackend();

	}
	g_object_unref(serverConn.connectionPtr);
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

gboolean OpenSocket(TS_CONNECITON* connPtr)
{

	gboolean RetVal = FALSE;
	GSocketConnection* lConectionPtr = NULL;

	if( SERVER_SIDE == connPtr->conectionType )
	{
		lConectionPtr = CreateServer();
	}
	else if ( CLIENT_SIDE == connPtr->conectionType )
	{
		lConectionPtr = Connect2Server();
	}	
	else{
		g_error ("Connection type is not defined");
	}

	if(NULL != lConectionPtr )
	{
		connPtr->connectionPtr = lConectionPtr;
		connPtr->istreamPtr = g_io_stream_get_input_stream(G_IO_STREAM(lConectionPtr));
		connPtr->ostreamPtr = g_io_stream_get_output_stream(G_IO_STREAM(lConectionPtr));
		RetVal = TRUE;
	}
	return RetVal;
}

GSocketConnection* CreateServer(void)
{
	GError * lerror = NULL;
	GSocketConnection* new_connectionPtr = NULL;
	GSocketListener *lListnSocketPtr = g_socket_listener_new();
	if(NULL == lListnSocketPtr)
	{
		g_error ("Could not create server socket.");
	}
	else
	{
		g_clear_error (&lerror);
		g_socket_listener_add_inet_port(lListnSocketPtr, (guint16 )CUSTMPORT, NULL, &lerror);

		if(lerror != NULL)
		{
			g_error ("Could not add listner: %s", lerror->message);
		}
		else
		{
			g_clear_error (&lerror);
			new_connectionPtr = g_socket_listener_accept(lListnSocketPtr, NULL, NULL, &lerror);
			if(lerror != NULL)
			{
				g_error ("Could not create new conection: %s", lerror->message);
				new_connectionPtr = NULL;
				g_socket_listener_close(lListnSocketPtr);
				g_object_unref (lListnSocketPtr);
			}
		}
	}
	g_socket_listener_close(lListnSocketPtr);
	g_object_unref (lListnSocketPtr);

	return new_connectionPtr;
}

GSocketConnection* Connect2Server(void)
{
	GError * lerror = NULL;
	GSocketConnection* new_connectionPtr = NULL;
	//sleep(1);
	GSocketClient* lClientSocketPtr  = g_socket_client_new ();
	new_connectionPtr = g_socket_client_connect_to_host ( lClientSocketPtr, HOSTNAME, CUSTMPORT , NULL,  &lerror);
	if(lerror != NULL)
	{
		g_error ("Could not connect to host: %s", lerror->message);
		new_connectionPtr = NULL;
		g_object_unref (lClientSocketPtr);
	}

	g_object_unref (lClientSocketPtr);
	return new_connectionPtr;
}

gboolean WriteToSocket(GOutputStream* ostreamptr, guint8* data, gsize  size)
{
	gboolean retVal = FALSE;
	GError * lerror = NULL;
	gssize writtenbytes = g_output_stream_write (ostreamptr, data, size, NULL, &lerror);

	if(lerror != NULL)
	{
		g_error ("Error occured during write: %s", lerror->message);
	}
	else
	{
		if( (0 < writtenbytes) && ((gsize)writtenbytes == size) )
		{
			retVal = TRUE;
		}
	}

	return retVal;
}

void ReadFromSocket(GInputStream* istreamptr, guint8* data, gsize size, gsize* readBytes)
{
	GError * lerror = NULL;
	gssize lreadBytes = g_input_stream_read(istreamptr, data, size, NULL, &lerror);
	if(lerror != NULL)
	{
		g_error ("Error occured during reading: %s", lerror->message);
	}
	else
	{
		if(0 < lreadBytes)
		{
			*readBytes = (gsize)lreadBytes;
		}
	}
}

void ChildBackend(void)
{
	guint8 receivedMsg[LOCAL_BUFFER_SIZE];
	gsize receivedBytes = 0;
	ReadFromSocket(serverConn.istreamPtr , receivedMsg, LOCAL_BUFFER_SIZE, &receivedBytes);

	if(0 != receivedBytes)
	{
		std::cout << "Message:" << std::endl << receivedMsg << std::endl <<" received." << std::endl;
	}

	GString* lsMsg = g_string_new((const gchar*) receivedMsg);

/*g_regex_replace */
	gchar lfind[2] = "0";
	gchar lreplase = '\0';
	for( gchar sym = '0'; sym <= '9'; sym++ )
	{
		lfind[0] = sym;
		g_string_replace (lsMsg, lfind, &lreplase, 0);
	}

	if (WriteToSocket(serverConn.ostreamPtr, (guint8*)lsMsg->str, ((gsize)lsMsg->len)+1))
	{
		std::cout << "Server returned the message" << std::endl;
	}

	guint lNoOfFind;
	gchar lfindStr[] = "UPD";

	lNoOfFind = CountStrinStr(lsMsg, lfindStr);

	if( WriteToSocket(serverConn.ostreamPtr, (guint8*)&lNoOfFind, sizeof(guint)) )
	{
		std::cout << "Server returned the message" << std::endl;
	}

	g_string_free(lsMsg, TRUE);
}


void ParentBackend(void)
{
	guint8 receivedMsg[LOCAL_BUFFER_SIZE];
	gsize receivedBytes = 0;
	GString* Msg = g_string_new((gchar*)MSG_TO_SEND);
	if (WriteToSocket(s_clientConn.ostreamPtr, (guint8*)Msg->str, ((gsize)Msg->len)+1))
	{
		std::cout << "Client sent msg successfully" << std::endl;
	}
	

	ReadFromSocket(s_clientConn.istreamPtr , receivedMsg, LOCAL_BUFFER_SIZE, &receivedBytes);
	if(0 != receivedBytes)
	{
		std::cout << "Returned message is:" << std::endl << receivedMsg << std::endl;
	}


	guint lNoOfFind;
	gchar lfind[] = "UPD";
	lNoOfFind = CountStrinStr(Msg, lfind);
	std::cout << "String \"upd\" was found: "<< lNoOfFind << " times" << std::endl;

	ReadFromSocket(s_clientConn.istreamPtr , (guint8*)&lNoOfFind, sizeof(guint), &receivedBytes);
	if(0 != receivedBytes)
	{
		std::cout << "Server found string \"upd\": "<< lNoOfFind << " times" << std::endl;
	}

	gchar *str = g_string_free_and_steal(Msg);
	g_free (str);
}


guint CountStrinStr(GString* str, gchar* find)
{
	str = g_string_ascii_up(str);
	return g_string_replace (str, find, find, 0);
}