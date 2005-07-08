/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

//! @file 
//! @brief Logger class interface

#ifndef LOGGER_HH
#define LOGGER_HH 1

#include <map>
#include <list>
#include <string>
#include <fstream>

#include <time.h>
#include <pwd.h>

#include "FileSystem.hh"
#include "MessageLevel.hh"

class LogMsg;
#include "LogMsg.hh"

class CategoryInformation;
#include "CategoryInformation.hh"

class LogCategory;
#include "LogCategory.hh"

typedef std::list<CategoryInformation *> CategoryStack;
typedef std::list<std::string>::const_iterator StringListCIter;
typedef std::map<std::string, CategoryInformation *> CategoryMap;
typedef CategoryMap::const_iterator CategoryMapCIter;


//! Manager for logging operations, acts as a backend to write to
/*! Manages the map of all categories, takes LogMsg and writes them to open log
 *  files, handles default filter levels and message levels
 */
class Logger
{
public:
  Logger(const char * logfile,
         const char * errorfile,
         const std::string & programName,
         const std::string & invocation);
  ~Logger();

  void addLogfile(const char * filename, bool isLogFile=true);
  void addOutputStream(std::ostream * os);
  
  // Defaults
  void         setFilterLevels(const std::string & filterLevels);
  bool         hasFilterLevel() const;

  MessageLevel getDefaultFilterLevel() const;
  void         setDefaultFilterLevel(MessageLevel filterLevel,
                                     bool forceFirstTime = false);
  void         setDefaultFilterLevel(int          filterLevel,
                                     bool forceFirstTime = false);

  MessageLevel getDefaultMessageLevel() const;
  void         setDefaultMessageLevel(MessageLevel messageLevel);
  void         setDefaultMessageLevel(int          messageLevel);


  // Category Management
  LogCategory getCategory(std::string categoryName,
                          MessageLevel messageLevel = MESSAGE_LEVEL_DEFAULT,
                          MessageLevel filterLevel = MESSAGE_LEVEL_DEFAULT);

  CategoryInformation * getCategoryInformation(std::string categoryName,
                            MessageLevel messageLevel = MESSAGE_LEVEL_DEFAULT,
                            MessageLevel filterLevel = MESSAGE_LEVEL_DEFAULT);

  LogCategory getLog();

  // Logging Master Control
  void enable();
  void disable();
  bool isEnabled() const;

  // Main logging function
  void writeLog(LogMsg * logMsg,
                bool incrementCount = true,
                std::ostream * os = NULL);

private:
  void writeFatal(LogMsg * logMsg);

  MessageLevel convertFilterLevel(const std::string & filterLevel) const;
  void         setFilterLevelPair(const std::string & filterPair,
                                  bool moreThanOnePair);

  bool hasCategory(std::string category) const;
  bool doLog() const;

  void openErrorFile();
  void openLogFiles();

  void writeOpenLogSequence(std::ostream * os);
  std::string getOpenLogStr();
  std::string getUsernameStr() const;
  std::string getHostnameStr() const;

  std::string getCloseLogStr() const;

  //! Boolean to determine if log is enabled
  bool m_logEnabled; 

  //! Determines if this is the first time the log is enabled
  bool m_firstTimeEnabled;

  //! Information for default category for getLog
  CategoryInformation * m_defaultCategory;

  //! Maps category names to category information
  CategoryMap m_categoryMap;

  //! Counter for log messages
  int m_msgCount;

  //! PID of program/thread that created the logger
  int m_pid;

  //! List of Output Streams
  std::list<std::ostream *> m_outputStreamList;
  
  //! List of logfiles
  std::list<std::ofstream *> m_logfileList;

  //! List of log file names
  std::list<std::string> m_logfileNameList;

  //! Name of errorfile
  std::string m_errorfileName;

  //! Errorfile
  std::ofstream * m_errorfile;

  //! Marks when log is first enabled
  time_t m_startTime;

  //! Invocation of program
  std::string m_invocation;

  //! Name of program
  std::string m_programName;
};

#endif
