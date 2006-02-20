# nepenthes tools Makefile
# Paul Baecher, Maximillian Dornseif, Markus Koetter
# $Id$

EXTRA_DIST = mwcollect-dump/mwcollect-dump.pl rpcxmlxfer/rpcxmlxfer-client.py rpcxmlxfer/rpcxmlxfer-server.py

SUBDIRS = bdiffm mkcarray

install-data-local:
	$(INSTALL_SCRIPT) mwcollect-dump/mwcollect-dump.pl $(DESTDIR)$(bindir)/
	$(INSTALL_SCRIPT) rpcxmlxfer/rpcxmlxfer-server.py $(DESTDIR)$(bindir)/rpcxmlxfer-server
	$(INSTALL_SCRIPT) rpcxmlxfer/rpcxmlxfer-client.py $(DESTDIR)$(bindir)/rpcxmlxfer-client
