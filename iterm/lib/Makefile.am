SUBDIRS         = src . include

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(includedir)/iterm;
	$(mkinstalldirs) $(DESTDIR)$(includedir)/iterm/unix;
	@for f in include/iterm/*.h ; do \
		$(INSTALL_DATA) $$f $(DESTDIR)$(includedir)/iterm ; \
	done
	@for f in include/iterm/unix/*.h ; do \
		$(INSTALL_DATA) $$f $(DESTDIR)$(includedir)/iterm/unix ; \
	done
