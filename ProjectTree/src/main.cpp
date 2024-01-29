#include<iostream>
#include <cstdint>
#include"C_HeaderFiles.hpp"
/* Defines */
#define LOAD					(1000u)
#define LOCAL_BUFFER_SIZE		(1000u)
#define SOCKET_NAME				"Socket.test"
#define MSG_TO_SEND 			"Hello from 2024, Happy new year!!! Update the status. Let's go on holidays to The Universitare Psychiatrische Dienste (UPD)."
#define CANCELE_TIMEOUT			(10)
#define NO_OF_ATTEMPTS			(100)

/* User-defined data type  */
typedef enum{
	NONE = 0,
	SERVER_SIDE,
	CLIENT_SIDE
}TE_CONNECTION;

typedef struct{
	TE_CONNECTION conectionType;
	GSocket* connectionPtr;
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
gboolean WriteToSocket(GSocket* socketPtr, guint8* data, gsize size);
void ReadFromSocket(GSocket* socketPtr, guint8* data, gsize size, gsize* readBytes);
GSocket* CreateServer(void);
GSocket* Connect2Server(void);
void ChildBackend(void);
void ParentBackend(void);
guint CountStrinStr(GString* str, gchar* find);
gpointer CancAtTimeout_thread (gpointer data);
void CleanSocket(void);

static gboolean b_UseSocket = FALSE;
static gboolean b_UseTread = FALSE;
static gboolean b_UseTreadWMx = FALSE;

static GOptionEntry entries[] =
{
	{ "UseSocket", 'S', 0, G_OPTION_ARG_NONE, &b_UseSocket, "Transmit data through sockets", NULL },
	{ "UseThread", 'T', 0, G_OPTION_ARG_NONE, &b_UseTread, "The threads increment and print the same global variable", NULL },
	{ "UseThreadWithMutex", 'M', 0, G_OPTION_ARG_NONE, &b_UseTread, "The threads increment and print the same global variable protected with mutex", NULL },
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

	if(G_IS_SOCKET(s_clientConn.connectionPtr)){
		g_socket_close(s_clientConn.connectionPtr, NULL);
		g_object_unref(s_clientConn.connectionPtr);
	}
}

void ChildProcess(void)
{
	std::cout << "Child process " << getpid() << " was called." << std::endl;

	if(OpenSocket(&serverConn))
	{
		std::cout << "Conection created successfully, in " << getpid() <<" process(child)." << std::endl;

		ChildBackend();

	}

	if(G_IS_SOCKET(serverConn.connectionPtr)){
		g_socket_close(serverConn.connectionPtr, NULL);
		g_object_unref(serverConn.connectionPtr);

		CleanSocket();
	}
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
	GSocket* lConectionPtr = NULL;

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
		RetVal = TRUE;
	}
	return RetVal;
}

GSocket* CreateServer(void)
{
	GError *lerror = NULL;
	GCancellable *lcancellablePtr = NULL;
	GSocketAddress *lSocketAdrPtr = NULL;
	GSocket* lnewsocketptr = NULL;
	GSocket* lsocketptr = g_socket_new (G_SOCKET_FAMILY_UNIX, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_DEFAULT, &lerror);
	if(NULL != lerror){
		g_warning ("Could not create server socket %s", lerror->message);
	}
	else{
		lSocketAdrPtr = g_unix_socket_address_new (SOCKET_NAME);
		g_clear_error (&lerror);
		/* normally allow_reuse should be TRUE for server sockets in order to not get G_IO_ERROR_ADDRESS_IN_USE*/
		if( FALSE == g_socket_bind (lsocketptr, lSocketAdrPtr, TRUE, &lerror)){
			g_warning ("Can't bind socket: %s\n", lerror->message);
		}
		else{
			g_clear_error (&lerror);
			/* Ensure that after timeout a listen funciton will be terminated */
			if(FALSE == g_socket_listen ( lsocketptr, &lerror) ){
				if(NULL != lerror){
					g_warning ("Can't listen on the socket: %s\n", lerror->message);
				}
			}
			else{
				g_clear_error (&lerror);
				/* Ensure that after timeout a listen funciton will be terminated */
				if( FALSE == g_socket_condition_timed_wait(lsocketptr, G_IO_IN, (G_USEC_PER_SEC * CANCELE_TIMEOUT), NULL, &lerror ) ){
					if(NULL != lerror){
						g_warning ("Timeout ocurred, and no client connected: %s\n", lerror->message);
					}
				}
				else{
					lcancellablePtr = g_cancellable_new ();
					g_thread_new(NULL, CancAtTimeout_thread, lcancellablePtr);
					g_clear_error (&lerror);
					lnewsocketptr = g_socket_accept (lsocketptr, lcancellablePtr, &lerror);
					if(NULL != lerror)
					{
						g_warning ("Can't accept the client: %s\n", lerror->message);
					}
					else{
						g_clear_error (&lerror);
						if(NULL != lerror){
							g_warning("Error getting remote address: %s\n", lerror->message);
						}
					}
				}
			}
				
		}
	}
	g_object_unref (G_OBJECT (lsocketptr));
	return lnewsocketptr;
}


GSocket* Connect2Server(void)
{
	int l_ConnectionAttemps = NO_OF_ATTEMPTS;
	GSocketAddress *lClinetAdrPtr = NULL;
	GSocket* lnewsocketptr = NULL;
	GError * lerror = NULL;
	gboolean lbConnected = FALSE;
	lClinetAdrPtr = g_unix_socket_address_new (SOCKET_NAME);
	while( (0 < l_ConnectionAttemps) && (FALSE == lbConnected) )
	{
		lnewsocketptr = g_socket_new (G_SOCKET_FAMILY_UNIX, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_DEFAULT, &lerror);
		if(NULL != lerror){
			g_warning ("Could not create client socket %s", lerror->message);
		}else{
			g_clear_error (&lerror);
			if(FALSE == g_socket_connect ( lnewsocketptr, lClinetAdrPtr, NULL, &lerror))
			{
				if(NULL != lerror){
					g_warning ("Could not connect client socket %s", lerror->message);
				}
				g_clear_error (&lerror);
				g_socket_close (lnewsocketptr, &lerror);
				if(NULL != lerror){
					g_error ("Could not close client socket %s", lerror->message);
				}
				g_object_unref (G_OBJECT (lnewsocketptr));
				g_clear_error (&lerror);
			}
			else{
				lbConnected = TRUE;
			}
		}

		if(0 < l_ConnectionAttemps){
			l_ConnectionAttemps--;
		}
	}
	return lnewsocketptr;
}

gboolean WriteToSocket(GSocket* socketPtr, guint8* data, gsize  size)
{
	gboolean retVal = FALSE;
	GError * lerror = NULL;
	gssize writtenbytes = g_socket_send (socketPtr, (gchar*)data, size, NULL, &lerror);

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

void ReadFromSocket(GSocket* socketPtr, guint8* data, gsize size, gsize* readBytes)
{
	GError * lerror = NULL;
	gssize lreadBytes = g_socket_receive(socketPtr, (gchar*)data, size, NULL, &lerror);
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
	ReadFromSocket(serverConn.connectionPtr , receivedMsg, LOCAL_BUFFER_SIZE, &receivedBytes);

	if(0 != receivedBytes)
	{
		std::cout << "Message:" << std::endl << receivedMsg << std::endl <<" received." << std::endl;
	}

	GString* lsMsg = g_string_new((const gchar*) receivedMsg);
	GError * lerror = NULL;
	GRegex *cleanup = g_regex_new ("[0-9]", G_REGEX_DEFAULT, G_REGEX_MATCH_DEFAULT, &lerror);
	if(NULL != lerror){
		g_error ("Error occured during regex creation: %s", lerror->message);
	}
	g_clear_error (&lerror);
	gchar* ls_result = g_regex_replace (cleanup, (gchar*)receivedMsg, strlen((char*)receivedMsg), 0, "\0", G_REGEX_MATCH_DEFAULT , &lerror);
	if(NULL != lerror){
		g_error ("Error occured during regex replace: %s", lerror->message);
	}
	g_regex_unref(cleanup);

	if (WriteToSocket(serverConn.connectionPtr, (guint8*)ls_result, (strlen((char*)ls_result)+1)))
	{
		std::cout << "Server returned the message" << std::endl;
	}
	g_free(ls_result);

	guint lNoOfFind;
	gchar lfindStr[] = "UPD";

	lNoOfFind = CountStrinStr(lsMsg, lfindStr);

	if( WriteToSocket(serverConn.connectionPtr, (guint8*)&lNoOfFind, sizeof(guint)) )
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
	if (WriteToSocket(s_clientConn.connectionPtr, (guint8*)Msg->str, ((gsize)Msg->len)+1))
	{
		std::cout << "Client sent msg successfully" << std::endl;
	}
	

	ReadFromSocket(s_clientConn.connectionPtr , receivedMsg, LOCAL_BUFFER_SIZE, &receivedBytes);
	if(0 != receivedBytes)
	{
		std::cout << "Returned message is:" << std::endl << receivedMsg << std::endl;
	}


	guint lNoOfFind;
	gchar lfind[] = "UPD";
	lNoOfFind = CountStrinStr(Msg, lfind);
	std::cout << "String \"upd\" was found: "<< lNoOfFind << " times" << std::endl;

	ReadFromSocket(s_clientConn.connectionPtr , (guint8*)&lNoOfFind, sizeof(guint), &receivedBytes);
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


gpointer CancAtTimeout_thread (gpointer data)
{
  GCancellable *cancellable = (GCancellable*)data;
  std::cout << "Thread called" << std::endl;
  g_usleep (G_USEC_PER_SEC*CANCELE_TIMEOUT);
  std::cout << "Timeout ocurred" << std::endl;
  g_cancellable_cancel (cancellable);
  return NULL;
}

void CleanSocket(void)
{
	if( g_file_test(SOCKET_NAME, G_FILE_TEST_EXISTS)){
		g_warning ("Exists");
		unlink (SOCKET_NAME);
	} 
	else{
		g_warning ("Do not exist");
	}

}
