#include "networker.h"

void NetWorker::threadWorkFunc(void )
{
	while(1)
	{
		if(m_nShutDown)
		{
			break;
		}
	
		
		MsgStream  msgStream;
		bool flag = popMsgFromInStream(msgStream);
		UInt32 count = msgStream.getCount();
		if(count == 0 || !flag )
		{
			#if defined _WIN32
	
				Sleep(2000);
				continue;
			#endif

			#if defined __linux__
				cout << "net worker msg node null"<<endl;
				sleep(2);
				continue;
			#endif
		}
	
		//调用消息处理的而回调函数，并且在回调函数内部反序列化
		
	
		for(UInt32 i = 0; i < count; i++)
		{
			ConMsgNode  msgNode ;
			 bool flag = msgStream.popMsgFromList(msgNode);
			 if(flag)
			 {
				m_msgHandler.handleMsg(msgNode);
			 }
			
		}
		cout << "success net worker threadfunc!!!"<<endl;
		
	}

	//销毁锁
	if(m_mutexLock)
	{
		delete(m_mutexLock);
	}
	m_mutexLock = NULL;	
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

bool MsgStream::popMsgFromList(ConMsgNode & msgNode)
{
	
	if(!m_nCount)
	{
		return false;
	}

	msgNode =  m_listConMsg.front();
	m_listConMsg.pop_front();
	m_nCount--;
	return true;
}

UInt32 MsgStream::getCount(void) const
{
	return m_nCount;
}

bool  NetWorker::popMsgFromInStream(MsgStream &msgStream)
{
	if(m_nShutDown)
	{
		return false;
	}
	CLock mylock(m_mutexLock);
//	cout << this << "pop lock!!" << endl;
	
	UInt32 count = m_msgInStream.getCount();
	if(!count)
	{
	//	cout << this << "pop unlock !!" <<endl;
		return false;
	}
	
	for(UInt32 i = 0; i < count; i++)
	{
	    ConMsgNode  msgNode;
		bool flag = m_msgInStream.popMsgFromList(msgNode);
		if(flag)
		{
				msgStream.insertMsgToList(msgNode);
		}
	
	}

	//cout <<this <<  "pop unlock!!" <<endl;
	return true;
}
