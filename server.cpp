#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <time.h>
#include "interface.h"
#pragma comment(lib, "Rpcrt4.lib")
using namespace std;

void Output(unsigned char* szOutput)
{
	std::cout << szOutput << std::endl;
}

RPC_STATUS CALLBACK SecurityCallback(RPC_IF_HANDLE, void*)													
{
	return RPC_S_OK;
}


int Delete_File_on_the_server(unsigned char* path)
{
	char message[100];
	if (remove((const char*)path) != 0)
	{
		strcpy(message, " File has not been deleted ");
		strcat(message, (const char*)path);
		printf("%s\n", message);
		return 0;
	}
	else
	{
		strcpy(message, (const char*)path);
		strcat(message, " File has been deleted");
		printf("%s\n", message);
		return 1;
	}
}

int Create_File_on_the_server(unsigned char* path, unsigned char* content)
{
	char message[100];
	ofstream file((const char*)path);
	if (!file)
	{
		strcpy(message, " File has not been created");
		strcat(message, (const char*)path);
		printf("%s\n", message);
		return 0;
	}
	else
	{
		file << content << endl;
		file.close();
		strcpy(message, (const char*)path);
		strcat(message, " File has been created");
		printf("%s\n", message);
		return 1;
	}
}

int Copy_Fileon_to_the_server(unsigned char *file_name, unsigned char *path_server, unsigned char* content, int *size)
{
	char message[100];
	string name, path;
	path = (const char *)path_server;
	if (path_server[strlen((const  char *)path_server) - 1] != '\\')
	{
		path += "\\";
	}
	path += (const char *)file_name;
	FILE* f = NULL;
	f = fopen(path.c_str(), "wb");
	if (f == NULL)
	{
		strcpy(message, " File has not been copied ");
		strcat(message, path.c_str());
		printf("%s\n", message);
		return 0;
	}
	fseek(f, 0, SEEK_SET);
	fwrite(content, sizeof(unsigned char), *size, f);
	fclose(f);
	strcpy(message, path.c_str());
	strcat(message, " File has been copied");
	printf("%s\n", message);
	return 1;
}

int Get_Size_of_file(unsigned char *path_file, int *size)
{
	printf("Filepath is %s\n", (const char *)path_file);
	FILE* f = fopen((const char *)path_file, "rb");
	if (f == NULL)
	{
		printf("this file doesn't exist\n");
		return 0;
	}
	fseek(f, 0, SEEK_END);
	*size = ftell(f);
	fseek(f, 0, SEEK_SET);
	fclose(f);
	return 1;
}

int Download_File_from_the_server(unsigned char *path_server, unsigned char* content, int *size)
{
	printf("downloading...\n");
	char message[100];
	FILE* f = fopen((const char *)path_server, "rb");
	if (f == NULL)
	{
		strcpy(message, " File has not been downloaded ");
		strcat(message, (const char *)path_server);
		printf("%s\n", message);
		return 0;
	}
	fseek(f, 0, SEEK_SET);
	fread(content, sizeof(unsigned char), *size, f);
	fclose(f);
	strcpy(message, (const char *)path_server);
	strcat(message, " File has been downloaded");
	printf("%s\n", message);
	return 1;
}

int Impersonation(unsigned char *user, unsigned char *password)
{
	char message[100];
	handle_t handle;
	if (LogonUser((const char *)user, NULL, (const char *)password, LOGON32_LOGON_NEW_CREDENTIALS, LOGON32_PROVIDER_WINNT50, &handle) == 0)
	{
		printf("first error\n"); return 1;
	}
																																				
	else
	{
		if (ImpersonateLoggedOnUser(handle) == 0) 
		{
			printf("second error\n"); return 2;
		}
		else
		{
			strcpy(message, "User ");
			strcat(message, (const char *)user);
			strcat(message, " has loged in");
			printf("%s\n", message);
			return 0;
		}
	}
}
int Log_out()
{
	char message[100];
	if (RevertToSelf() != 0) 
	{
		strcpy(message, "User has loged out");
		printf("%s\n", message);
		return 1;
	}
	else return 0; 
}

int main()
{
	RPC_STATUS status;
	cout << "Waiting for clients\n";
	status = RpcServerUseProtseqEp((RPC_CSTR)("ncacn_ip_tcp"), RPC_C_PROTSEQ_MAX_REQS_DEFAULT, (RPC_CSTR)("9000"), NULL);
	if (status)
	{
		exit(status);
	}
	status = RpcServerRegisterIf2(InterfaceRPC_v1_0_s_ifspec, NULL, NULL, RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH, RPC_C_LISTEN_MAX_CALLS_DEFAULT, (unsigned)-1, SecurityCallback);
	if (status)
	{
		exit(status);
	}
	status = RpcServerListen(1, RPC_C_LISTEN_MAX_CALLS_DEFAULT,0);

	if (status)
	{
		exit(status);
	}
}
void* __RPC_USER midl_user_allocate(size_t size)
{
	return malloc(size);
}
void __RPC_USER midl_user_free(void* p)
{
	free(p);
}