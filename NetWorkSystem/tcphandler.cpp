
#include "stdafx.h"
#include "tcphandler.h"
//处理读事件
void TcpHandler::dealReadEvent()
{
	evbuffer * inputBuf = bufferevent_get_input(m_pBufferevent);
	size_t inputLen = evbuffer_get_length(inputBuf);

	while(inputLen > 0)
	{
		//tcphandler第一次接收消息或者该node接收完消息，需要开辟新的node接受消息
		if(!m_pLastNode || m_pLastNode->m_nMsgLen <= m_pLastNode->m_nOffSet)
		{
			//判断消息长度是否满足包头大小，不满足跳出
			if(inputLen  < PACKETHEADLEN)
			{
				break;
			}

			char data[PACKETHEADLEN]  = {0};
			bufferevent_read(m_pBufferevent, data, PACKETHEADLEN);
			struct PacketHead  packetHead;

			memcpy(&packetHead, data, PACKETHEADLEN);

			cout << "packetId is : " <<packetHead.packetID << endl;

			cout << "packetLen is :  " << packetHead.packetLen << endl;

			insertNode(packetHead.packetID, packetHead.packetLen);

			inputLen -= PACKETHEADLEN;
		}

		//考虑可能去包头后剩余的为0
		if(inputLen <= 0)
		{
			break;
		}
		//读取去除包头后剩余消息
		tcpRead(inputLen);
	}

}

void TcpHandler::tcpRead(UInt32 &inputLen)
{
	//node节点中的数据还有多少没读完
	UInt32 remainLen = m_pLastNode->m_nMsgLen - m_pLastNode->m_nOffSet;

	UInt32 readLen = bufferevent_read(m_pBufferevent, m_pLastNode->m_pMsg + m_pLastNode->m_nOffSet, remainLen);
	//统计bufferevent 的inputbuffer中剩余的长度
	inputLen -= readLen;
	//更改偏移标记
	m_pLastNode->m_nOffSet += readLen;
	//判断读完
	if(m_pLastNode->m_nOffSet >= m_pLastNode->m_nMsgLen)
	{
		m_pLastNode->m_pMsg[m_pLastNode->m_nMsgLen + 1] = '\0'; 
		cout << "receive msg is : " << m_pLastNode->m_pMsg << endl;
	}

}


bool TcpHandler::insertNode(UInt32 msgId, UInt32 msgLen)
{
	MsgNode * msgNode = new MsgNode(msgId, msgLen);
	if(!msgNode)
	{
		return false;
	}

	m_listMsgs.push_back(msgNode);

	m_pLastNode = msgNode;
	return true;
}