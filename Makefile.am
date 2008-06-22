##-- SUBDIRS TO RECURSE
SUBDIRS = \
	src \
	doc

EXTRA_DIST = test

dist-hook:
	rm -rf `find $(distdir)/test -type d -name CVS`

ACLOCAL_AMFLAGS = -I config
