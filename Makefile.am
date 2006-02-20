# nepenthes meta-Makefile
# Maximillian Dornseif
# $Id$

AUTOMAKE_OPTIONS = foreign

SUBDIRS = nepenthes-core modules tools

EXTRA_DIST = CHANGES doc/README doc/README.VFS doc/logo-shaded.svg conf/nepenthes.conf.dist

docdir = $(prefix)/share/doc
doc_DATA = doc/README doc/README.VFS doc/logo-shaded.svg

sysconf_DATA = conf/nepenthes.conf.dist

AM_CXXFLAGS = ''

#ifndef NDEBUG
#AM_CXXFLAGS += -g
#endifndef

#ifdef PROFILE
#AM_CXXFLAGS += -pg
#endifdef

# $(INSTALL_DATA) $(srcdir)/conf/nepenthes.conf.dist $(DESTDIR)$(sysconfdir)/nepenthes.conf.dist

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(sysconfdir)
	if [ ! -e $(DESTDIR)$(sysconfdir)/nepenthes.conf ]; then \
		$(INSTALL_DATA) $(srcdir)/conf/nepenthes.conf.dist $(DESTDIR)$(sysconfdir)/nepenthes.conf; \
	fi	
	for src in `find modules -name '*.conf.dist'`; do \
		$(INSTALL_DATA) "$$src" $(DESTDIR)$(sysconfdir) ; \
	done
	for src in `find modules -name '*.conf.dist'`; do \
		dst="$(DESTDIR)$(sysconfdir)/`basename "$$src" | perl -npe 's/\.dist$$//'`" ; \
		if [ ! -e "$$dst" ] ; then \
		 	$(INSTALL_DATA) "$$src" "$$dst" ; \
		fi ; \
	done
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)/log
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)/binaries
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)/hexdumps

