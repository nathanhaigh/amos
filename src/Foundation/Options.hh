/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

//! @file 
//! @brief Options class interface

#ifndef OPTIONS_HH
#define OPTIONS_HH 1

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <list>
#include <map>
#include <set>
#include <string>
#include <getopt.h>

#include "Exceptions.hh"
#include "OptionResult.hh"

//! Map type to hold (option, result *) pairs
typedef std::map<std::string, OptionResult *> OptionsMap;

//! Iterator Type for OptionsMap
typedef OptionsMap::iterator OptionsMapIter;

//! Constant Iterator Type for OptionsMap
typedef OptionsMap::const_iterator OptionsMapCIter;


//! Map type to hold (optionName, has_arg) pairs
typedef std::map<std::string, int> LongOptionsMap;

//! Constant Iterator for LongOptionsMap
typedef LongOptionsMap::const_iterator LongOptionsMapCIter;


//! List of strings
typedef std::list<std::string> StringList;
//! Iterator for a list of strings
typedef StringList::const_iterator StringListCIter;

//! Set of strings
typedef std::set<std::string> StringSet;
//! Iterator for a set of strings
typedef StringSet::const_iterator StringSetCIter;

//! Class to simplify results of getopt and getopt_long
class Options
{
public:
  Options(int argc, char ** argv);
  ~Options();

  // Simple "getters"
  std::string getApplicationFilespec();
  std::string getApplicationName();
  std::string getInvocation();

  // Functions for adding options
  void addOption(const char * optionName, int has_arg,
                 const char * alias = NULL,
                 const std::string & optionHelp = "");
  
  // For adding options with results
  void addOptionResult(const std::string & optionArgument,
                       int * resultPointer,
                       const std::string & optionHelp = "");

  void addOptionResult(const std::string & optionArgument,
                       float * resultPointer,
                       const std::string & optionHelp = "");

  void addOptionResult(const std::string & optionArgument,
                       std::string * resultPointer,
                       const std::string & optionHelp = "");

  // For Option results
  int         hasOption(const std::string & optionName);
  std::string getOption(const std::string & optionName);
  std::string getNextOption();

  // Command line arguments that are not options
  std::string getNextOtherData();
  StringList  getAllOtherData();

  // Utility Functions
  void standardOptionsComplete();
  void printHelp();
  void parseOptions();

private:
  void verifyName(const std::string & alias);
  void parseArgument(const std::string & optionArgument,
                     OptionResult * optionResult);

  // Insert options into appropriate lists
  void insertOptionResult(OptionResult * optionResult);

  void insertLongOption(const std::string & longOptionName, int has_arg);
  void insertShortOption(const std::string & shortOptionName, int has_arg);

  // Long Option array Utility
  struct option * getLongOptionsArr();
  void deleteLongOptionsArr(struct option * longOptionsArr);

  // Member vars

  bool m_optionsParsed;
  bool m_standardOptionsComplete;

  int m_argc;
  char ** m_argv;

  std::string m_applicationFilespec;
  std::string m_applicationName;
  std::string m_invocation;

  std::string m_shortOptions;
  LongOptionsMap m_longOptions;

  OptionsMap m_optionResults;
  OptionsMapCIter m_optionIterator;

  StringSet m_optionNames;
  StringSet m_standardOptionNames;

  StringList m_otherOptionData;
  StringListCIter m_otherOptionDataIterator;
};



#endif
