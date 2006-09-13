#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <errno.h>

extern "C"
{
	#include <udns.h>
}

#include "Module.hpp"
#include "ModuleManager.hpp"
#include "Nepenthes.hpp"
#include "Socket.hpp"

#include "DNSHandler.hpp"
#include "EventHandler.hpp"
#include "Event.hpp"


#include "POLLSocket.hpp"

using namespace std;

namespace nepenthes
{
	class DNSResolverUDNS : public POLLSocket, public Module , DNSHandler
	{
	public:
		DNSResolverUDNS(Nepenthes *nepenthes);
		~DNSResolverUDNS();

		bool Init();
		bool Exit();

		/* POLLSocket */
		bool wantSend();
		int32_t doSend();
		int32_t doRecv();
		bool checkTimeout();
		bool handleTimeout();
		int32_t getSocket();

		/* DNSHandler */
		bool resolveDNS(DNSQuery *query);
		bool resolveTXT(DNSQuery *query);

		/* udns callback */
		static void dnscb(struct dns_ctx *ctx, void *result, void *data);
		static void dnscbA(struct dns_ctx *ctx, struct dns_rr_a4 *result, void *data);

	private:
		struct dns_ctx *m_DNSctx;
	};
}
