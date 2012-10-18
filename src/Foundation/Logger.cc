/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

//! @file 
//! @brief Logger class implementation

/** @example example-logging.cc
 *  Demonstrates logging capabilities.\n\n
 *  Log messages are written to the log from three different categories, and
 *  at various message levels using both insertion style and string messages.\n
 *
 *  A separate log category should be used for different functional parts of 
 *  the program. In this example, "MAIN" (implicitly), "A", and "B" are used.\n
 *
 *  Different message levels should be used to note importance to the different 
 *  messages, so that very important messages will be noticed, but lower 
 *  priority messages can be filtered or ignored.
 */

/** @example example-logging-1.log
 *  Sample log from example-logging.\n\n
 *  In this example, --debug 7 is passed to example-logging, which
 *  sets the default filter level to MESSAGE_LEVEL_DEBUG_HIGH. This is the 
 *  most verbose filter level. By default, the name of the logfile is just
 *  the name of the program followed by .log, but in this case, an explicit
 *  logfile name is given. In either case, the name of the error file will be
 *  the same as the logfile, but .log will be replaced with .error. Also by
 *  default, the old logfile is deleted, unless the option --append is used.
 */

/** @example example-logging-1.error
 *  Sample error log from example-logging.\n\n
 *  Regardless of the filter level settings, log events with a message level
 *  greater than or equal to MESSAGE_LEVEL_DEBUG_ERROR will be written to the 
 *  error log as well.
 */

/** @example example-logging-2.log
 *  Sample log from example-logging.\n\n
 *  In this example, all categories are disabled by default, but category 'A'
 *  is enabled and its filter level is set to 3 (MESSAGE_LEVEL_DEBUG_WARNING).\n
 *  More categories could have been enabled by adding them to the command line 
 *  option filterlevel.\n\n
 *  The syntax is fl[,CATEGORY=fl[...]] where:\n
 *  fl is the filterlevel to set (0 disables the category)\n
 *  CATEGORY is the name of the category\n
 *  [...] means that as many categories can be set as desired\n
 */

/** @example example-logging-2.error
 *  Sample error log from example-logging.\n\n
 *  This has the same contents as example-logging-1.error, because error files
 *  are independent of the filter level settings.
 */

#include "Logger.hh"
#include <cstdlib>

//! Sets default values for class
/*! Note: Log is disabled until a filter level is set\n
 *
 *  Creates a category named "MAIN" at defaults for message and filter levels\n
 *  default filterlevel to MESSAGE_LEVEL_DEBUG_HIGH\n
 *  default messagelevel to MESSAGE_LEVEL_DEBUG_HIGH\n
 *  \n
 *  Sets m_logEnabled to false\n\n
 *
 *  @param logfile Path to log to
 *  @param errorfile Path to write error messages to
 *  @param programName Name of program for FINISH message
 *  @param invocation Invocation of program for START message
 */
Logger::Logger(const char * logfile,
               const char * errorfile,
               const std::string & programName,
               const std::string & invocation)
{
  m_startTime = 0;
  m_msgCount = 0;

  m_logEnabled = false;
  m_firstTimeEnabled = true;

  m_errorfile = NULL;

  m_pid = FileSystem::getPID();
  m_invocation = invocation;
  m_programName = programName;
  
  if (logfile)   addLogfile(logfile, true);
  if (errorfile) addLogfile(errorfile, false);
  
  // Add a new category named "MAIN" to the map as the default category
  m_defaultCategory = getCategoryInformation("MAIN",
                                             MESSAGE_LEVEL_DEBUG_HIGH,
                                             MESSAGE_LEVEL_DEBUG_HIGH);
}


//! Deconstructor disables log, deletes categories, closes logfiles
Logger::~Logger()
{
  disable();

  // Delete each category
  CategoryMapCIter i;

  for (i =  m_categoryMap.begin();
       i != m_categoryMap.end();
       i++)
  {
    delete i->second;
  }

  // Close Logfiles
  while (m_logfileList.size())
  {
    std::ofstream * tmpfile = m_logfileList.front();
    m_logfileList.pop_front();
    delete tmpfile;
  }

  // Close Error file
  if (m_errorfile)
  {
    delete m_errorfile;
  }
}

//! Simple function to check if the default filter level has been set
bool Logger::hasFilterLevel() const
{
  return !m_firstTimeEnabled;
}

//! Converts a string to a MessageLevel
/*! Internal function for converting a string to a MessageLevel.
 *  Note: Filter levels specified on the command line get a 10x multiplier
 *  to the real filter level used.
 *
 *  @param filterLevel String to convert
 *  @exception InvalidOptionError If string has characters other than '0'..'9'
 *  @exception InvalidOptionError If filterlevel is empty
 *  @return Filter level as a MessageLevel
 */
MessageLevel Logger::convertFilterLevel(const std::string & filterLevel) const
{
  if (filterLevel.length() == 0)
  {
    throw InvalidOptionError("Cant specify an empty filterlevel",
                             "Logger::convertFilterLevel");
  }

  std::string::size_type badIndex;

  badIndex = filterLevel.find_first_not_of("0123456789");

  if (badIndex != filterLevel.npos)
  {
    throw InvalidOptionError("Cant specify a non integer filter level in " +
                             filterLevel,
                             "Logger::convertFilterLevel");
  }

  return (MessageLevel) ((atoi(filterLevel.c_str()) * 10));
}


//! Splits a category=filterlevel pair and sets the filter level
/*! If no category is provided, it is assumed that the string is entirely
 *  a filter level, and the default filter level will be set.
 *
 *  @param filterPair String of category=filterlevel
 *  @param moreThanOnePair Flags that there is more than one pair of filter
 *                         levels set, so that setDefaultFilterLevel can be
 *                         forced to open the log
 */
void Logger::setFilterLevelPair(const std::string & filterPair,
                                bool moreThanOnePair)
{
  std::string filterLevelName;

  std::string::size_type splitIndex;
  int filterLevel;

  // Try to split the pair on "="
  splitIndex = filterPair.find("=");

  if (splitIndex == filterPair.npos)
  {
    // All of filterPair is setting the default filterLevel
    filterLevel = convertFilterLevel(filterPair);
    setDefaultFilterLevel(filterLevel, moreThanOnePair);
  }
  else
  {
    // There is a category name given
    std::string categoryName = filterPair.substr(0, splitIndex);
    std::string filterLevelName = filterPair.substr(splitIndex + 1, 
                                               filterPair.length()-splitIndex);

    filterLevel = convertFilterLevel(filterLevelName);
    getCategory(categoryName).setFilterLevel(filterLevel);

    LogCategory log = getLog();
    log.pushMessageLevel(MESSAGE_LEVEL_DEBUG_INFORMATION);
    log << "Set category " << categoryName 
        << " to filter level " << filterLevel << std::endl;
    log.popMessageLevel();
  }
}

//! Set the default filter level, and levels for multiple categories
/*!
 *  Syntax for setting multiple filter levels is:\n
 *  fl[,CATEGORY=fl[...]]\n
 *  fl means the filterlevel as a positive integer\n
 *  CATEGORY is the name of the category\n
 *  If no CATEGORY is provided, then the filterLevel will be set as the default
 *
 *  @param filterLevels String containing filterLevels to set
 */
void Logger::setFilterLevels(const std::string & filterLevels)
{
  // Roughly in perl:
  // foreach $filterpair split(',', $filterLevels)
  // {
  //   setFilterLevelPair($filterPair)
  // }
  
  std::string filterPair;

  std::string::size_type index = 0;
  std::string::size_type splitIndex;
  bool moreThanOnePair = false;
  while ((splitIndex = filterLevels.find(',', index)) != filterLevels.npos)
  {
    moreThanOnePair = true;

    filterPair = filterLevels.substr(index, splitIndex - index);
    setFilterLevelPair(filterPair, moreThanOnePair);
    index = splitIndex + 1;
  }

  // setFilterLevelPair on remaining string
  filterPair = filterLevels.substr(index, filterLevels.length() - index + 1),
  setFilterLevelPair(filterPair, moreThanOnePair);
}

//! Sets the default filter level for new categories and category "MAIN"
/*! Messages with a message level below the filter level are not written to
 *  the log.
 *
 *  @param filterLevel Filter level for all future categories
 *  @param forceFirstTime Force the firsttime enabled sequence, even
 *                        if the default level is set to MESSAGE_LEVEL_OFF
 *  @see CategoryInformation::setFilterLevel
 */
void Logger::setDefaultFilterLevel(MessageLevel filterLevel,
                                   bool forceFirstTime)
{
  if (m_firstTimeEnabled &&
     ((filterLevel != MESSAGE_LEVEL_OFF) || forceFirstTime))
  {
    enable();
    m_firstTimeEnabled = false;
  }
  
  m_defaultCategory->setFilterLevel(filterLevel);

  // Log what the default filter level was set to
  LogCategory log = getLog();
  log.pushMessageLevel(MESSAGE_LEVEL_DEBUG_INFORMATION);
  log << "Set default filter level to " << filterLevel << std::endl;
  log.popMessageLevel();
}

//! Simple wrapper 
void Logger::setDefaultFilterLevel(int filterLevel, bool forceFirstTime)
{
  setDefaultFilterLevel((MessageLevel) filterLevel, forceFirstTime);
}

//! Gets the default filter level from category "MAIN"
/*! 
 *  @return Current filter level
 */
MessageLevel Logger::getDefaultFilterLevel() const
{
  return m_defaultCategory->getFilterLevel();
}

//! Gets the default message level from category "MAIN"
/*!
 *  @return Current message level
 */
MessageLevel Logger::getDefaultMessageLevel() const
{
  return m_defaultCategory->getMessageLevel();
}

//! Sets the default message level and for category "MAIN"
/*! 
 *  @param messageLevel New default message level
 */
void Logger::setDefaultMessageLevel(MessageLevel messageLevel)
{
  m_defaultCategory->setMessageLevel(messageLevel);
}

//! Simple wrapper
void Logger::setDefaultMessageLevel(int messageLevel)
{
  setDefaultMessageLevel((MessageLevel) messageLevel);
}


//! Checks for the existence of a category
/*! Since only Logger can create or delete categories, checking the map
 *  will be accurate.
 */
bool Logger::hasCategory(std::string category) const
{
  return (m_categoryMap.find(category) != m_categoryMap.end());
}

//! Gets a LogCategory for category "MAIN"
LogCategory Logger::getLog()
{
  return LogCategory(m_defaultCategory, this);
}

//! Utility Function to find the CategoryInformation for a given categoryName
/*! Returns the internal information for a category from the category map.
 *  If the category is not found, it is added to the map. Only Logger can
 *  create new CategoryInformation objects. \n\n
 *
 *  Note: MESSAGE_LEVEL_DEFAULT means to use the existing level for existing 
 *  categories or to use the default level for new categories.
 *
 *  @param categoryName Name of category to get
 *  @param messageLevel Message level of category to get
 *  @param filterLevel FilterLevel of category to get
 *  @return CategoryInformation of categoryName
 */
CategoryInformation * Logger::getCategoryInformation(std::string categoryName, 
                                                     MessageLevel messageLevel,
                                                     MessageLevel filterLevel)
{
  CategoryInformation * retval;

  if (hasCategory(categoryName))
  {
    retval = m_categoryMap[categoryName];

    if (messageLevel != MESSAGE_LEVEL_DEFAULT)
    {
      retval->setMessageLevel(messageLevel);
    }

    if (filterLevel != MESSAGE_LEVEL_DEFAULT)
    {
      retval->setFilterLevel(filterLevel);
    }
  }
  else
  {
    if (messageLevel == MESSAGE_LEVEL_DEFAULT)
    {
      messageLevel = m_defaultCategory->getMessageLevel();
    }

    if (filterLevel == MESSAGE_LEVEL_DEFAULT)
    {
      filterLevel = m_defaultCategory->getFilterLevel();
    }

    // Create and store this new category in the main map
    retval = new CategoryInformation(categoryName, messageLevel, filterLevel);
    m_categoryMap[categoryName] = retval;
  }

  return retval;
}

//! Gets a LogCategory wrapping the CategoryInformation of categoryName
/*!
 *  @see getCategoryInformation
 */
LogCategory Logger::getCategory(std::string categoryName,
                                MessageLevel messageLevel,
                                MessageLevel filterLevel)
{
  return LogCategory(getCategoryInformation(categoryName,
                                            messageLevel,
                                            filterLevel),
                     this);
}

//! Enables the log if it was previously disabled
/*! Opens the log file if it is the first time the log is enabled
 *  @see disable
 */
void Logger::enable()
{
  if (!m_logEnabled)
  {
    if (m_firstTimeEnabled)
    {
      openLogFiles();
    }

    m_logEnabled = true;
    writeOpenLogSequence(NULL);
  }
}

//! Disables the log for all messages if it was previously enabled
/*! Writes close log sequence to log files
 *  @see enable
 */
void Logger::disable()
{
  if (m_logEnabled)
  {
    writeLog(new LogMsg(getCloseLogStr(), 
                        m_defaultCategory, 
                        MESSAGE_LEVEL_DEBUG_INFORMATION, 
                        m_pid, 
                        true),
             true, NULL);

    m_logEnabled = false;
  }
}

//! Returns if log is currently enabled
bool Logger::isEnabled() const
{
  return m_logEnabled;
}



//! Determines if message should be written to the log
/*! Determined by m_logEnabled which is set when a default filter level is set
 *
 *  @return Boolean indicating if message should be written
 */
bool Logger::doLog() const
{
  return m_logEnabled;
}

//! Opens the errorfile
/*!
 *  @exception UnableToOpenError
 *  @see FileSystem::openOutputFile
 */
void Logger::openErrorFile()
{
  if (m_errorfileName.length())
  {
    m_errorfile = FileSystem::openOutputFile(m_errorfileName);
  }
}


//! Opens the log files
/*!
 *  @exception UnableToOpenError
 *  @see FileSystem::openOutputFile
 */
void Logger::openLogFiles()
{
  StringListCIter pFilename;

  for (pFilename  = m_logfileNameList.begin();
       pFilename != m_logfileNameList.end();
       pFilename++)
  {
    std::ofstream * logfile = FileSystem::openOutputFile(*pFilename);

    m_logfileList.push_back(logfile);
    addOutputStream(logfile);
  }
}



//! Add a log file to output list
/*!
 *  @param filename File to open
 *  @param isLogFile If File is for logging (vs error only)
 */
void Logger::addLogfile(const char * filename, bool isLogFile)
{
  if (filename)
  {
    if (isLogFile)
    {
      m_logfileNameList.push_back((std::string)filename);
    }
    else
    {
      m_errorfileName = (std::string) filename;
    }
  }
}


//! Add an output stream to the list of streams to log to
void Logger::addOutputStream(std::ostream * os)
{
  // Add this stream to the list of output streams
  m_outputStreamList.push_back(os);

  if (m_logEnabled) 
  {
    // This sequence was added after the log was enabled, 
    // write open log sequence
    
    writeOpenLogSequence(os);
  }
}

//! Takes a message, formats it, and logs it to log and possibly error file
/*! If it is a fatal error, also calls writeFatal().\n\n
 *  Note: Deletes logMsg when done.
 *
 *  @param logMsg Message to log
 *  @param incrementCount If this object should increment the global msg count
 *  @param os Output stream to write to, NULL means write to all output stream
 */  
void Logger::writeLog(LogMsg * logMsg,
                      bool incrementCount,
                      std::ostream * os)
{
  if (logMsg)
  {
    bool dolog = doLog() && logMsg->doLog();

    if (dolog || logMsg->isFatal())
    {
      logMsg->setMsgCount(m_msgCount);

      if (incrementCount)
      {
        m_msgCount++;
      }

      if (dolog)
      {
        if (os == NULL)
        {
          std::list<std::ostream *>::const_iterator hOutputStream;

          for (hOutputStream =  m_outputStreamList.begin(); 
               hOutputStream != m_outputStreamList.end(); 
               hOutputStream++)
          {
            **hOutputStream << *logMsg << std::endl << std::flush;
          }
        }
        else
        {
          *os << *logMsg << std::endl << std::flush;
        }
      }

      if (logMsg->isFatal())
      {
        writeFatal(logMsg);
      }
    }

    delete logMsg;
  }
}


//! Writes fully formated message to the error file
/*! Messages are automatically forwarded to writeFatal from writeLog\n
 *
 *  Note: If writeFatal is called before the log has been enabled (and 
 *  consequently before the errorfile is opened) it will open the errorfile.
 *
 *  @param logMsg Message to write
 */
void Logger::writeFatal(LogMsg * logMsg)
{
  if (logMsg)
  {
    if (!m_errorfile)
    {
      openErrorFile();
    }

    if (m_errorfile)
    {
      *m_errorfile << *logMsg << std::endl << std::flush;
    }
  }
}


// String Functions

//! Gets the START message for log files
std::string Logger::getOpenLogStr()
{
  if (m_startTime == 0);
  {
    // if this is the first time the log has been enabled,
    // mark the start time
    time(&m_startTime); 
  }

  return "START: " + m_invocation; 
}

//! Gets the name of the user (based on getuid) for logfile
std::string Logger::getUsernameStr() const
{
  std::string username = "Unknown"; 

  if (struct passwd * userInfo = getpwuid(getuid()))
  {
    username = (std::string) userInfo->pw_name;
  }

  return "Username: " + username;
}

//! Gets the name of the hostname for log file
std::string Logger::getHostnameStr() const
{
  std::string hostname = "Unknown";
  char buffer[256]; // MAXHOSTNAMELEN for solaris 

  if (!gethostname(buffer, sizeof(buffer)))
  {
    hostname = (std::string) buffer;
  }

  return "Hostname: " + hostname;
}

//! Gets FINISH message for log files
std::string Logger::getCloseLogStr() const
{
  time_t endTime;
  time(&endTime);

  double elapsed = difftime(endTime, m_startTime);
  
  int num_days = (int) (elapsed / 86400);
            elapsed -= num_days * 86400;

  int num_hours = (int) (elapsed / 3600);
            elapsed -= num_hours * 3600;

  int num_min = (int) (elapsed / 60);
            elapsed -= num_min * 60;

  int num_sec = (int) elapsed;

  char buffer[16];
  snprintf(buffer, sizeof(buffer),
           "%03d-%02d:%02d:%02d", num_days, num_hours, num_min, num_sec);

  return "FINISH: " + m_programName + ", elapsed " + buffer;
}

//! Writes the sequence of START, username, hostname
/*! Checks if it should write the sequence before writing it
 *  @param os Output stream, or NULL for all logfiles
 */
void Logger::writeOpenLogSequence(std::ostream * os)
{
  writeLog(new LogMsg(getOpenLogStr(), 
                      m_defaultCategory, 
                      MESSAGE_LEVEL_DEBUG_INFORMATION, 
                      m_pid, 
                      true),
           os == NULL, os);

  writeLog(new LogMsg(getUsernameStr(),
                      m_defaultCategory, 
                      MESSAGE_LEVEL_DEBUG_INFORMATION,
                      m_pid,
                      true),
           os == NULL, os);

  writeLog(new LogMsg(getHostnameStr(), 
                      m_defaultCategory, 
                      MESSAGE_LEVEL_DEBUG_INFORMATION, 
                      m_pid,
                      true),
           os == NULL, os);
}
