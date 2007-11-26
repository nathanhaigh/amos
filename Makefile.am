##-- SUBDIRS TO RECURSE
SUBDIRS = \
	src \
	doc \
    test

EXTRA_DIST = test

dist-hook:
	rm -rf `find $(distdir)/test -type d -name CVS`
