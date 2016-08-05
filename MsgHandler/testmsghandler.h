#ifndef _TEST_MSG_HANDLER_H_
#define _TEST_MSG_HANDLER_H_
class ConMsgNode;

class TestMsgHandler
{
public:
	static void HandleTestMsg(const ConMsgNode & msgNode);
};

#endif