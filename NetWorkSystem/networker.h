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
		return * this;
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

	bool popMsgFromList(ConMsgNode & msgNode);

	UInt32 getCount(void) const;

private:
	UInt32 m_nCount;
	std::list<ConMsgNode > m_listConMsg;

	friend class NetWorker;
};

class NetWorker: public BaseThread
{
public:
	NetWorker(){ m_mutexLock = new Mutex();m_msgHandler.registerMsgs();
	}
	virtual ~ NetWorker(){
		if(m_mutexLock)
		{
			m_nShutDown = 1;
		}
	}
	virtual void threadWorkFunc();

	void pushNodeInStream(TcpHandler * tcpHandler);

	bool  popMsgFromInStream(MsgStream &msgstream);

private:
	MsgStream m_msgInStream;
    MsgStream m_msgOutStream;
	MsgHandler m_msgHandler;

	BaseLock * m_mutexLock;
	
};


#endif
