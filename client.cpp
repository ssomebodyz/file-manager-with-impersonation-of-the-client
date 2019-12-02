#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "interface.h"
#include <fstream>
#include <string>
#include <windows.h>
#include <conio.h>
#include <cstdio>
#pragma comment(lib, "Rpcrt4.lib")
using namespace std;

using namespace std;
bool flag = false;
class functions
{
public:
	void delete_file()
	{
		string path;
		unsigned char *path_ch;
		cout << "Enter path: ";
		getline(cin, path);
		path_ch = (unsigned char *)path.c_str();
		if (Delete_File_on_the_server(path_ch) == 1) cout << "File is deleted" << endl;
		else cout << "File wasn't deleted" << endl;
	}
	void copy_file()
	{
		string cl_path, serv_path;
		unsigned char *file_name_ch, *serv_path_ch;
		unsigned char *content;
		int size;
		cout << "Enter client file path " << endl;
		getline(cin, cl_path);
		FILE* f = fopen(cl_path.c_str(), "rb");
		if (f == NULL)
		{
			cout << "File wasn't opened" << endl;
			return;
		}
		unsigned found = cl_path.find_last_of("/\\");
		cl_path = cl_path.substr(found + 1);
		file_name_ch = (unsigned char *)cl_path.c_str();
		cout << "Enter server file path " << endl;
		getline(cin, serv_path);
		serv_path_ch = (unsigned char *)serv_path.c_str();
		fseek(f, 0, SEEK_END);
		size = ftell(f);
		content = (unsigned char *)malloc(size);
		if (content == 0)
		{
			cout << "Memory fault" << endl;
			return;
		}
		fseek(f, 0, SEEK_SET);
		fread(content, sizeof(unsigned char), size, f);
		if (Copy_Fileon_to_the_server(file_name_ch, serv_path_ch, content, &size) == 1) cout << "File was copied" << endl;
		else cout << "File wasn't copied" << endl;
		free(content);
		fclose(f);
	}
	void download_file()
	{
		unsigned char *serv_path_ch;
		unsigned char *content;
		int size;
		string serv_path, client_path;
		cout << "Enter server file path " << endl;
		getline(cin, serv_path);
		serv_path_ch = (unsigned char *)serv_path.c_str();
		cout << "Enter location of file on client:" << endl;
		getline(cin, client_path);
		if (client_path[client_path.length() - 1] != '\\') client_path += "\\";
		unsigned found = serv_path.find_last_of("/\\");
		client_path += serv_path.substr(found + 1);
		if (Get_Size_of_file(serv_path_ch, &size) == 1)
		{
			content = (unsigned char *)malloc(size);
			if (content == 0)
			{
				cout << "Memory fault" << endl;
				return;
			}
			if (Download_File_from_the_server(serv_path_ch, content, &size) == 1)
			{
				FILE* f = fopen(client_path.c_str(), "wb");
				if (f == NULL)
				{
					cout << "File wasn't opened" << endl;
					return;
				}
				fseek(f, 0, SEEK_SET);
				fwrite(content, sizeof(unsigned char), size, f);
				fclose(f);
				cout << "File is downloaded" << endl;
			}
			else cout << "File wasn't downloaded" << endl;
			free(content);
		}
		else cout << "File wasn't downloaded" << endl;
	}
	int login()
	{
		const char *usre_name, *password_of_user;
		string user, password;
		int func;
		while (true)
		{
			cout << "Enter login: ";
			cin >> user;
			cout << endl;
			usre_name = user.c_str();
			cout << "Enter password: ";
			cin >> password;
			cout << endl;
			password_of_user = password.c_str();
			func = Impersonation((unsigned char *)usre_name, (unsigned char *)password_of_user);
			if (func == 0)
			{
				return 0;
				printf("error with Impersonation\n");
				system("pause");
			}
			if (func == 1)
			{
				cout << "Incorrect password" << endl;
			}
			if (func == 2)
			{
				cout << "Error with autentification" << endl;
			}
		}
	}
	void log_out()
	{
		if (Log_out() == 1)
		{
			flag = false;
		}
		else cout << "Error" << endl;
	}
};
functions commands;
void menu()
{
	cout << "\n Commands:";
	cout << "\n 1) Delete file";
	cout << "\n 2) Copy file";
	cout << "\n 3) Download file";
	cout << "\n 4) Exit";
	cout << "\n (!)" << endl;
	string command;
	getline(cin, command);
	getline(cin, command);
	if (command == "1") { commands.delete_file(); }
	else if (command == "2") { commands.copy_file(); }
	else if (command == "3") { commands.download_file(); }
	else if (command == "4") { commands.log_out(); }
	else { cout << "Unknown command" << endl; }
}
int main()
{
	RPC_STATUS status;
	unsigned char* szStringBinding = NULL;
	unsigned char address[100];
	flag = false;
	cout << "Enter server ip address: ";
	cin >> address;
	cout << endl;
	status = RpcStringBindingCompose((RPC_CSTR)"40d8e656-fe6f-11e9-aad5-362b9e155667", (RPC_CSTR)("ncacn_ip_tcp"), (RPC_CSTR)(address), (RPC_CSTR)("9000"), NULL, &szStringBinding); 
	if (status)
	{
		exit(status);
		system("pause");
	}
	status = RpcBindingFromStringBindingA(szStringBinding,&InterfaceRPCbind); 
	if (status)
	{
		exit(status);
		system("pause");
	}
	RpcTryExcept
	{
		while (true)
		{
			status = RpcMgmtIsServerListening(InterfaceRPCbind);
			if (status)
			{
				if (status == RPC_S_NOT_LISTENING)
				{
					cout << "Server is down, sorry" << endl;
					main();
				}
				else
				{
					if (flag == true)
					{
						menu();
					}
					else
					{
						commands.login();
						flag = true;
					}

				}
			}
		}
	}
	RpcExcept(1)
	{
		std::cerr << "Too much connections" << RpcExceptionCode() << std::endl;
	}
	RpcEndExcept
	status = RpcStringFree(&szStringBinding);
	if (status)
	{
		exit(status);
		system("pause");
	}
	status = RpcBindingFree(&InterfaceRPCbind); 
	if (status)
	{
		exit(status);
		system("pause");
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