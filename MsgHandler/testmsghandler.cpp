#include "testmsghandler.h"
#include "../ProtoBuf/smart.msg.pb.h"
#include "../NetWorkSystem/networker.h"
#include "../NetWorkSystem/networksystem.h"
void TestMsgHandler::HandleTestMsg(const ConMsgNode & msgNode)
{
		UInt32 msgId = msgNode.mMsgNode.getMsgId();
		smart::test m_smartTest;
		std::string buf = msgNode.mMsgNode.getMsgData();

		bool res = m_smartTest.ParseFromString(buf);
		if(!res)
		{
			cout <<msgId << " msg parse error!" <<endl;
			return;
		}

		cout << m_smartTest.age() <<endl;
		cout << m_smartTest.name() << endl;
		cout << m_smartTest.email() << endl;

		//²âÊÔ»Ø°ü
	/*	TcpHandler * tcpHandler = NetWorkSystem::getSingleton().getHandlerByConnId(msgNode.mConnId);
		if(tcpHandler)
		{
				std::string strTest;
				m_smartTest.SerializeToString(&strTest);
				tcpHandler->dealWriteEvent(strTest);
		}*/
		
	
		
}