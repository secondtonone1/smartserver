#ifndef _NETWORKER_H_
#define _NETWORKER_H_

#include "../structtype.h"
#include "../basethread.h"
#include "tcphandler.h"
#include "../CLock.h"
#include "msghandler.h"

class  ConMsgNode
{
public:
	ConMsgNode():mConnId(0){}
	~ConMsgNode(){mConnId = 0;}
	ConMsgNode(const ConMsgNode & conMsgNode)
	{
		mConnId = conMsgNode.mConnId;
		mMsgNode = conMsgNode.mMsgNode;
	}

	ConMsgNode & operator = (const ConMsgNode & conMsgNode)
	{
		mConnId = conMsgNode.mConnId;
		mMsgNode = conMsgNode.mMsgNode;
	}

	UInt64 mConnId;
	MsgNode mMsgNode;
};

class MsgStream
{
public:
	MsgStream():m_nCount(0){}
	~MsgStream(){
		m_nCount = 0;
		m_listConMsg.clear();
	}
	
	MsgStream( const MsgStream & msgStream)
	{
		m_nCount = msgStream.m_nCount;

		msgStream.m_listConMsg.begin();

		for(std::list<ConMsgNode>::const_iterator  msgIter = msgStream.m_listConMsg.begin(); 
			msgIter != msgStream.m_listConMsg.end();  msgIter++ )
		{
			m_listConMsg.push_back(*msgIter);
		}
		
	}

	MsgStream & operator = (const MsgStream & msgStream)
	{
		m_nCount = msgStream.m_nCount;
		m_listConMsg.clear();

		for(std::list<ConMsgNode>::const_iterator  msgIter = msgStream.m_listConMsg.begin(); 
			msgIter != msgStream.m_listConMsg.end();  msgIter++ )
		{
			m_listConMsg.push_back(*msgIter);
		}
	}

	void insertMsgToList(const ConMsgNode  &msgNode);

	ConMsgNode popMsgFromList();

	UInt32 getCount(void) const;

private:
	UInt32 m_nCount;
	std::list<ConMsgNode > m_listConMsg;

	friend class NetWorker;
};

class NetWorker: public BaseThread
{
public:
	NetWorker(){}
	virtual ~ NetWorker(){}
	virtual void threadWorkFunc();

	void pushNodeInStream(TcpHandler * tcpHandler);

	MsgStream  popMsgFromInStream();

private:
	MsgStream m_msgInStream;
    MsgStream m_msgOutStream;
	MsgHandler m_msgHandler;
	
};


#endif