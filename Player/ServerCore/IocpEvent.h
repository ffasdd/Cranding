#pragma once

class Session; 

enum class EventType : uint8
{
	Connect,
	Accept,
	//PreRecv,
	Recv,
	Send
};

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void Init();
	EventType GetType() { return _type; }
private:
	EventType _type;

};
// OVERRLAPPED EX ¿ªÇÒ 

// Connect
class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) {}

};
// Accept
class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}

	void SetSession(Session* session) { _session = session; }
	Session* GetSession() { return _session; }
private :
	Session* _session = nullptr;
	// TODO 

};
// Send
class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) {}

private:
	// TODO 

};
// Recv
class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) {}

private:
	// TODO 

};