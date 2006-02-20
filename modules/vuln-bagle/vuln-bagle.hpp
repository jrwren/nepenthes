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

	class BagleVuln : public Module , public DialogueFactory
	{
	public:
		BagleVuln(Nepenthes *);
		~BagleVuln();
		Dialogue *createDialogue(Socket *socket);
		bool Init();
		bool Exit();
	};


}
extern nepenthes::Nepenthes *g_Nepenthes;
