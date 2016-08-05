#ifndef _MSG_HANDLER_H_
#define _MSG_HANDLER_H_
#include "../structtype.h"

class ConMsgNode; 
class MsgHandler
{
public:
	MsgHandler(){}
	~MsgHandler(){}

	bool handleMsg(const ConMsgNode & msgNode);
	
	static void registerMsgs();
	static void deregisterMsgs();
	static void registerTestMsgs();
	
private:
	
	//template<typename MsgType>
	static void registerMsgHandler(UInt32 msgId, void (*func)(const ConMsgNode & ));
	
	//template <typename MsgType >
	 static bool recvWrapper(const ConMsgNode & msgNode,  void  * func);

	class HandlerWraper
	{
	public:
		HandlerWraper(){}
		~HandlerWraper(){}
			
		void  * m_pFunc;
		bool (*m_pRecvWrapper)(const ConMsgNode & msgNode,   void  * func);

	};

	static HandlerWraper* m_hHandlers[MAXMESSAGEID + 1];
};






#endif