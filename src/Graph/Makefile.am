##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

##-- TO BE TESTED
check_PROGRAMS = Test

##-- test
Test_LDADD = \
	$(top_builddir)/src/AMOS/libAMOS.a

Test_SOURCES = \
	Test.cc


#-- TO BE INSTALLED
amoslib_LIBRARIES = \
	libGraph.a

amosinclude_HEADERS = \
	Edge.hh \
	Graph.hh \
	IEdge.hh \
	IGraph.hh \
	INode.hh \
	Node.hh


##-- libGraph.a
libGraph_a_SOURCES = \
	Edge.cc \
	Graph.cc \
	Node.cc 


##-- END OF MAKEFILE --##
