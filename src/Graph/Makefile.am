##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

##-- TO BE TESTED
check_PROGRAMS = Test

##-- GLOBAL INCLUDE
AM_CPPFLAGS = -I$(top_srcdir)/src/AMOS 

##-- test
Test_LDADD = \
	$(top_builddir)/src/Graph/libGraph.a

Test_SOURCES = \
	Test.cc


#-- TO BE INSTALLED
amoslib_LIBRARIES = \
	libGraph.a 

amosinclude_HEADERS = \
	INode.hh \
	Node.hh \
	IEdge.hh \
	Edge.hh \
	Graph.hh \
	IGraph.hh \
	SubGraph.hh \
	CompositeNode.hh



##-- libGraph.a
libGraph_a_SOURCES = \
	Edge.cc \
	Graph.cc \
	Node.cc \
	SubGraph.cc \
	CompositeNode.cc


##-- END OF MAKEFILE --##
