/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

//! @file 
//! @brief LogCategory class implementation

#include "LogCategory.hh"

//! Constructor asks the log manager for its category information
LogCategory::LogCategory(Logger * logger,
                         std::string categoryName,
                         MessageLevel messageLevel,
                         MessageLevel filterLevel)
  : std::ostream(&sb), sb(this)
{
  m_alreadySyncing = false;
  m_pid = FileSystem::getPID();
  m_logger = logger;
  m_category = m_logger->getCategoryInformation(categoryName,
                                                messageLevel,
                                                filterLevel);
}

//! Constuctor saves pointers to the category information and main logger
/*! Also, each LogCategory queries for its own pid at creation
 */
LogCategory::LogCategory(CategoryInformation * categoryInformation,
                         Logger * logger)
  : std::ostream(&sb), sb(this)
{
  m_pid = FileSystem::getPID();
  m_logger = logger;
  m_category = categoryInformation;
}

//! Copy Constructor copies everything but the stream
LogCategory::LogCategory(const LogCategory & input)
  : std::ostream(&sb), sb(this)
{
  m_pid = FileSystem::getPID();
  m_logger = input.m_logger;
  m_category = input.m_category;
}

//! Assignment copies everything but the stream
LogCategory & LogCategory::operator= (const LogCategory & input)
{
  m_pid = FileSystem::getPID();
  m_logger = input.m_logger;
  m_category = input.m_category;

  return *this;
}

//! Deconstructor flushes the stream 
/*! CategoryInformation can only be deleted by the main logger, so it is not
 *  deleted, nor is the log manager pointer. \n
 */
LogCategory::~LogCategory()
{
  sync();
}

//! Takes the data in the buffer and logs it at current messagelevel
/*! The deconstructor also calls sync, but if there is nothing in the
 *  buffer it wont be logged.\n\n
 *
 *  *log << endl; // an empty line is written into the log\n
 *  *log << "a" // Not written to log until flushed by endl or deconstructor
 *  delete log; // an empty line won't be written to the log\n
 *  *log << "a"; delete log; // "a" will be flushed to the log\n
 *  
 *  @see logMsg
 */
void LogCategory::sync()
{
  if (m_alreadySyncing)
  {
    // Under 2.95 and 2.96, msg=str() triggers a sync, which was causing an
    // infinite recurse. Detect this, and don't recurse.

    return;
  }

  // Get the stream buffer
  m_alreadySyncing = true;
  std::string msg = str();
  m_alreadySyncing = false;
  
  // Reset the buffer
  str("");

  // Split the string at the newline, and log each substring
  std::string::size_type startpos = 0;
  std::string::size_type endpos = 0;

  std::string::size_type len = msg.length();

  while (startpos < len)
  {
    endpos = msg.find('\n', startpos);

    if (endpos == msg.npos)
    {
      endpos = len + 1;
    }

    // Pass the substring to logMsg 
    const std::string & logmsg = msg.substr(startpos, (endpos - startpos));
    logMsg(logmsg);

    // Advance for next iteration
    startpos = endpos + 1;
  }
}

//! Creates LogMsg object and sends it to log manager for writing
/*! Logs a new LogMsg with the message
 *
 *  @see Logger::writeLog
 *  @param msg Message to log
 *  @param messageLevel Message Level of message
 */
void LogCategory::logMsg(const std::string & msg, MessageLevel messageLevel) const
{
  if (messageLevel == MESSAGE_LEVEL_DEFAULT)
  {
    messageLevel = m_category->getMessageLevel();
  }

  m_logger->writeLog(new LogMsg(msg, m_category, messageLevel, m_pid));
}


//! Gets a LogCategory from the main logger
/*! @see Logger::getCategory
 *  @return A LogCategory with its category of categoryName, and levels set
 */
LogCategory LogCategory::getCategory(std::string categoryName,
                                     MessageLevel messageLevel,
                                     MessageLevel filterLevel)
{
  return m_logger->getCategory(categoryName, messageLevel, filterLevel);
}

//! Wrapper for m_category->getMessageLevel()
/*! @see CategoryInformation::getMessageLevel
 *  @return Message level
 */
MessageLevel LogCategory::getMessageLevel() const
{
  return m_category->getMessageLevel();
}

//! Wrapper for m_category->setMessageLevel()
/*! @see CategoryInformation::setMessageLevel
 *  @param messageLevel New message level
 */
void LogCategory::setMessageLevel(MessageLevel messageLevel)
{
  m_category->setMessageLevel(messageLevel);
}

//! Simple wrapper
/*! @see LogCategory::setMessageLevel(MessageLevel)
 *  @param messageLevel New message level
 */
void LogCategory::setMessageLevel(int messageLevel)
{
  setMessageLevel((MessageLevel) messageLevel);
}

//! Wrapper for m_category->getFilterLevel()
/*! @see CategoryInformation::getFilterLevel
 *  @return Filter level
 */
MessageLevel LogCategory::getFilterLevel() const
{
  return m_category->getFilterLevel();
}

//! Wrapper for m_category->setFilterLevel()
/*! @see CategoryInformation::setFilterLevel
 *  @param filterLevel New filter level
 */
void LogCategory::setFilterLevel(MessageLevel filterLevel)
{
  m_category->setFilterLevel(filterLevel);
}

//! Simple Wrapper
/*! @see LogCategory::setFilterLevel(MessageLevel)
 *  @param filterLevel New filter level
 */
void LogCategory::setFilterLevel(int filterLevel)
{
  setFilterLevel((MessageLevel) filterLevel);
}

//! Wrapper for m_category->getCategoryName()
/*! @see CategoryInformation::getCategoryName
 *  @return Name of category
 */
std::string LogCategory::getCategoryName() const
{
  return m_category->getCategoryName();
}

//! Wrapper for m_category->pushMessageLevel()
/*! @see CategoryInformation::pushMessageLevel
 *  @param messageLevel New message level
 */
void LogCategory::pushMessageLevel(MessageLevel messageLevel)
{
  m_category->pushMessageLevel(messageLevel);
} 

//! Simple Wrapper
/*! @see LogCategory::pushMessageLevel(MessageLevel)
 *  @param messageLevel New message level
 */
void LogCategory::pushMessageLevel(int messageLevel)
{
  m_category->pushMessageLevel((MessageLevel) messageLevel);
}

//! Wrapper for m_category->popMessageLevel()
/*! @see CategoryInformation::popMessageLevel
 */
void LogCategory::popMessageLevel()
{
  m_category->popMessageLevel();
}

//! Wrapper for m_category->disable()
/*! @see CategoryInformation::disable
 */
void LogCategory::disable()
{
  m_category->setFilterLevel(MESSAGE_LEVEL_OFF);
}




// logMsg wrappers

//! Writes a message with message level at MESSAGE_LEVEL_DEBUG_HIGH
/*! 
 *  @param msg String to log 
 *  @see logMsg
 */
void LogCategory::debug(const std::string & msg) const
{
  logMsg(msg, MESSAGE_LEVEL_DEBUG_HIGH);
}

//! Writes a message with message level at MESSAGE_LEVEL_DEBUG_HIGH
/*! 
 *  @param msg String to log 
 *  @see logMsg
 */
void LogCategory::debugH(const std::string & msg) const
{
  logMsg(msg, MESSAGE_LEVEL_DEBUG_HIGH);
}

//! Writes a message with message level at MESSAGE_LEVEL_DEBUG_MEDIUM
/*! 
 *  @param msg String to log 
 *  @see logMsg
 */
void LogCategory::debugM(const std::string & msg) const
{
  logMsg(msg, MESSAGE_LEVEL_DEBUG_MEDIUM);
}

//! Writes a message with message level at MESSAGE_LEVEL_DEBUG_LOW
/*!
 *  @param msg String to log 
 *  @see logMsg
 */
void LogCategory::debugL(const std::string & msg) const
{
  logMsg(msg, MESSAGE_LEVEL_DEBUG_LOW);
}

//! Writes a message with message level at MESSAGE_LEVEL_DEBUG_INFORMATION
/*!
 *  @param msg String to log 
 *  @see logMsg
 */
void LogCategory::information(const std::string & msg) const
{
  logMsg(msg, MESSAGE_LEVEL_DEBUG_INFORMATION);
}

//! Writes a message with message level at MESSAGE_LEVEL_DEBUG_WARNING
/*!
 *  @param msg String to log 
 *  @see logMsg
 */
void LogCategory::warning(const std::string & msg) const
{
  logMsg(msg, MESSAGE_LEVEL_DEBUG_WARNING);
}

//! Writes a message with message level at MESSAGE_LEVEL_DEBUG_ERROR
/*!
 *  @param msg String to log 
 *  @see logMsg
 */
void LogCategory::error(const std::string & msg) const
{
  logMsg(msg, MESSAGE_LEVEL_DEBUG_ERROR);
}

//! Writes a message with message level at MESSAGE_LEVEL_DEBUG_FATAL
/*!
 *  @param msg String to log 
 *  @see logMsg
 */
void LogCategory::fatal(const std::string & msg) const
{
  logMsg(msg, MESSAGE_LEVEL_DEBUG_FATAL);
}

//! Log a message at the active message level 
/*!
 *  @param msg String to log 
 *  @see logMsg
 */
void LogCategory::log(const std::string & msg) const
{
  logMsg(msg, MESSAGE_LEVEL_DEFAULT);
}

/////////////////////

//! Calls the stringbuf initializer, sets the category
LogStreamBuf::LogStreamBuf(LogCategory * category) 
 : std::stringbuf(std::ios::in|std::ios::out)
{
  m_category = category;
}


//! Reports the sync to the category, and passes the sync to the stringbuf
int LogStreamBuf::sync()
{
  m_category->sync();
  return std::stringbuf::sync();
}
 
//! Returns the str of the stringbuf
std::string LogCategory::str() const
{
  return rdbuf()->str();
}

//! Set the str of the stringbuf
void LogCategory::str(const std::string & s)
{
  rdbuf()->str(s);
}

//! Returns a pointer to the associated stringbuffer
LogStreamBuf * LogCategory::rdbuf() const
{
  return const_cast<LogStreamBuf *>(&sb);
}
