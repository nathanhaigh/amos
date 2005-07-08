/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

//! @file
//! @brief ConfigSection class interface

#ifndef CONFIGSECTION_HH
#define CONFIGSECTION_HH 1

#include <map>
#include <list>
#include <string>
#include <iostream>

#include "Exceptions.hh"

// Predeclare to avoid circular references
class ConfigFile;

//! Class to manage individual sections of a config file
class ConfigSection
{
public:
  ConfigSection(ConfigSection * parent, std::string sectionName);
  ~ConfigSection();

  ConfigSection * findSection(std::string sectionName);
  void printSection(int indentLevel=0, std::ostream & os=std::cout);

  std::string getSectionName();
  std::string getFullSectionName();
  std::string getFullVariableName(std::string variableName);

  ConfigSection * getNextSection();
  std::string getNextVariable();

  std::string getValue(std::string variableName);
  bool hasVariable(std::string variableName);

  void resetIterators();

protected:
  ConfigSection * addNewSection(std::string sectionName);

  void resolveSubstitutions();
  void addVariable(const char * varLine);
  static std::string parseSectionName(const char * sectionName);
 
private:
  void addSection(ConfigSection * newSection);
  static void stripWhitespace(std::string & val);
  std::string getIndent(int indentlevel);

  std::string getBaseSectionName(std::string sectionName);
  std::string getSubSectionName(std::string sectionName);

  std::string resolveVariable(std::string searchval, std::string searchVariable);
  std::list<ConfigSection *> m_sectionList;
  std::list<ConfigSection *>::const_iterator m_sectionListIter;

  std::map<std::string, std::string> m_valueMap;
  std::map<std::string, std::string>::const_iterator m_valueMapIter;

  std::string m_sectionName;
  ConfigSection * m_parent;

  // ConfigFile can load the config file without exposing the
  // entire interface to underling application
friend class ConfigFile;
};


#endif
