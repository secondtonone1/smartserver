#include "networker.h"

void NetWorker::threadWorkFunc(void )
{
	while(1)
	{
		//cout <<"current worker pointer *"<<  this <<endl;
		MsgStream & msgStream = popMsgFromInStream();
		UInt32 count = msgStream.getCount();
		if(count == 0)
		{
			#if defined _WIN32
		//		cout << "msg count == 0!" <<endl;
				Sleep(2000);
				continue;
			#endif
		}
	
		//调用消息处理的而回调函数，并且在回调函数内部反序列化
		
	
		for(UInt32 i = 0; i < count; i++)
		{
			 ConMsgNode  &msgNode = msgStream.popMsgFromList();
			m_msgHandler.handleMsg(msgNode);
		}
		
	}
}


void NetWorker::pushNodeInStream(TcpHandler * tcpHandler)
{
		//加锁处理消息加入到instream里
		CLock mylock(m_mutexLock);	

		//cout << this << "push lock!!!" <<endl;
		list<MsgNode *> * msgList = tcpHandler->getListMsgs();
		UInt32 count = msgList->size();
		
		for(UInt32 i = 0; i < count; i++)
		{
			MsgNode * pMsgNode = msgList->front();
		
			bool res = pMsgNode->isReceived();
			if(res)
			{
				
				ConMsgNode conMsgNode;
				conMsgNode.mConnId = tcpHandler->getConnId();
				conMsgNode.mMsgNode = *pMsgNode;
				delete(pMsgNode);
				pMsgNode = NULL;
				msgList->pop_front();
			
				m_msgInStream.insertMsgToList(conMsgNode);

			}
			else
			{
				break;
			}
			
		}
//	cout <<this << "push unlock!!!" <<endl;
}


void MsgStream::insertMsgToList(const ConMsgNode  &msgNode)
{
	m_listConMsg.push_back(msgNode);
	m_nCount++;
}

ConMsgNode MsgStream::popMsgFromList()
{
	
	if(!m_nCount)
	{
		ConMsgNode msgNode;
		return msgNode ;
	}

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
	CLock mylock(m_mutexLock);
//	cout << this << "pop lock!!" << endl;
	MsgStream msgStream;
	UInt32 count = m_msgInStream.getCount();
	if(!count)
	{
	//	cout << this << "pop unlock !!" <<endl;
		return msgStream;
	}
	
	for(UInt32 i = 0; i < count; i++)
	{
	    ConMsgNode & msgNode = m_msgInStream.popMsgFromList();
		msgStream.insertMsgToList(msgNode);
	}

	//cout <<this <<  "pop unlock!!" <<endl;
	return msgStream;
}