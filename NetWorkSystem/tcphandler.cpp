
#include "tcphandler.h"
#include <assert.h>
static int readtimes = 0;
//处理读事件
void TcpHandler::dealReadEvent(int & success)
{
	//cout << "m_pLastNode addr is:  " << m_pLastNode <<endl;
	if(m_pLastNode)
	{
		//cout << "m_pLastNode msglen is:  "<< m_pLastNode->m_nMsgLen <<endl;
		//cout << "m_pLastNode m_nOffSet is: " << m_pLastNode->m_nOffSet <<endl;
	}
	else
	{
		//cout << "m_pLastNode addr is null!!!" <<endl;
	}
	

	evbuffer * inputBuf = bufferevent_get_input(m_pBufferevent);
	size_t inputLen = evbuffer_get_length(inputBuf);
	//cout <<"total len is : "<<  inputLen <<endl;
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
			
			if(packetHead.packetID > 1024 || packetHead.packetID < 0)
			{
				success = 0;
				return;
			}

			insertNode(packetHead.packetID, packetHead.packetLen);

			inputLen -= PACKETHEADLEN;
			//cout << "after remove head the length is : " << inputLen <<endl;
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

void TcpHandler::dealWriteEvent(UInt32 msgID, std::string strSend)
{
	cout << "packet length is : " << strSend.size() <<endl;
	PacketHead packetHead;
	packetHead.packetID = msgID;
	packetHead.packetLen = strSend.size();

	char * sendData = (char *)malloc(sizeof(char) *(strSend.size() + PACKETHEADLEN) );
	memcpy(sendData, &packetHead, PACKETHEADLEN);
	memcpy(sendData + PACKETHEADLEN,  strSend.c_str(), strSend.size());
	int n = bufferevent_write(m_pBufferevent, sendData, strSend.size() + PACKETHEADLEN);
	//由于bufferevent的读写事件初始化都是永久的，用户可以根据自己的需求更改
	//在写事件将数据写到低水位后会将写事件移除，而enable会使事件重新加进来
	//读事件将数据从socket缓存区读取到inputbuffer，成功的情况下并不会将读事件移除
	if(n < 0)
	{
		cout << "write msg to bufferevent outputbuffer failed !!!" <<endl;
		return ;
	}

	bufferevent_enable(m_pBufferevent, EV_WRITE);

}


list<MsgNode *> * TcpHandler::getListMsgs(void)
{
	return &m_listMsgs;
}

const UInt64 &TcpHandler::getConnId(void)
{
	return m_nConnId;
}

void TcpHandler::tcpRead(UInt32 &inputLen)
{
	//node节点中的数据还有多少没读完
	assert(m_pLastNode->m_nMsgLen >= m_pLastNode->m_nOffSet);
	UInt32 remainLen = m_pLastNode->m_nMsgLen - m_pLastNode->m_nOffSet;
	//cout << "remainLen is: " <<remainLen << endl;
	
	UInt32 readLen = bufferevent_read(m_pBufferevent, m_pLastNode->m_pMsg + m_pLastNode->m_nOffSet, remainLen);
	//cout << "tcp read len is : " << readLen <<endl;

	//统计bufferevent 的inputbuffer中剩余的长度
	inputLen -= readLen;
	//更改偏移标记
	m_pLastNode->m_nOffSet += readLen;
	//cout << "after read  offset is :  " <<m_pLastNode->m_nOffSet <<endl;

	//判断读完
	if(m_pLastNode->m_nOffSet >= m_pLastNode->m_nMsgLen)
	{
		m_pLastNode->m_pMsg[m_pLastNode->m_nMsgLen ] = '\0'; 
		//cout << "receive msg is : " << m_pLastNode->m_pMsg << endl;
		//cout <<"read times is :  " << ++readtimes<< endl;

		m_pLastNode = NULL;
		
	}

}

void TcpHandler::tcpSend(UInt32 msgId, UInt32 msgLen, char * msg)
{

}

bool MsgNode::isReceived()
{
	if(m_nOffSet >= m_nMsgLen)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool TcpHandler::insertNode(UInt32 msgId, UInt32 msgLen)
{
	readtimes++;
	//cout << "insert node : "<< readtimes << endl;
	//cout << "msgId is: " << msgId << endl;
	//cout << "msgLen is: " <<msgLen <<endl;

	MsgNode * msgNode = new MsgNode(msgId, msgLen);
	if(!msgNode)
	{
		assert(false);
		return false;
	}

	m_listMsgs.push_back(msgNode);

	m_pLastNode = msgNode;
	//cout <<"new m_pLastNode addr is:  " <<m_pLastNode << endl;
	

	return true;
}