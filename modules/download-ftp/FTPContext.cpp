#include "FTPContext.hpp"
#include "CTRLDialogue.hpp"
#include "FILEDialogue.hpp"

#include "Download.hpp"
#include "LogManager.hpp"
#include "Nepenthes.hpp"

using namespace nepenthes;


FTPContext::FTPContext(Download *down)
{
	m_Download = down;

	m_ActiveFtpBindPort = 0;
}

FTPContext::FTPContext(Download *down, CTRLDialogue *dia)
{
	m_Download = down;
	m_CDialogue = dia;

	m_ActiveFtpBindPort = 0;
}

FTPContext::~FTPContext()
{
	logPF();
//	delete m_Download;
}

void FTPContext::setActiveFTPBindPort(uint16_t port)
{
	m_ActiveFtpBindPort = port;
}

uint16_t FTPContext::getActiveFTPBindPort()
{
	return m_ActiveFtpBindPort;
}

Download *FTPContext::getDownload()
{
	return m_Download;
}

CTRLDialogue *FTPContext::getCTRLDialogue()
{
	return m_CDialogue;
}
