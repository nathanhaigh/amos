/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

//! @file 
//! @brief Options class implementation

/** @example example-options.cc
 *  Demonstrates commmand line options handling.\n\n
 *  Notice how the results of the command line option handling can be found
 *  automatically without explicitly asking if an option was specified, or
 *  option data can be queried by name. If automatic results are requested,
 *  the options handler will convert the raw character array from argv into the
 *  output type (int, float, or string). Automatic results also allow for 
 *  default values to be automatically updated if corresponding options are 
 *  provided on the command line.
 */

/** @example example-options.out
 *  Sample output from example-options.\n\n
 *  In each case, the commmand to get the option precedes the output. Data
 *  written to stderr is prefixed with (stderr).
 */

#include "Options.hh"
#include <cstring>
#include <cstdio>

//! Constructor takes command line options in standard argc, argv format
/*! Initializes application name, filespec, and invocation
 */
Options::Options(int argc, char ** argv)
{
  m_argc = argc;
  m_argv = argv;
  
  m_standardOptionsComplete = false;

  m_applicationFilespec = argv[0];

  // Strip leading directories for application name
  char * lastpath = strrchr(argv[0], '/');
  m_applicationName = lastpath ? (char *)(lastpath+1) : argv[0];
 
  // Join together argv for invocation  
  m_invocation = argv[0];
  for (int i=1; i<argc; i++)
  {
    m_invocation += " " + (std::string)argv[i];
  }

  m_optionsParsed = false;
}


//! Deletes OptionResults
/*! Loops through standard option names and other other option names, because
 *  m_optionResults will have multiple names and aliases pointing to the 
 *  same result
 */
Options::~Options()
{
  StringSetCIter i;

  // delete results of all the standard option names

  for (i  = m_standardOptionNames.begin();
       i != m_standardOptionNames.end();
       i++)
  {
    OptionResult * o = m_optionResults[*i];
    if (o)
    {
      delete o;
    }
  }

  // delete results of all the other option names
  for (i  = m_optionNames.begin();
       i != m_optionNames.end();
       i++)
  {
    OptionResult * o = m_optionResults[*i];
    if (o)
    {
      delete o;
    }
  }
}

//! Returns the application filespec (argv[0])
std::string Options::getApplicationFilespec()
{
  return m_applicationFilespec;
}

//! Returns application name
std::string Options::getApplicationName()
{
  return m_applicationName;
}

//! Returns invocation (application filespec with all arguments appended)
std::string Options::getInvocation()
{
  return m_invocation;
}

//! Verifies an optionName isnt null, and hasn't already been added
/*!
 *  @param alias Option name or alias to add
 *
 *  @throws InvalidOptionError If options have already been parsed
 *  @throws InvalidOptionError If alias is null
 *  @throws InvalidOptionError If alias has already been added
 */
void Options::verifyName(const std::string & alias)
{
  if (m_optionsParsed)
  {
    throw InvalidOptionError((std::string) "Can't add an option after the"
                             " options have been parsed",
                             "verifyName");
  }

  if (alias.length() == 0)
  {
    throw InvalidOptionError("Can't add Null Option", "verifyName");
  }

  if (m_optionResults.find(alias) != m_optionResults.end())
  {
    throw InvalidOptionError((std::string)"Option " + alias 
                             + "has already been added",
                             "verifyName");
  }
}

//! Simple Interface for adding options (without setting result)
/*! Builds an option result with no result pointer out of optionName and
 *  alias. Then inserts it into the map.
 *
 *  @param optionName Name of option to add
 *  @param has_arg Flag if option takes an option in getopt_long style
 *  @param alias Optional Additional name of argument 
 *                   (for specifing short and long names)
 *  @param optionHelp Help string for this option
 *  @see insertOptionResult()
 *  @see verifyName()
 */
void Options::addOption(const char * optionName, int has_arg,
                        const char * alias, 
                        const std::string & optionHelp)
{
  verifyName(optionName);

  StringOptionResult * result = new StringOptionResult(optionHelp, NULL);

  result->setHas_arg(has_arg);
  result->addNameOrAlias(optionName, false);
  
  if (alias)
  {
    verifyName(alias);
    result->addNameOrAlias(alias, false);
  }

  insertOptionResult(result);
}

//! Adds a short option 
/*! Internal Function to insert short option
 *  @param shortOptions Short Option to add
 *  @see insertLongOption()
 *  @see insertOptionResult()
 */
void Options::insertShortOption(const std::string & shortOption, int has_arg)
{
  m_shortOptions += shortOption;

  if (has_arg == 1)
  {
    m_shortOptions += ":";
  }
  else if (has_arg == 2)
  {
    m_shortOptions += "::";
  }
}

//! Adds a long option to the long option list
/*! Internal function to add a long option
 *
 *  @param longOptionName Name of option (without leading --)
 *  @param has_arg Specifies if argument always has an argument
 */
void Options::insertLongOption(const std::string & longOptionName, int has_arg)
{
  m_longOptions[longOptionName] = has_arg;
}

//! Internal Function to flatten map of long options into an array
/*! Be sure to free array with deleteLongOptionsArr
 */
struct option * Options::getLongOptionsArr()
{
  int numoptions = m_longOptions.size() + 1;
  struct option * retval = new struct option [numoptions];

  LongOptionsMapCIter iter;
  int i = 0;

  for (iter =  m_longOptions.begin();
       iter != m_longOptions.end();
       iter++)
  {
    // Insert each long option into the array

    char * optionName = new char [iter->first.length() + 1];
    strcpy(optionName, iter->first.c_str());

    retval[i].name = optionName;
    retval[i].has_arg = iter->second;
    retval[i].flag = 0;
    retval[i].val = 0;

    i++;
  }

  // Set the last element to be {0,0,0,0} as per getopt_long API
  retval[i].name = NULL;
  retval[i].has_arg = 0;
  retval[i].flag = 0;
  retval[i].val = 0;

  return retval;
}

//! Deletes the name of each long argument, and then the entire array of
//  long options
void Options::deleteLongOptionsArr(struct option * longOptionsArr)
{
  int i = 0;
  while (longOptionsArr[i].name != NULL)
  {
    delete [] longOptionsArr[i].name;
    i++;
  }

  delete [] longOptionsArr;
}



//! Wraps GNU getopt_long
/*! Wraps calls to getopt_long and stores results in a map for easy access.
 *  The rest of the class aids this function either by setting up parameters
 *  or processing results.
 *  @exception InvalidOptionError On option on commandline but not specified 
 *             as valid
 *  @exception MissingRequiredParameterError On option given but required 
 *             parameter not found
 */
void Options::parseOptions()
{
  if (!m_optionsParsed)
  {
    m_optionsParsed = true;
    
    // Don't allow getopt_long to print its own error messages
    opterr = 0;

    // Flatten long options into an array
    struct option * longOptionsArr = getLongOptionsArr();

    while (1)
    {
      std::string optionName;
      std::string optionValue;

      int option_index = 0;
      int c = getopt_long (m_argc, 
                           m_argv, 
                           m_shortOptions.c_str(), 
                           longOptionsArr, 
                           &option_index);

      if (c == EOF) 
      {
        break;
      }
      else if (c == '?')
      {
        if (optind > 1) optind -= 1;

        throw InvalidOptionError((std::string)"Option <" + m_argv[optind] + ">");

      }
      else if (c == ':')
      {
        throw MissingRequiredParameterError((std::string)"Option <" + (char)optopt 
                                            + ">");
      }
      else if (c == 0)
      {
        // Long Option
        optionName = longOptionsArr[option_index].name;
      }
      else
      {
        // Short Option
        optionName = c;
      }

      if (optarg)
      {
        optionValue = optarg;
      }
      else
      {
        optionValue = "";
      }

      OptionResult * result = m_optionResults[optionName];

      if (result)
      {
        result->setVal(optionName, optionValue);
      }
      else
      {
        throw amosException("Internal Error: Result for non-specified option",
                            "parseOptions");

      }
    }

    // Store away arguments that were not options
    while (optind < m_argc)
    {
      m_otherOptionData.push_back(m_argv[optind++]);
    }

    // Initialize iterators 
    m_optionIterator = m_optionResults.begin();
    m_otherOptionDataIterator = m_otherOptionData.begin();
    
    // Delete the long options array
    deleteLongOptionsArr(longOptionsArr);
  }
}

//! Checks to see if an option is contained in options map
/*! 
 *  @param optionName Name (or alias) or option to check
 *  @return An integer count of the number of times the option was set
 */
int Options::hasOption(const std::string & optionName)
{
  if (!m_optionsParsed) parseOptions();

  int retval = 0;
  OptionsMapCIter i = m_optionResults.find(optionName);
  if (i != m_optionResults.end())
  {
    retval = i->second->resultSet(optionName);
  }

  return retval;
}

  
//! Gets the argument value for an option
/*! Note: Returns "" for options not found which is identical to options
 *  that are defined but have no value. Use hasOption first to determine
 *  if option exists. 
 *
 *  @return Value of option or "" if not found or valueless
 */
std::string Options::getOption(const std::string & optionName)
{
  std::string retval;

  if (!m_optionsParsed) parseOptions();

  if (OptionResult * result = m_optionResults[optionName])
  {
    retval = result->getRawValue();
  }

  return retval;
}

//! Iterates through the list of arguments getting one argument name at a time
std::string Options::getNextOption()
{
  if (!m_optionsParsed) parseOptions();

  std::string retval;

  if (m_optionIterator != m_optionResults.end())
  {
    retval = m_optionIterator->first;
    m_optionIterator++;
  }

  return retval;
}

//! Gets all non-option data from the command line
StringList Options::getAllOtherData()
{
  if (!m_optionsParsed) parseOptions();

  return m_otherOptionData;
}

//! Iterates through non-option data
std::string Options::getNextOtherData()
{
  if (!m_optionsParsed) parseOptions();

  std::string retval;

  if (m_otherOptionDataIterator != m_otherOptionData.end())
  {
    retval = *m_otherOptionDataIterator;
    m_otherOptionDataIterator++;
  }

  return retval;
}

//! Call after all of the standard options have been set
void Options::standardOptionsComplete()
{
  m_standardOptionsComplete = true;
}

//! Inserts an option into the optionData Map and long and short lists
/*!
 *  Note: For all option adding functions,\n
 *  has_arg = 0 means option takes no argument\n
 *  has_arg = 1 means options always takes an argument\n
 *  has_arg = 2 means options optionally takes an argument
 *
 *  @see insertShortOption()
 *  @see insertLongOption()
 */
void Options::insertOptionResult(OptionResult * optionResult)
{
  std::string aliasName;
  int has_arg = optionResult->has_arg();

  if (!m_standardOptionsComplete)
  {
    m_standardOptionNames.insert(optionResult->getName());
  }
  else
  {
    m_optionNames.insert(optionResult->getName());
  }

  while ((aliasName = optionResult->getNextAlias()) != "")
  {
    // Insert into main result map
    m_optionResults[aliasName] = optionResult;

    // Add the option to the long and short option lists
    int aliasHasArg = !optionResult->isNegation(aliasName) && has_arg;

    if (aliasName.length() == 1)
    {
      insertShortOption(aliasName, aliasHasArg);
    }
    else
    {
      insertLongOption(aliasName, aliasHasArg);
    }
  }

  optionResult->resetAliasIter();
}

//! Sets up an option result with an argument string in perl style
/*! Parses optionArgument, updates optionResult, and inserts optionResult\n\n
 *  optionArgument format is: name[|alias[...]][!][{=,:}{s,f,i}]\n
 *  name:        Main name for option, only required argument\n
 *  |alias[...]: The list of aliases for the option separated by '|'\n
 *  ! Creates negative options as well, ie. "option" and "nooption"\n
 *  = means requires an argument\n
 *  : means argument is optional\n
 *  s means result type is string\n
 *  f means result type is float\n
 *  i means result type is integer\n
 *
 *  if result type is integer 0 is set for negative option,\n
 *                            1 is set for positive option and no value\n\n
 *
 *  if result type is float 0.0 is set for negative option,\n
 *                          1.0 is set for positive option and no value\n\n
 *
 *  if result type is string "" is set for negative option,\n
 *                           "1" is set for positive option and no value\n
 *
 *  @see verifyName()
 *  @see addOptionResult()
 */
void Options::parseArgument(const std::string & optionArgument,
                            OptionResult * optionResult)
{
  bool hasNegation = false;
  char optionType = 'i';
  int has_arg = 0;

  std::string::size_type index, splitIndex;
  std::string::size_type endNameIndex = optionArgument.length();

  // Check to see if option takes an argument
  if ((index = optionArgument.find_first_of(":=")) != optionArgument.npos)
  {
    if (index == 0)
    {
      throw InvalidOptionError("First character can't be : or = in argument",
                               "Options::parseArgument");
    }

    // option takes an argument
    has_arg = 1;

    if (optionArgument[index] == ':')
    {
      // optional argument
      has_arg = 2;
    }

    if (index == optionArgument.length())
    {
      throw InvalidOptionError("No type specified for argument in option",
                               "Options::parseArgument");
    }

    optionType = optionArgument[index+1];
    endNameIndex = index - 1;

    optionResult->setHas_arg(has_arg);
  }


  // Check for negation
  if ((index = optionArgument.find("!")) != optionArgument.npos)
  {
    if (index == 0)
    {
      throw InvalidOptionError("First character can't be ! in argument",
                               "Options::parseArgument");
    }

    hasNegation = true;
    if ((index - 1) < endNameIndex)
    {
      endNameIndex = index - 1;
    }
  }


  // At this point optionArgument[0]..optionArgument[endNameIndex] is
  // the name and list of aliases
  // Roughly this would be in perl
  // foreach $name in split ("|", $optionArgument[0..endNameIndex])
  // {
  //   verifyName($name);
  //   addNameOrAlias($name, $hasNegation);
  // }
  
  std::string optionAlias;

  index = 0;
  while ((splitIndex = optionArgument.find('|', index)) != optionArgument.npos)
  {
    // a name or alias is between index and splitindex - 1
    optionAlias = optionArgument.substr(index, splitIndex - index);

    verifyName(optionAlias);
    optionResult->addNameOrAlias(optionAlias, hasNegation);

    index = splitIndex + 1;
  }

  optionAlias = optionArgument.substr(index, endNameIndex - index + 1),

  verifyName(optionAlias);
  optionResult->addNameOrAlias(optionAlias, hasNegation);

  // Call verifyType after name has been set so there can be a meaningful
  // exception
  optionResult->verifyType(optionType);

  // optionResult is now ready to be added to main list of options
  insertOptionResult(optionResult);
}


//! Creates an IntOptionResult, and calls parseArgument with it
/*!
 *  optionArgument format is: name[|alias[...]][!][{=,:}{s,f,i}]\n
 *  name:        Main name for option, only required argument\n
 *  |alias[...]: The list of aliases for the option separated by '|'\n
 *  ! Creates negative options as well, ie. "option" and "nooption"\n
 *  = means requires an argument\n
 *  : means argument is optional\n
 *  s means result type is string\n
 *  f means result type is float\n
 *  i means result type is integer\n
 *
 *  if result type is integer 0 is set for negative option,\n
 *                            1 is set for positive option and no value\n\n
 *
 *  if result type is float 0.0 is set for negative option,\n
 *                          1.0 is set for positive option and no value\n\n
 *
 *  if result type is string "" is set for negative option,\n
 *                           "1" is set for positive option and no value\n
 *  @exception InvalidOptionError If optionArgument is NULL
 *
 *  @param optionArgument Perl style option string
 *  @param resultPointer Pointer to int where result should be written
 *  @param optionHelp Help string for the option
 */
void Options::addOptionResult(const std::string & optionArgument,
                              int * resultPointer,
                              const std::string & optionHelp)
{
  if (optionArgument.length())
  {
    IntOptionResult * optionResult = new IntOptionResult(optionHelp,
                                                         resultPointer);
    parseArgument(optionArgument, optionResult);
  }
  else
  {
    throw InvalidOptionError("Passed a null optionArgument", 
                             "addOptionResult");
  }
}

//! Creates an FloatOptionResult, and calls parseArgument with it
/*!
 *  @see addOptionResult(const char [], int *, const string &)
 *  @exception InvalidOptionError If optionArgument is NULL
 *
 *  @param optionArgument Perl style option string
 *  @param resultPointer Pointer to float where result should be written
 *  @param optionHelp Help string for the option
 */
void Options::addOptionResult(const std::string & optionArgument,
                              float * resultPointer,
                              const std::string & optionHelp)
{
  if (optionArgument.length())
  {
    FloatOptionResult * optionResult = new FloatOptionResult(optionHelp,
                                                             resultPointer);
    parseArgument(optionArgument, optionResult);
  }
  else
  {
    throw InvalidOptionError("Passed a null optionArgument", 
                             "addOptionResult");
  }
}

//! Creates an StringOptionResult, and calls parseArgument with it
/*!
 *  @see addOptionResult(const char [], int *, const string &)
 *  @exception InvalidOptionError If optionArgument is NULL
 *
 *  @param optionArgument Perl style option string
 *  @param resultPointer Pointer to float where result should be written
 *  @param optionHelp Help string for the option
 */
void Options::addOptionResult(const std::string & optionArgument,
                              std::string * resultPointer,
                              const std::string & optionHelp)
{
  if (optionArgument.length())
  {
    StringOptionResult * optionResult = new StringOptionResult(optionHelp,
                                                               resultPointer);
    parseArgument((std::string) optionArgument, optionResult);
  }
  else
  {
    throw InvalidOptionError("Passed a null optionArgument", 
                             "addOptionResult");
  }
}

//! Print help for each alias alphabetically, starting with the standard options
void Options::printHelp()
{
  StringSetCIter i;
  OptionResult * o;

  // Loop through standard options
  for (i  = m_standardOptionNames.begin();
       i != m_standardOptionNames.end();
       i++)
  {
    o = m_optionResults[*i];

    if (o)
    {
      o->printHelp(true);
    }
  }
  
  std::cerr << std::endl;

  // Loop through other options
  for (i  = m_optionNames.begin();
       i != m_optionNames.end();
       i++)
  {
    o = m_optionResults[*i];

    if (o)
    {
      o->printHelp(false);
    }
  }
}

