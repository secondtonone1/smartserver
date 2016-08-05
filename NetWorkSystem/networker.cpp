#include "networker.h"

void NetWorker::threadWorkFunc(void )
{
	while(1)
	{
		
		const MsgStream & msgStream = popMsgFromInStream();
		UInt32 count = msgStream.getCount();
		if(count == 0)
		{
			#if defined _WIN32
				cout << "msg count == 0!" <<endl;
				Sleep(2000);
				continue;
			#endif
		}
	
		//调用消息处理的而回调函数，并且在回调函数内部反序列化
		
	
		for(UInt32 i = 0; i < count; i++)
		{
			ConMsgNode & msgNode = m_msgInStream.popMsgFromList();
			m_msgHandler.handleMsg(msgNode);
		}
		
	}
}


void NetWorker::pushNodeInStream(TcpHandler * tcpHandler)
{
		//加锁处理消息加入到instream里
		Mutex mutexlock;
		CLock mylock(mutexlock);	
		list<MsgNode *> * msgList = tcpHandler->getListMsgs();
		UInt32 count = msgList->size();
		
		for(UInt32 i = 0; i < count; i++)
		{
			MsgNode * pMsgNode = msgList->front();
			bool res = pMsgNode->isReceived();
			if(res)
			{
				msgList->pop_front();
				ConMsgNode conMsgNode;
				conMsgNode.mConnId = tcpHandler->getConnId();
				conMsgNode.mMsgNode = *pMsgNode;
				delete(pMsgNode);
				m_msgInStream.insertMsgToList(conMsgNode);

			}
			else
			{
				break;
			}
			
		}

}


void MsgStream::insertMsgToList(const ConMsgNode  &msgNode)
{
	m_listConMsg.push_back(msgNode);
	m_nCount++;
}

ConMsgNode MsgStream::popMsgFromList()
{
	ConMsgNode msgNode =  m_listConMsg.front();
	m_listConMsg.pop_front();
	m_nCount--;
	return msgNode;
}

UInt32 MsgStream::getCount(void) const
{
	return m_nCount;
}

MsgStream  NetWorker::popMsgFromInStream()
{
	Mutex mutexLock;
	CLock mylock(mutexLock);
	MsgStream msgStream;
	UInt32 count = m_msgInStream.getCount();
	if(!count)
	{
		return msgStream;
	}
	
	for(UInt32 i = 0; i < count; i++)
	{
	    ConMsgNode & msgNode = m_msgInStream.popMsgFromList();
		msgStream.insertMsgToList(msgNode);
	}

	return msgStream;
}