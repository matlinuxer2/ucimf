# quick and dirty makefile to install this terminfo file

TERMINFO_DIR=/usr/share/terminfo

# try to use TIC from the env
ifeq ($(TIC),)
	TIC=tic
endif

install:
	mkdir -p $(DESTDIR)$(TERMINFO_DIR) && \
	$(TIC) -o$(DESTDIR)$(TERMINFO_DIR) iterm.terminfo
