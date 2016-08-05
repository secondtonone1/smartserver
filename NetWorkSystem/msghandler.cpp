#include "msghandler.h"
#include "networker.h"
#include "../ProtoBuf/smart.msg.pb.h"
#include "../MsgHandler/testmsghandler.h"
#include "../Msg/msgid.h"

MsgHandler::HandlerWraper * MsgHandler::m_hHandlers[MAXMESSAGEID + 1];

bool MsgHandler:: handleMsg(const ConMsgNode & msgNode)
{
		UInt32 msgId = msgNode.mMsgNode.getMsgId();
		if(msgId > MAXMESSAGEID || msgId < 0)
		{
			return false;
		}
		m_hHandlers[msgId]->m_pRecvWrapper(msgNode, m_hHandlers[msgId]->m_pFunc);
		return true;
}

void MsgHandler::registerMsgs()
{
	for(UInt32 i = 0; i < MAXMESSAGEID; i ++)
	{
		m_hHandlers[i] = NULL;
	}
	
	registerTestMsgs();
}

void MsgHandler::registerTestMsgs()
{
	 registerMsgHandler(TESTMSGSID,&TestMsgHandler::HandleTestMsg);
	
}

void MsgHandler::deregisterMsgs()
{
	for(UInt32 i = 0; i < MAXMESSAGEID; i ++)
	{
		if( m_hHandlers[i])
		{
			delete m_hHandlers[i];
			m_hHandlers[i] = NULL;
		}
	}
}

void MsgHandler::registerMsgHandler(UInt32 msgId, void (*func)(const ConMsgNode & ))
{
	HandlerWraper * msgHandler = new HandlerWraper();
	msgHandler->m_pFunc = (void *)func;
	//将函数赋值给函数指针
	msgHandler->m_pRecvWrapper = &recvWrapper;
	
	assert(msgId <= MAXMESSAGEID);
	assert(msgId >= 0);
	m_hHandlers[msgId] = msgHandler;
}

bool MsgHandler::recvWrapper(const ConMsgNode & msgNode,  void  * func)
{
	//调用函数处理问题
	UInt32 msgId = msgNode.mMsgNode.getMsgId();
	( (void(*)(const ConMsgNode & ) )func )(msgNode);
	
	return true;
}

