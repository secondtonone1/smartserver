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
			 memset(m_pMsg, 0, m_nMsgLen+1);
		}

		MsgNode(const MsgNode & msgNode)
		{
			m_pMsg=(char *)malloc(sizeof(char) *(msgNode.m_nMsgLen +1));
			if(!m_pMsg)
			{
				cout << "malloc error !!!" <<endl;
				return;
			}
			m_nOffSet = msgNode.m_nOffSet;
			m_nMsgLen = msgNode.m_nMsgLen;
			m_nMsgId = msgNode.m_nMsgId;
			memcpy(m_pMsg, msgNode.m_pMsg, msgNode.m_nMsgLen);
			m_pMsg[msgNode.m_nMsgLen] = '\0';

		}

		MsgNode & operator = (const MsgNode & msgNode)
		{
			if(m_pMsg)
			{
				delete m_pMsg;
				m_pMsg = NULL;
			}

			m_pMsg = (char *)malloc(sizeof(char) *(msgNode.m_nMsgLen +1));
			
			if(!m_pMsg)
			{
				cout << "malloc error !!!" <<endl;
				return *this;
			}

			m_nOffSet = msgNode.m_nOffSet;
			m_nMsgLen = msgNode.m_nMsgLen;
			m_nMsgId = msgNode.m_nMsgId;
			memcpy(m_pMsg, msgNode.m_pMsg, msgNode.m_nMsgLen);
			m_pMsg[msgNode.m_nMsgLen] = '\0';

			
			return * this;
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

		bool isReceived();
		UInt32 getMsgId()const{return m_nMsgId;}
		std::string getMsgData() const{return std::string(m_pMsg) ;}
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

	const UInt64 &getConnId(void);

	void dealReadEvent();

	void dealWriteEvent(UInt32 msgID, std::string strSend);

	list<MsgNode *> * getListMsgs(void);

private:
	//考虑到禁止copy构造函数和=运算符
	//保证一个bufferevent只有一个TcpHandler管理
	TcpHandler & operator = (const TcpHandler & tcpHandler)
	{
		m_pBufferevent = tcpHandler.m_pBufferevent;
		m_nConnId = tcpHandler.m_nConnId;
		m_pLastNode = tcpHandler.m_pLastNode;
		m_listMsgs = tcpHandler.m_listMsgs;

		return *this;
	}

	TcpHandler(const TcpHandler & tcpHandler)
	{
		m_pBufferevent = tcpHandler.m_pBufferevent;
		m_nConnId = tcpHandler.m_nConnId;
		m_pLastNode = tcpHandler.m_pLastNode;
		m_listMsgs = tcpHandler.m_listMsgs;
	}

private:
	void tcpRead(UInt32 &inputLen);
	void tcpSend(UInt32 msgId, UInt32 msgLen, char * msg);
	
	bool insertNode(UInt32 msgId, UInt32 msgLen);

	UInt64 m_nConnId;
	bufferevent* m_pBufferevent;
	list<MsgNode *> m_listMsgs;
	MsgNode * m_pLastNode;
	

};


#endif