loopiditydir = $(pkgdatadir)
doc_DATA     = COPYING            \
               LICENSES           \
               data/COPYING.fonts \
               data/LICENSES.data
man1_MANS    = doc/loopidity.1
EXTRA_DIST   = $(man_MANS)

SUBDIRS = src


installcheck-local:
	echo "installcheck-local"
	stat --printf='' $(bindir)/loopidity
	stat --printf='' $(loopiditydir)/
	stat --printf='' $(docdir)/COPYING
	stat --printf='' $(docdir)/LICENSES
	stat --printf='' $(docdir)/COPYING.fonts
	stat --printf='' $(docdir)/LICENSES.data
	stat --printf='' $(man1dir)/loopidity.1


uninstall-hook:
	-rmdir ${loopiditydir}
	-rmdir ${docdir}


distclean-local:
	-rm -rf autom4te.cache/ build-aux/ src/.deps/ src/trace/.deps/ 2> /dev/null || true
	-rm -f  aclocal.m4 autoscan.log configure.scan test-driver     2> /dev/null || true
