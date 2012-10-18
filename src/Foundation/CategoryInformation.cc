/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

//! @file 
//! @brief CategoryInformation class implementation

#include "CategoryInformation.hh"

//! Constuctor sets the category information
CategoryInformation::CategoryInformation(std::string categoryName,
                                         MessageLevel messageLevel,
                                         MessageLevel filterLevel)
{
  m_categoryName = categoryName;

  setMessageLevel(messageLevel);
  setFilterLevel(filterLevel);
}

//! Empty Deconstructor
CategoryInformation::~CategoryInformation()
{

}

//! Gets the current message level
/*!
 *  @return Current message Level
 */
MessageLevel CategoryInformation::getMessageLevel() const
{
  return m_messageLevel;
}

//! Sets the message level
/*!
 *  @param messageLevel New Message level
 */
void CategoryInformation::setMessageLevel(MessageLevel messageLevel)
{
  if (messageLevel == MESSAGE_LEVEL_DEFAULT)
  {
    throw amosException("Internal Error: Can't set message level to be "\
                        "MESSAGE_LEVEL_DEFAULT for category " + 
                        m_categoryName,
                        "CategoryInformation::setMessageLevel");

  }

  m_messageLevel = messageLevel;
}

//! Gets the current filter level
/*!
 *  @return Current filter Level
 */
MessageLevel CategoryInformation::getFilterLevel() const
{
  return m_filterLevel;
}

//! Sets the current filter level
/*!
 *  @param filterLevel New filter Level
 */
void CategoryInformation::setFilterLevel(MessageLevel filterLevel)
{
  if (filterLevel == MESSAGE_LEVEL_DEFAULT)
  {
    throw amosException("Internal Error: Can't set filter level to be "\
                        "MESSAGE_LEVEL_DEFAULT for category " + 
                        m_categoryName,
                        "CategoryInformation::setFilterLevel");

  }

  if (filterLevel == MESSAGE_LEVEL_OFF)
  {
    m_categoryEnabled = false;
  }
  else
  {
    m_categoryEnabled = true;
  }

  m_filterLevel = filterLevel;
}

//! Gets the name of the category
/*!
 *  @return Name of the category
 */
std::string CategoryInformation::getCategoryName() const
{
  return m_categoryName;
}

//! Pushes the current message level onto a stack, and sets a new messagelevel
/*!
 *  @param messageLevel Message level to set
 *  @see popMessageLevel
 */
void CategoryInformation::pushMessageLevel(MessageLevel messageLevel)
{
  m_messageLevelStack.push_front(m_messageLevel);
  m_messageLevel = messageLevel;
} 

//! Pops a message level off of a stack and sets that to be the new messagelevel
/*!
 *  @see pushMessageLevel
 */
void CategoryInformation::popMessageLevel()
{
  if (m_messageLevelStack.size())
  {
    m_messageLevel = m_messageLevelStack.front();
    m_messageLevelStack.pop_front();
  }
}

//! Checks if category is enabled
bool CategoryInformation::isEnabled()
{
  return m_categoryEnabled;
}

//! Function to check that category is enabled and message is not filtered
/*! A message will be logged if it's messagelevel is less than or equal to 
 *  the filter level
 *
 *  @return If message should be logged
 */
bool CategoryInformation::doLog(MessageLevel messageLevel) const
{
  return m_categoryEnabled && (messageLevel <= m_filterLevel);
}

