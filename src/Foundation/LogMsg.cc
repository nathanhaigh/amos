/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

//! @file 
//! @brief LogMsg class implementation

#include "LogMsg.hh"

//! Constructor sets parameters, and timestamps creation
LogMsg::LogMsg(std::string msg,
               CategoryInformation * category,
               MessageLevel messageLevel,
               int pid,
               bool forceLog)
{
  m_msg = msg;
  m_category = category;
  m_messageLevel = messageLevel;
  m_pid = pid;

  time(&m_timeStamp);
  m_doLog = forceLog || m_category->doLog(m_messageLevel);
}

//! Gets the category name
std::string LogMsg::getCategory() const
{
  return m_category->getCategoryName();
}

//! Gets the timestamp in time_t format
time_t LogMsg::getTimeStamp() const
{
  return m_timeStamp;
}

//! Gets the message level of the LogMsg
MessageLevel LogMsg::getMessageLevel() const
{
  return m_messageLevel;
}

//! Gets the msg std::string
const std::string & LogMsg::getMsg() const
{
  return m_msg;
}

//! Gets the pid
int LogMsg::getPID() const
{
  return m_pid;
}

//! Gets the message count
int LogMsg::getMsgCount() const
{
  return m_msgCount;
}

//! Sets the message count
void LogMsg::setMsgCount(int msgCount)
{
  m_msgCount = msgCount;
}


//! Returns if message should be logged or not
/*! m_doLog is set at creation in case category changes before message
 *  is ready. m_doLog is set based on if the category filterLevel and this
 *  message's messageLevel, but can be overwritten by using the forcelog flag
 *  at creation.
 */
bool LogMsg::doLog() const
{
  return m_doLog;
}

//! Determines if message is at MESSAGE_LEVEL_DEBUG_ERROR or lower
/*!
 *  @return If message is fatal
 */
bool LogMsg::isFatal() const
{
  return m_messageLevel <= MESSAGE_LEVEL_DEBUG_ERROR;
}

//! Gets a string with the name of the message level
/*! Converts known message levels into symbolic form
 */
std::string LogMsg::getMessageLevelStr() const
{
  std::string retval;
  switch (m_messageLevel)
  {
    case MESSAGE_LEVEL_DEBUG_HIGH: 
      {
        retval = "HIGH";
        break;
      }

    case MESSAGE_LEVEL_DEBUG_MEDIUM:
      {
        retval = "MED";
        break;
      }

    case MESSAGE_LEVEL_DEBUG_LOW:
      {
        retval = "LOW";
        break;
      }

    case MESSAGE_LEVEL_DEBUG_INFORMATION:
      {
        retval = "INFO";
        break;
      }

    case MESSAGE_LEVEL_DEBUG_WARNING:
      {
        retval = "WARN";
        break;
      }

    case MESSAGE_LEVEL_DEBUG_ERROR:
      {
        retval = "ERROR";
        break;
      }

    case MESSAGE_LEVEL_DEBUG_FATAL:
      {
        retval = "FATAL";
        break;
      }

    default:
      {
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%d", m_messageLevel);
        retval = buffer;
        break;
      }
  };

  return retval;
}



//! Writes the prefix and msg to an output stream
/*! Prefix format is:
 *  "YYYYMMDD|hhmmss|PID|CATEGORY|MESSAGE_LEVEL|COUNT: "
 */
std::ostream & operator<< (std::ostream & os, const LogMsg & logMsg)
{
  // Get the time stamp
  time_t rawtime = logMsg.getTimeStamp();
  tm * now = localtime(&rawtime);

  // Date/time string is actually fixed at 17 chars long, but thats an
  // unnatural boundary
  char buffer[32]; 
  strftime(buffer, sizeof(buffer), "%Y%m%d|%H%M%S|", now);

  // Write the prefix
  os << buffer
     << logMsg.getPID() << "|"
     << logMsg.getCategory() << "|" 
     << logMsg.getMessageLevelStr() << "|" 
     << logMsg.getMsgCount() << ": ";

  // Write the actual message
  os << logMsg.getMsg();

  return os;
}


