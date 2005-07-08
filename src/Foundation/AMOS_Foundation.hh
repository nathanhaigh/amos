//! @file 
//! @brief AMOS_Foundation class interface

#ifndef AMOS_Foundation_HH
#define AMOS_Foundation_HH 1

#include <string>
#include "Options.hh"
#include "ConfigFile.hh"
#include "Logger.hh"
#include "FileSystem.hh"

//! Wrapper Class to ease use of Options, Configuration Files, and Logging
class AMOS_Foundation
{
public:
  AMOS_Foundation(std::string version, 
                  std::string helptext,
                  std::string dependencies,
                  int argc,
                  char ** argv);
  ~AMOS_Foundation();

  void handleStandardOptions();
  void printHelpText();

  std::string getVersion();
  std::string getHelpText();

  Options * getOptions();
  ConfigFile * getConfig();
  Logger * getLogger();

  LogCategory log();

  void disableOptionHelp();

private:
  std::string m_version;
  std::string m_helptext;
  std::string m_dependencies;

  Options * m_options;
  Logger * m_logger;
  ConfigFile * m_config;

  bool m_doOptionHelp;

  // Command line option results
  int m_doHelp;
  int m_doVersion;
  int m_doAppend;
  int m_doDepend;

  std::string m_configFile;
  std::string m_logfile;
  std::string m_filterLevels;
};

#endif
