#ifndef MMATCHCHANNEL_H
#define MMATCHCHANNEL_H

#include <map>
#include <list>
using namespace std;

#include "MMatchGlobal.h"
#include "MUID.h"
#include "MMatchChannel.h"
#include "MPageArray.h"
#include "MSmartRefresh.h"
#include "MMatchChannelRule.h"

#define CHANNELNAME_LEN		64
#define CHANNELRULE_LEN		64
#define DEFAULT_CHANNEL_MAXPLAYERS			200
#define DEFAULT_CHANNEL_MAXSTAGES			100
#define MAX_CHANNEL_MAXSTAGES				500
#define NUM_PLAYERLIST_NODE					6
#define CHANNEL_NO_LEVEL					(-1)

class MMatchObject;
class MMatchStage;
class MCommand;


enum MCHANNEL_TYPE {
	MCHANNEL_TYPE_PRESET	= 0,		// �Ϲ�ä��
	MCHANNEL_TYPE_USER		= 1,		// �缳ä��
	MCHANNEL_TYPE_PRIVATE	= 2,		// ���ä�� - ���� �Ⱦ���
	MCHANNEL_TYPE_CLAN		= 3,		// Ŭ��ä��
	MCHANNEL_TYPE_MAX
};


// ä�� ����Ʈ �޶�� ��û�Ҷ� ������ ����ü
struct MCHANNELLISTNODE {
	MUID			uidChannel;						// ä�� UID
	short			nNo;							// ä�ι�ȣ
	unsigned char	nPlayers;						// �����ο�
	short			nMaxPlayers;					// �ִ��ο�
	short			nLevelMin;						// �ּҷ���
	short			nLevelMax;						// �ִ뷹��
	char			nChannelType;					// ä��Ÿ��
	char			szChannelName[CHANNELNAME_LEN];	// ä���̸�
};

typedef map<string, MMatchObject*>			MObjectStrMap;
typedef map<int, MMatchStage*>				MChannelStageMap;
typedef MPageArray<MMatchObject*>			MChannelUserArray;


class MMatchChannel {
private:
	MUID			m_uidChannel;
	char			m_szChannelName[CHANNELNAME_LEN];
	MCHANNEL_TYPE	m_nChannelType;
	int				m_nMaxPlayers;
	int				m_nLevelMin;
	int				m_nLevelMax;
	int				m_nMaxStages;
	char			m_szRuleName[CHANNELRULE_LEN];
	MCHANNEL_RULE	m_nRuleType;
	//bool			m_bNewbieChannel;		// ����ä���� ���� �ʺ��� �� �� �ִ�.

	MMatchObjectMap	m_ObjUIDCaches;			// ä����ü �÷��̾��
	MMatchObjectMap	m_ObjUIDLobbyCaches;	// �κ� �ִ� �÷��̾��
//	MObjectStrMap	m_ObjStrCaches;

	MMatchStage*	m_pStages[MAX_CHANNEL_MAXSTAGES];
	list<int>		m_UnusedStageIndexList;

	MChannelUserArray			m_UserArray;
	MSmartRefresh				m_SmartRefresh;

	unsigned long	m_nChecksum;
	u64				m_nLastChecksumTick;

	u64				m_nLastTick;
	unsigned long	m_nEmptyPeriod;

	void JoinLobby(const MUID& uid, MMatchObject* pObj);
	void LeaveLobby(const MUID& uid);
protected:
	bool IsChecksumUpdateTime(u64 nTick);
	void UpdateChecksum(u64 nTick);
	unsigned long GetEmptyPeriod()	{ return m_nEmptyPeriod; }

public:
	bool CheckTick(u64 nClock);
	void Tick(u64 nClock);

	unsigned long GetChecksum()		{ return m_nChecksum; }
	bool CheckLifePeriod();

public:
	bool Create(const MUID& uid, const char* pszName, const char* pszRuleName, 
				MCHANNEL_TYPE nType=MCHANNEL_TYPE_PRESET, int nMaxPlayers=DEFAULT_CHANNEL_MAXPLAYERS, 
				int nLevelMin=CHANNEL_NO_LEVEL, int nLevelMax=CHANNEL_NO_LEVEL);
	void Destroy();

	const char* GetName()			{ return m_szChannelName; }
	const char* GetRuleName()		{ return m_szRuleName; }
	MUID GetUID()					{ return m_uidChannel; }
	MCHANNEL_TYPE GetChannelType()	{ return m_nChannelType; }
	MCHANNEL_RULE GetRuleType()		{ return m_nRuleType; }

	int GetMaxPlayers()				{ return m_nMaxPlayers; }
	int GetLevelMin()				{ return m_nLevelMin; }
	int GetLevelMax()				{ return m_nLevelMax; }
	int	GetMaxStages()				{ return m_nMaxStages; }
	size_t GetObjCount()			{ return m_ObjUIDCaches.size(); }
	int GetPlayers();
	auto GetObjBegin()		{ return m_ObjUIDCaches.begin(); }
	auto GetObjEnd()		{ return m_ObjUIDCaches.end(); }
	auto GetLobbyObjBegin()	{ return m_ObjUIDLobbyCaches.begin(); }
	auto GetLobbyObjEnd()	{ return m_ObjUIDLobbyCaches.end(); }


	void AddObject(const MUID& uid, MMatchObject* pObj);
	void RemoveObject(const MUID& uid);
public:
	bool AddStage(MMatchStage* pStage);
	void RemoveStage(MMatchStage* pStage);
	bool IsEmptyStage(int nIndex);
	MMatchStage* GetStage(int nIndex);
	int GetPrevStageCount(int nStageIndex);	// nStageIndex�� �������� �ʴ� nStageIndex������ ������� �� ���� 
	int GetNextStageCount(int nStageIndex);	// nStageIndex�� �������� �ʴ� nStageIndex������ ������� �� ���� 
	
	//bool IsNewbieChannel()			{ return m_bNewbieChannel; }

public:
	MChannelUserArray* GetUserArray()	{ return &m_UserArray; }

public:
	void SyncPlayerList(MMatchObject* pObj, int nPage);
};


class MMatchChannelMap : public map<MUID, MMatchChannel*> {
private:
	MUID						m_uidGenerate;
	unsigned long				m_nChecksum;
	map<MUID, MMatchChannel*>	m_TypesChannelMap[MCHANNEL_TYPE_MAX];
	void Insert(const MUID& uid, MMatchChannel* pChannel)	{	insert(value_type(uid, pChannel));	}
	MUID UseUID()				{	m_uidGenerate.Increase();	return m_uidGenerate;	}
//	void UpdateChecksum(unsigned long nClock);
public:
	MMatchChannelMap()			{	m_uidGenerate = MUID(0,0);	m_nChecksum=0; }
	virtual ~MMatchChannelMap()	{	}
	void Destroy();
	
	MMatchChannel* Find(const MUID& uidChannel);
	MMatchChannel* Find(const MCHANNEL_TYPE nChannelType, const char* pszChannelName);

	bool Add(const char* pszChannelName, const char* pszRuleName, MUID* pAllocUID, MCHANNEL_TYPE nType=MCHANNEL_TYPE_PRESET, int nMaxPlayers=DEFAULT_CHANNEL_MAXPLAYERS, int nLevelMin=-1, int nLevelMax=-1);
	bool Remove(const MUID& uidChannel, MMatchChannelMap::iterator* pNextItor);
	void Update(u64 nClock);

	unsigned long GetChannelListChecksum() { return m_nChecksum; }
	int GetChannelCount(MCHANNEL_TYPE nChannelType);

	map<MUID, MMatchChannel*>::iterator GetTypesChannelMapBegin(MCHANNEL_TYPE nType);
	map<MUID, MMatchChannel*>::iterator GetTypesChannelMapEnd(MCHANNEL_TYPE nType);

};


#endif