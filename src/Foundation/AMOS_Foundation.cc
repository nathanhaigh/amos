//! @file 
//! @brief AMOS_Foundation class implementation

/** @example example-minimal.cc
 *  Demonstrates a minimal use of AMOS_Foundation.\n\n
 *  This file can be used as a template for new programs to incorporate the
 *  services provided by AMOS_Foundation.
 */

/** @example example-minimal.out
 *  Sample output from example-minimal.\n\n
 *  Notice that by adding a few lines of code to instantiate a AMOS_Foundation
 *  object, all of the standard AMOS options are handled without complicated
 *  options handling code. In addition to easy and robust command line options
 *  handling, logging services and configuration file parsing are also 
 *  available for use immediately.
 */

#include "AMOS_Foundation.hh"

static char const libdependicies[]= 
  "$Dependencies: getopts_long, STL::{map, vector, list}, sstream";

static char const libversion[]=
  "$Version: 2.0 $Revision$";

//! Constructor: Initializes options
/*! Initializes Options, ConfigFile, and Logger based on command line
 *  options. Used for automatic incorporation of standard AMOS options. 
 *
 *  @param version Version information for program if --version or -V is given
 *  @param helptext Short help text to display if --help of -h is given
 *  @param dependencies Dependency information for --depend
 *  @param argc Standard argument count
 *  @param argv Standard array of strings for argument values
 */
AMOS_Foundation::AMOS_Foundation(std::string version, 
                                 std::string helptext,
                                 std::string dependencies,
                                 int argc,
                                 char ** argv)
{
  m_version = version;
  m_helptext = helptext;
  m_dependencies = dependencies;

  m_doOptionHelp = true;

  m_logger = NULL;
  m_config = NULL;

  m_doHelp    = 0;
  m_doAppend  = 0;
  m_doDepend  = 0;
  m_doVersion = 0;

  m_filterLevels = "";
  m_configFile = "";
  m_logfile = "";

  m_options = new Options (argc, argv);

  // Add the standard options
  m_options->addOptionResult("help|h", &m_doHelp, 
                             "Get this help");

  m_options->addOptionResult("version|V", &m_doVersion, 
                             "Get the version info");

  m_options->addOptionResult("configfile=s", &m_configFile,
                             "Set the config file");

  m_options->addOptionResult("depend", &m_doDepend,
                             "Get dependency information");

  m_options->addOptionResult("debug|filterlevel=s", &m_filterLevels, 
                             "Set filter levels for logging " \
                             "(fl[,Category=fl[...]]");

  m_options->addOptionResult("logfile=s", &m_logfile,
                             "Set logfile for logging");

  m_options->addOptionResult("append", &m_doAppend,
                             "Log messages should append to old logfile");

  m_options->standardOptionsComplete();
}

//! Deletes logger, config file, and options handlers
AMOS_Foundation::~AMOS_Foundation()
{
  if (m_logger) delete m_logger;
  if (m_config) delete m_config;
  if (m_options) delete m_options;
}

void AMOS_Foundation::printHelpText()
{
  std::cerr << m_helptext << std::endl;
  if (m_doOptionHelp)
  {
    m_options->printHelp();
  }
}

//! Handles standard AMOS options appropriately
void AMOS_Foundation::handleStandardOptions()
{
  m_options->parseOptions();
  
  if (m_doHelp)
  {
    printHelpText();
    throw ExitProgramNormally("Help Text Requested");
  }
  else if (m_doVersion)
  {
    std::cerr << m_version << std::endl;
    throw ExitProgramNormally("Version Infortmation Requested");
  }
  else if (m_doDepend)
  {
    std::cerr << "Dependencies: " << m_dependencies << std::endl;
    throw ExitProgramNormally("Dependency Information Requested");
  }

  if (m_configFile.length())
  {
    // Config was specified, load config file
    
    m_config = new ConfigFile(m_configFile);
  }

  // Set up logfiles
  std::string logfilename = m_options->getApplicationName() + ".log";
  std::string errorfilename = m_options->getApplicationName() + ".error";
 
  // See if a specific logfile was requested
  if (m_logfile.length())
  {
    logfilename = m_logfile;

    // Errorfile name is /path/to/logfile.error from
    // /path/to/logfile.log

    errorfilename = logfilename;
    int dotpos = errorfilename.rfind(".log");
    if (dotpos > 0)
    {
      errorfilename.erase(dotpos, errorfilename.length()-1);
    }

    errorfilename += ".error";
  }

  // Remove old log files if not in append mode
  if (!m_doAppend)
  {
    if (FileSystem::doesFileExist(logfilename.c_str()))
    {
      unlink(logfilename.c_str());
    }

    if (FileSystem::doesFileExist(errorfilename.c_str()))
    {
      unlink(errorfilename.c_str());
    }
  }

  // Check to see if logfile can be opened
  if (!FileSystem::isCreatableFile(logfilename.c_str()))
  {
    // Can't create this as requested, log to /tmp
    logfilename = "/tmp/" + m_options->getApplicationName() 
                  + "." + FileSystem::getPIDString();
    errorfilename = logfilename + ".error";
    logfilename += ".log";
  }

  // Create the logger (which is disabled until a default filter level is set
  m_logger = new Logger(logfilename.c_str(), 
                        errorfilename.c_str(),
                        m_options->getApplicationName(),
                        m_options->getInvocation());
  
  // --debug or --filterlevel was specified
  if (m_filterLevels.length())
  {
    m_logger->setFilterLevels(m_filterLevels);
  }
}

//! Gets version string
std::string AMOS_Foundation::getVersion()
{
  return m_version;
}

//! Gets Help Text
std::string AMOS_Foundation::getHelpText()
{
  return m_helptext;
}

//! Gets Command Line Option pointer
Options * AMOS_Foundation::getOptions()
{
  return m_options;
}

//! Gets Configuration File pointer
ConfigFile * AMOS_Foundation::getConfig()
{
  return m_config;
}

//! Gets the main log manager
/*! 
 *  @exception amosException If a log manager was not created, which means
 *                           handleStandardOptions was never called
 */
Logger * AMOS_Foundation::getLogger()
{
  if (!m_logger)
  {
    throw amosException("Call handleStandardOptions to create the log manager",
                        "AMOS_Foundation");
  }
  
  return m_logger;
}

//! Gets the default log for ease of use
LogCategory AMOS_Foundation::log()
{
  return getLogger()->getLog();
}

//! Disables printing help on each option
void AMOS_Foundation::disableOptionHelp()
{
  m_doOptionHelp = false;
}
