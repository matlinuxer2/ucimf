bin_PROGRAMS = jfbterm
jfbterm_SOURCES = \
		main.c main.h \
		mytypes.h sequence.h \
		fbcommon.c fbcommon.h \
		fbdpsp.c fbdpsp.h \
		font.c font.h \
		picofont.c picofont.h \
		getcap.c getcap.h \
		message.c message.h \
		pcf.c pcf.h \
		pen.c pen.h \
		term.c term.h \
		util.c util.h \
		csv.c csv.h \
		vterm.c vtermlow.c vterm.h
jfbterm_LDADD = -lutil

JFBTERM_CONFIG_FILE = jfbterm.conf.sample

EXTRA_DIST = $(JFBTERM_CONFIG_FILE)

install-exec-hook:
	chown root:utmp $(DESTDIR)$(bindir)/jfbterm
	chmod ug+s $(DESTDIR)$(bindir)/jfbterm

install-data-local:
	$(INSTALL_DATA) $(srcdir)/$(JFBTERM_CONFIG_FILE) $(DESTDIR)$(sysconfdir)/$(JFBTERM_CONFIG_FILE)
	$(INSTALL_DATA) $(srcdir)/fonts/*.pcf.gz $(DESTDIR)$(datadir)/fonts/jfbterm

