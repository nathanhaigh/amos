/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

//! @file 
//! @brief LogMsg class interface

#ifndef LOGMSG_HH
#define LOGMSG_HH 1

#include <string>
#include <time.h>
#include "MessageLevel.hh"

class Logger;
class CategoryInformation;
#include "CategoryInformation.hh"

//! Holds information for each item to be logged
/*! Holds the raw string, message level, category information, and other 
 *  attributes of each item to send to the log.
 */
class LogMsg
{
public:
  LogMsg(std::string msg,
         CategoryInformation * category,
         MessageLevel messageLevel,
         int pid,
         bool forceLog = false);

  // Getters and Setters
  void                setMsgCount(int msgCount);

  int                 getPID()             const;
  int                 getMsgCount()        const;
  time_t              getTimeStamp()       const;
  MessageLevel        getMessageLevel()    const;
  std::string         getMessageLevelStr() const;
  std::string         getCategory()        const;

  const std::string & getMsg()             const;

  // Message Level testing
  bool doLog()    const;
  bool isFatal()  const;

private:
  //! Message to Log
  std::string m_msg;

  //! Category message is associated with
  CategoryInformation * m_category;

  //! Timestamp of message
  time_t m_timeStamp;
  
  //! Message level
  MessageLevel m_messageLevel;

  //! If this message should be logged (as determined at creation)
  bool m_doLog;

  //! Holds the pid of orignating thread/program
  int m_pid;

  //! Holds the global msg count of this message
  int m_msgCount;
};

std::ostream & operator<< (std::ostream & os, const LogMsg & logMsg);

#endif
