#ifndef _TCPHANDLER_H_
#define _TCPHANDLER_H_
#include "../structtype.h"
#include <list>
using namespace std;
//消息队列节点
class MsgNode
{
	friend class TcpHandler;
	public:
		MsgNode():m_nMsgLen(0), m_nMsgId(0), m_nOffSet(0), m_pMsg(NULL){}
		MsgNode(UInt32 msgId, UInt32 msgLen)
		{
			 m_pMsg = (char *)malloc(sizeof(char) * (msgLen + 1));
			 if(!m_pMsg)
			 {
				cout << "malloc error !!!" <<endl;
				return; 
			 }
		
			 m_nOffSet = 0;
			 m_nMsgLen = msgLen;
			 m_nMsgId = msgId;
		}

		~MsgNode(){
			if(m_pMsg)
			{
				free (m_pMsg);
				m_pMsg = NULL;

				m_nMsgLen = 0;
				m_nMsgId = 0;
				m_nOffSet = 0;
			}
		}

		
	private:
		//消息长度
		UInt32 m_nMsgLen;
		//消息id
		UInt32 m_nMsgId;
		//消息偏移位置
		UInt32 m_nOffSet;
		//消息内容
		char * m_pMsg;


};

class TcpHandler
{
public:
	TcpHandler():m_nConnId(0), m_pBufferevent(NULL),m_pLastNode(NULL){}
	
	TcpHandler(UInt64 connId, bufferevent * pBufferevent)
	{
			m_nConnId = connId;
			m_pBufferevent = pBufferevent;
			m_pLastNode = NULL;
	}

	~TcpHandler()
	{
		m_nConnId = 0;
		if(!m_pBufferevent)
		{
			bufferevent_free(m_pBufferevent);			
		}
		m_pBufferevent = NULL;
		
		for(list<MsgNode *>::iterator msgIter = m_listMsgs.begin(); msgIter != m_listMsgs.end();
			msgIter ++)
		{
			if(*msgIter)
			{
					delete(*msgIter);
			}
		
			*msgIter = NULL;
		}

		m_listMsgs.clear();

	}

	void setConnId(UInt64 connId)
	{
		m_nConnId = connId;
	}

	void setBufferevent(bufferevent * pBufferevent)
	{
		m_pBufferevent = pBufferevent;
	}


	void dealReadEvent();

private:
	void tcpRead(UInt32 &inputLen);
	
	bool insertNode(UInt32 msgId, UInt32 msgLen);
	UInt64 m_nConnId;
	bufferevent* m_pBufferevent;
	list<MsgNode *> m_listMsgs;
	MsgNode * m_pLastNode;
	

};


#endif