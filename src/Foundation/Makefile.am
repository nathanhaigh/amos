##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

AM_CXXFLAGS = -I$(top_builddir)/src/GNU

amoslib_LIBRARIES = \
   libAMOSFoundation.a

amosinclude_HEADERS = \
  AMOS_Foundation.hh      \
  CategoryInformation.hh  \
  ConfigFile.hh           \
  ConfigSection.hh        \
  Exceptions.hh           \
  FileSystem.hh           \
  LogCategory.hh          \
  Logger.hh               \
  LogMsg.hh               \
  MessageLevel.hh         \
  OptionResult.hh         \
  Options.hh


libAMOSFoundation_a_SOURCES = \
  CategoryInformation.cc \
  ConfigFile.cc          \
  ConfigSection.cc       \
  Exceptions.cc          \
  FileSystem.cc          \
  LogCategory.cc         \
  Logger.cc              \
  LogMsg.cc              \
  OptionResult.cc        \
  Options.cc             \
  AMOS_Foundation.cc
