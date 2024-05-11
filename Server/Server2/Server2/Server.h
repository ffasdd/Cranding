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
	int get_new_room_id(unordered_map<int,Room>& rooms);
	void MoveNightNpc(int r_id,int npcid,float elapsedtime,float speed);


	void ReadyToStart(); // �κ� ���� ������ �Լ� 

public:
	SOCKET listensocket; 
	SOCKET clientsocket;

	Over_Exp _overlapped;


	thread lobbythread;
	thread timer_thread;
	vector<thread> worker_thread;



	concurrency::concurrent_queue<Session*> matchingqueue;
	
	unordered_map<int, Room> ingameroom;
	// array ���� clients �� �ʿ��Ѱ�? 
	// �ʿ����� �����Ű�����
	// 0 -  [ 0, 1 , 2 ] 
	// 1 - [ 3 , 4, 5 ]
	// room �ȿ� array �ξ Room ���� ��ü�� �����Ѵ�? 

	mutex r_l;

	int readycnt = 0;
	//bool lobbyrun = false;
};

 