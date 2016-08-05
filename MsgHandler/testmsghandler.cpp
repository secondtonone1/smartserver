#include "testmsghandler.h"
#include "../ProtoBuf/smart.msg.pb.h"
#include "../NetWorkSystem/networker.h"

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
		

}