/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

//! @file
//! @brief CategoryInformation class interface

#ifndef CATEGORYINFORMATION_HH
#define CATEGORYINFORMATION_HH 1

#include <string>
#include <list>
#include "MessageLevel.hh"

class Logger;
#include "Logger.hh"

//! Hold the internal information for each category.
/*! A "Category" would be created for each different functional section of
 *  a program. They are created and destroyed only by the log manager to store 
 *  information about each category throughout the lifetime of the logger. 
 *  Each category has independent fitlerLevels, messageLevels, 
 *  messageLevel stacks, and can be independently turned on or off. 
 */
class CategoryInformation
{
public:
  std::string getCategoryName() const;

  bool         isEnabled();
  MessageLevel getFilterLevel() const;
  void         setFilterLevel(MessageLevel filterLevel);

  MessageLevel getMessageLevel() const;
  void         setMessageLevel(MessageLevel messageLevel);

  void         pushMessageLevel(MessageLevel messageLevel);
  void         popMessageLevel();

  bool         doLog(MessageLevel messagelevel) const;

private:
  // Only the log manager can create or delete category information
  friend class Logger;
  CategoryInformation(std::string categoryName,
                      MessageLevel messageLevel,
                      MessageLevel filterLevel);

  ~CategoryInformation();

  //! Determines if any messages should be logged from this category
  bool m_categoryEnabled;

  //! Current message level
  MessageLevel m_messageLevel;

  //! Current filter level
  MessageLevel m_filterLevel;

  //! Name of the category
  std::string m_categoryName;

  //! Stack of old message levels for quick changes
  std::list<MessageLevel> m_messageLevelStack;
};

#endif

