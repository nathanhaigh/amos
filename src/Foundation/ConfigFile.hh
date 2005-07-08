/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

//! @file
//! @brief ConfigFile class interface

#ifndef CONFIGFILE_HH
#define CONFIGFILE_HH 1

#include <iostream>
#include <fstream>
#include <cstdio>        // for sprintf

#include "ConfigSection.hh"
#include "Exceptions.hh"

//! Class to load and manage configuration files
/*! Loads and manages a root section and subsections from a given filename.
 *  @see ConfigSection
 */
class ConfigFile
{
public:
  ConfigFile(const std::string & filename);
  ~ConfigFile();

  void printConfiguration();

  ConfigSection * findSection(std::string sectionName);
  ConfigSection * getRootSection();

private:
  void parseConfig();

  ConfigSection * m_rootSection;

  std::string m_filename;
  std::ifstream m_configFile;

  int m_linenum;
};

#endif
