#pragma once

#include"Session.h"
#include"define.h"

class Iocp
{
public:
	Iocp() 
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			return;
	};

	virtual ~Iocp()
	{
		WSACleanup();
	}

	bool InitSocketSet()
	{
		_listensocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
		if (_listensocket == INVALID_SOCKET)
		{
			cout << " Listen socket error " << endl;
			return false;
		}

		SOCKADDR_IN serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(PORT_NUM);
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(_listensocket, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR_IN)) != 0)
		{
			cout << " Bind error " << endl;
			return false;
		}

		if (listen(_listensocket, SOMAXCONN) != 0)
		{
			cout << " Listen error " << endl;
			return false;
		}
		cout << " Socket Set Complete " << endl;
		return true;
	}
	bool StartServer()
	{
		SOCKADDR_IN cl_addr;
		int addr_size = sizeof(cl_addr);

		_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
		if (_iocpHandle == NULL)
		{
			cout << " CreateIoCompletionPort() error " << endl;
			return false;
		}
		if (CreateIoCompletionPort(reinterpret_cast<HANDLE>(_listensocket), _iocpHandle, 9999, 0) == nullptr) // cp 등록
		{
			cout << " CreateIoCompletionPort() Regist Error " << endl;
			return false;
		}
		cout << " Read to Connect..." << endl;

		if (CreateWorkerThread() == false)
		{
			cout << " WorkerThread Error " << endl;
			return false;
		}
		if (CreateAcceptExThread() == false)
		{
			cout << " AcceptThread Error " << endl;
			return false;
		}

		return true;
	}

private:

	bool CreateWorkerThread()
	{
		unsigned int uiThreadId = 0;
		int num_thread = thread::hardware_concurrency();
		for (int i = 0; i < num_thread; ++i)
			_workerThreads.emplace_back([this]() {WorkerThread(); });
		for (auto& th : _workerThreads)
			th.join();

		cout << " Start Worker Thread .. " << endl;

		return true;
	}
	bool CreateAcceptExThread()
	{
		_acceptThread = thread([this]() {AccepterThread(); });
		return true;
	}

	void AccepterThread()
	{
		while (_acceptthreadRun)
		{
			//auto curTimeSec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
			for (auto& client : _clients)
			{
				if (client._state == STATE::ALLOC)
					continue;
				
				client.Acceptex(_listensocket);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(32));
		}
	}
	void WorkerThread()
	{
		ULONG_PTR key;
		DWORD dwIosize = 0;
		WSAOVERLAPPED* over = nullptr;

		while (_workerthreadRun)
		{
			BOOL ret = GetQueuedCompletionStatus(_iocpHandle, &dwIosize, &key, &over, INFINITE);
			OverEx* ex_over = reinterpret_cast<OverEx*>(over);
			if (ret == FALSE)
			{
				if (ex_over->_opstate == OPSTATE::ACCEPT)
					cout << " Accept Error " << endl;
				else
				{
					cout << " GQCS Error On Client [ " << key << " ]" << endl;
					// DISCONNCET
					if (ex_over->_opstate == OPSTATE::SEND) {
						delete ex_over;
						ex_over = nullptr;
					}
					continue;
				}
			}
			switch (ex_over->_opstate)
			{

			case OPSTATE::ACCEPT: {
				int client_id = get_new_client_id();
				if (client_id != -1)
				{
					{
						lock_guard<mutex>ll(_clients[client_id].Lock);
						_clients[client_id]._state = STATE::ALLOC;
					}
					cout << "WorkerThread Accept " << endl;
					// client 초기 정보 입력 ,,
					//ACCEPT를 다시열어줘야 하는가? ㄴㄴ accept쓰레드에서 관리하고 있기 때문에 안해줘도됌 
				}
			}
								break;
			case OPSTATE::RECV: {
				cout << " WorkerThread Recv " << endl;
				// 로직 쓰레드 분리, 

			}
							  break;
			case OPSTATE::SEND: {
				cout << " WorkertThread Send " << endl;
			}
							  break;
			}
		}
		
	}
	int get_new_client_id()
	{
		for (int i = 0; i < MAX_CLIENTS; ++i)
		{
			lock_guard<mutex> ll{ _clients[i].Lock };
			if (_clients[i]._state == STATE::FREE)
				return i;
		}
		return  -1;
	}

private:

	SOCKET _listensocket;
	SOCKET _clientsocket;
	HANDLE _iocpHandle;
	OverEx _overlapped;
	int _clientscnt = 0; // 접속한 클라이언트 수 

	thread _acceptThread;
	vector<thread> _workerThreads;
	array < Session, MAX_CLIENTS > _clients;

	bool _workerthreadRun = false;
	bool _acceptthreadRun = false;

};