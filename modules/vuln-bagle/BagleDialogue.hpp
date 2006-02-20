 /* $Id$ */

#include "DialogueFactory.hpp"
#include "Module.hpp"
#include "ModuleManager.hpp"
#include "SocketManager.hpp"
#include "Nepenthes.hpp"
#include "Dialogue.hpp"
#include "Socket.hpp"

using namespace std;

namespace nepenthes
{
	typedef enum
	{
		BAGLE_AUTH,
		BAGLE_REFERRER,
		BAGLE_BINARY
	} bagle_state;

	class Buffer;
	class Download;

	class BagleDialogue : public Dialogue
	{
	public:
		BagleDialogue(Socket *socket);
		~BagleDialogue();
		ConsumeLevel incomingData(Message *msg);
		ConsumeLevel outgoingData(Message *msg);
		ConsumeLevel handleTimeout(Message *msg);
		ConsumeLevel connectionLost(Message *msg);
		ConsumeLevel connectionShutdown(Message *msg);
	protected:
		bagle_state m_State;
		Buffer 		*m_Buffer;
		Download 	*m_Download;
		unsigned long 	m_FileSize;
	};
}
