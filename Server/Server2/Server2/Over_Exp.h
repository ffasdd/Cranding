#pragma once
enum class MonsterType: char;
enum class COMP_TYPE : int { Accept , Recv , Send,
	NPC_MOVE, NPC_ATTACK, NPC_DIE, FIND_PLAYER, NPC_WAKE_UP ,NPC_UPDATE ,NPC_DEAD, NPC_INITIALIZE,
	DAYTIME_TIMER, NIGHT_TIMER, ICE_NPC_UPDATE,FIRE_NPC_UPDATE,NATURE_NPC_UPDATE, NPC_TRACE,
	ICE_BOSS_MOVE,FIRE_BOSS_MOVE,NATURE_BOSS_MOVE,PLAYER_ATTACKED_NPC
};
class Over_Exp
{
public:
	Over_Exp();
	Over_Exp(char* packet);
public:
	WSAOVERLAPPED _over;
	WSABUF _wsaBuf;
	char _sendbuf[200];
	COMP_TYPE _comptype;
	int _ai_target_obj;
	int room_id;
	MonsterType _monstertype;
};

