/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

//! @file
//! @brief LogCategory class interface

#ifndef LOGCATEGORY_HH
#define LOGCATEGORY_HH 1

#include <string>
#include <sstream>
#include "MessageLevel.hh"

#include "Logger.hh"
#include "CategoryInformation.hh"

using namespace std;


//! Derives from stringbuf so that sync messages can be intercepted
class LogStreamBuf : public std::stringbuf
{
public:
  LogStreamBuf(LogCategory * category);
  int sync();

private:
  //! The associated category
  LogCategory * m_category;
};


//! Provides an interface for logging to a specific category
/*! LogCategory provides an interface to underlying CategoryInformation. 
 *  LogCategory derives from ostream for insertion style logging, and 
 *  creates LogMsg objects for all log messages, and sends them to the log
 *  manager.
 */
class LogCategory : public std::ostream
{
public:
  LogCategory(const LogCategory & input);
  LogCategory(Logger * logger,
              std::string categoryName,
              MessageLevel messageLevel = MESSAGE_LEVEL_DEFAULT,
              MessageLevel filterLevel = MESSAGE_LEVEL_DEFAULT);

  LogCategory(CategoryInformation * categoryInformation,
              Logger * logger);
  ~LogCategory();

  LogCategory & operator= (const LogCategory & input);

  // Get a different category from log manager
  LogCategory getCategory(std::string categoryName,
                          MessageLevel messageLevel = MESSAGE_LEVEL_DEFAULT,
                          MessageLevel filterLevel = MESSAGE_LEVEL_DEFAULT);
  
  // Category Data Functions
  std::string getCategoryName() const;

  MessageLevel getFilterLevel() const;
  void         setFilterLevel(MessageLevel filterLevel);
  void         setFilterLevel(int          filterLevel);

  MessageLevel getMessageLevel() const;
  void         setMessageLevel(MessageLevel messageLevel);
  void         setMessageLevel(int          messageLevel);

  void         pushMessageLevel(MessageLevel messageLevel);
  void         pushMessageLevel(int          messageLevel);
  void         popMessageLevel();

  // Category Management
  void disable();

  // Log Messages
  void logMsg(const std::string & msg, 
              MessageLevel messageLevel=MESSAGE_LEVEL_DEFAULT) const;

  void debug       (const std::string & msg) const;
  void debugH      (const std::string & msg) const;
  void debugM      (const std::string & msg) const;
  void debugL      (const std::string & msg) const;
  void information (const std::string & msg) const;
  void warning     (const std::string & msg) const;
  void error       (const std::string & msg) const;
  void fatal       (const std::string & msg) const;
  void log         (const std::string & msg) const;

private:
  // ostream operations
  LogStreamBuf * rdbuf()                   const;
  std::string    str()                     const;
  void           str(const std::string & s);
  void           sync();
  bool           m_alreadySyncing;
  
  //! The current category
  CategoryInformation * m_category;

  //! The log manager
  Logger * m_logger;

  //! The pid of the program/thread that created this LogCategory
  int m_pid;

  //! The underling buffer is a friend so it can call back
  friend class LogStreamBuf;
  LogStreamBuf sb;
};



#endif
