#pragma once
#include"Over_Exp.h"
#include"Session.h"
#include"Room.h"
#include"Monster.h"
#include"Timer.h"
#include <queue>

extern HANDLE _IocpHandle;

class Server
{
public:
	static Server& GetInstance();

	Server();
	Server(const Server& server) = delete;
	Server& operator=(const Server& server) = delete;

	~Server();

	void Run();
	void Stop();

	void NetworkSet();
	void Iocp();
	void WorkerThread();

	void InitialziedMonster(int room_Id);

	void ProcessPacket(int id, char* packet);

	void disconnect(int id);

	bool can_see(int to, int from);
	int get_new_client_id();
	int get_new_room_id(unordered_map<int, Room>& rooms);



	void ReadyToStart(); // 로비 관찰 스레드 함수 

public:
	//Socket Utils 
	SOCKET CreateSocket();
	bool SetLinger(SOCKET socket, UINT16 onoff, UINT16 linger);
	bool SetReuseAddress(SOCKET socket, bool flag);
	bool SetTcpNoDelay(SOCKET socket);

public:
	// For Db

	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;

	SQLCHAR user_id;
	SQLCHAR password;
	SQLLEN cbuser_id = 0; SQLLEN cbuser_password = 0;


	void ShowError(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
	{
		SQLSMALLINT iRec = 0;
		SQLINTEGER iError;
		WCHAR wszMessage[1000];
		WCHAR wszState[SQL_SQLSTATE_SIZE + 1];
		if (RetCode == SQL_INVALID_HANDLE) {
			fwprintf(stderr, L"Invalid handle!\n");
			return;
		}
		while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
			(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS) {
			// Hide data truncated..
			if (wcsncmp(wszState, L"01004", 5)) {
				fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
			}
		}
	}

	void initialize_DB()
	{
		setlocale(LC_ALL, "korean");

		retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);
		retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
		SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

		retcode = SQLConnect(hdbc, (SQLWCHAR*)L"2018180024_SDY", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

		retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

		std::cout << "DB Access OK\n";
	}

	bool isAllowAccess(int cid, int _password)
	{
		retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

		wstring _db_id = to_wstring(cid);
		wstring _db_password = to_wstring(_password);
		

		wstring storedProcedure = L"EXEC SelectId " + _db_id + L", " + _db_password;


		retcode = SQLExecDirect(hstmt, (SQLWCHAR*)storedProcedure.c_str(), SQL_NTS);
		ShowError(hstmt, SQL_HANDLE_STMT, retcode);
		//  저장 
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {


			retcode = SQLBindCol(hstmt, 1, SQL_C_LONG, &user_id, 10, &cbuser_id);
			retcode = SQLBindCol(hstmt, 2, SQL_C_LONG, &password, 10, &cbuser_password);

			for (int i = 0; ; i++) {
				retcode = SQLFetch(hstmt);
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				{

					//DB 에 있으면 ,, 
					cout << " 로그인 성공 " << endl;
					return true;


				}
				else
				{
					// 없으면 추가해라 
					std::cout << "DB에 정보를 추가하였습니다. ";
					return false;
				}
			}
		}

	}

public:
	SOCKET listensocket;
	SOCKET clientsocket;

	Over_Exp _overlapped;


	thread lobbythread;
	thread timer_thread;
	vector<thread> worker_thread;


	concurrency::concurrent_queue<Session*> matchingqueue;

	unordered_map<int, Room> ingameroom;
	// array 전역 clients 가 필요한가? 
	// 필요하지 않을거같은데
	// 0 -  [ 0, 1 , 2 ] 
	// 1 - [ 3 , 4, 5 ]
	// room 안에 array 두어서 Room 으로 전체를 관리한다? 

	mutex r_l;

	int readycnt = 0;
	//bool lobbyrun = false;
};

