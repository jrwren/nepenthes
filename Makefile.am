# nepenthes meta-Makefile
# Maximillian Dornseif
# $Id$

AUTOMAKE_OPTIONS = foreign

SUBDIRS = nepenthes-core modules 
#tools

EXTRA_DIST = configure.ac CHANGES doc/README doc/README.VFS doc/logo-shaded.svg doc/nepenthes.8 conf/nepenthes.conf.dist

docdir = $(prefix)/share/doc
doc_DATA = doc/README doc/README.VFS doc/logo-shaded.svg

#sysconf_DATA = conf/nepenthes.conf.dist

AM_CXXFLAGS = ''

#ifndef NDEBUG
#AM_CXXFLAGS += -g
#endifndef

#ifdef PROFILE
#AM_CXXFLAGS += -pg
#endifdef

# $(INSTALL_DATA) $(srcdir)/conf/nepenthes.conf.dist $(DESTDIR)$(sysconfdir)/nepenthes/nepenthes.conf.dist

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(sysconfdir)
	$(mkinstalldirs) $(DESTDIR)$(sysconfdir)/nepenthes
	$(INSTALL_DATA) $(srcdir)/conf/nepenthes.conf.dist $(DESTDIR)$(sysconfdir)/nepenthes/nepenthes.conf.dist; \
	if [ ! -e $(DESTDIR)$(sysconfdir)/nepenthes/nepenthes.conf ]; then \
		$(INSTALL_DATA) $(srcdir)/conf/nepenthes.conf.dist $(DESTDIR)$(sysconfdir)/nepenthes/nepenthes.conf; \
	fi	
#	for src in `find modules -name '*.conf.dist'`; do \
#		$(INSTALL_DATA) "$$src" $(DESTDIR)$(sysconfdir)/nepenthes ; \
#	done
	for src in `find modules -name '*.conf.dist'`; do \
		dst="$(DESTDIR)$(sysconfdir)/nepenthes/`basename "$$src" | perl -npe 's/\.dist$$//'`" ; \
		if [ ! -e "$$dst" ] ; then \
		 	$(INSTALL_DATA) "$$src" "$$dst" ; \
		fi ; \
	done
	
	$(mkinstalldirs) $(DESTDIR)$(mandir)
	$(mkinstalldirs) $(DESTDIR)$(mandir)/man8
	$(INSTALL_DATA) $(srcdir)/doc/nepenthes.8 $(DESTDIR)$(mandir)/man8/nepenthes.8;
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)/cache
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)/cache/nepenthes
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)/cache/nepenthes/geolocation
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)/log
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)/binaries
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)/hexdumps

