/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

//! @file 
//! @brief OptionResult class implementation

#include "OptionResult.hh"
#include <cstdlib>

//! Sets defaults for all results
OptionResult::OptionResult(const std::string & optionHelp)
{ 
  m_has_arg = 0;
  m_resultSet = 0;
  m_negationSet = 0;

  m_optionHelp = optionHelp;
}

//! Empty Deconstructor
OptionResult::~OptionResult()
{ }

//! Gets the raw value that was set
std::string OptionResult::getRawValue() const
{
  return m_rawval;
}

//! Adds a name or alias to a result class
/*! The first time this is called, it adds it as its name. If it has a
 *  negation, then it adds "no"name as well.
 *
 *  Also resets the iterator for getNextAlias()
 *  
 *  @param name
 *  @param hasNegation
 */
void OptionResult::addNameOrAlias(const std::string & name, bool hasNegation)
{
  if (m_name == "")
  {
    m_name = name;
  }

  // Insert this name into the map as not being a negation
  m_aliases[name] = false;

  // If it has a negation, add it into the map as well as a negation
  if (hasNegation)
  {
    m_aliases["no" + name] = true;
  }

  resetAliasIter();
}

//! Returns if a given name is a negation
bool OptionResult::isNegation(const std::string & name)
{
  return m_aliases[name];
}

//! Verifies that the type of the class matches the requested type
/*!
 *  @exception InvalidOptionError If there is a type mismatch
 */
void OptionResult::verifyType(char optionType)
{
  if (m_type != optionType)
  {
    throw InvalidOptionError((std::string) "Option Type mismatch: Asked for " 
                             + optionType 
                             + (std::string)" but gave pointer to " 
                             + m_type + " in " + m_name, 
                             "Options");
  }
}

//! Gets the next alias from map using the class iterator
std::string OptionResult::getNextAlias()
{
  std::string retval;

  if (m_aliasesIter != m_aliases.end())
  {
    retval = m_aliasesIter->first;
    m_aliasesIter++;
  }

  return retval;
}

//! Returns the main name of the object
std::string OptionResult::getName() const
{
  return m_name;
}

//! Resets the object iterator for getNextAlias
void OptionResult::resetAliasIter()
{
  m_aliasesIter = m_aliases.begin();
}

//! Simple getter to check if option takes an argument in positive form
/*! Uses same syntax as getopt
 *  @see setHas_arg
 */
int OptionResult::has_arg()
{
  return m_has_arg;
}

//! Simple setter for if option takes an argument in positive form
/*! Uses same syntax as getopt
 *  @see has_arg
 */
void OptionResult::setHas_arg(int has_arg)
{
  m_has_arg = has_arg;
}

//! Increments the proper counter for the name
void OptionResult::incrementCount(const std::string & name)
{
  if (isNegation(name))
  {
    m_negationSet++;
  }
  else
  {
    m_resultSet++;
  }
}

//! Validates that the option name should or should not take an argument
/*!
 *  @exception InvalidOptionError If option was given an argument but takes none
 *  @exception InvalidOptionError If option wasnt given a required argument
 */
void OptionResult::validateArgumentCount(const std::string & name,
                                         const std::string & val)
{
  if (val.length())
  {
    // Validate that it should have an argument
    if (!m_has_arg || isNegation(name))
    {
      throw InvalidOptionError("Argument given to " + name + " but takes none",
                               "validateArgumentCount");
    }
  }
  else
  {
    // Validate that it shouldnt have an argument
    if (m_has_arg == 1)
    {
      throw InvalidOptionError("No Argument given to " + name 
                               + " but requires one",
                               "validateArgumentCount");
    }
  }
}










//! Returns the number of times an option was set
/*! Note: Separate counters are maintained for positive or negative options,
 *  so caller may need to call resultSet("option") and resultSet("nooption")
 *  to find out what was really specified.
 */
int OptionResult::resultSet(const std::string & name)
{
  int retval = 0;

  if (m_aliases[name])
  {
    retval = m_negationSet;
  }
  else
  {
    retval = m_resultSet;
  }
  
  return retval;
}

//! Iterates through each alias and prints the option help
/*! 
 *  @param isStandardOption Standard options should not print their defaults
 */
void OptionResult::printHelp(bool isStandardOption)
{
  std::map<std::string, bool>::const_iterator i;

  // Loop through all of the aliases to find the longest name
  int maxlen = 0;
  for (i =  m_aliases.begin();
       i != m_aliases.end();
       i++)
  {
    int thislen = i->first.length();
    if (thislen > 1) thislen++;

    // If this is a negative name add strlen (" Disables")
    if (i->second) thislen += 9;

    if (thislen > maxlen) maxlen = thislen;
  }

  // print each alias and information
  for (i =  m_aliases.begin();
       i != m_aliases.end();
       i++)
  {
    std::cerr << "  -";

    int thislen = i->first.length();
    if (thislen > 1)
    {
      // long option
      std::cerr << "-";
      thislen++;
    }

    if (i->second) thislen += 9;

    std::cerr << i->first;

    // add spaces for alignment
    for (; thislen < maxlen; thislen++)
    {
      std::cerr << " ";
    }

    if (i->second)
    {
      std::cerr << " Disables";
    }
    else if (m_has_arg == 1)
    {
      std::cerr << " <argument>";
    }
    else if (m_has_arg == 2)
    {
      std::cerr << "[=<argument>]";
    }

    std::cerr << " " << m_optionHelp << std::endl;
  }

  if (m_has_arg && !isStandardOption)
  {
    writeDefault();
  }
}










//! Constructor sets defaults for integer class, and constructs base class
IntOptionResult::IntOptionResult(const std::string & optionHelp,
                                 int * result)
  : OptionResult(optionHelp)
{
  m_result = result;
  m_type = 'i';
}

//! Empty deconstructor
IntOptionResult::~IntOptionResult()
{ }

//! If a result pointer is set, prints out its value
/*! Note: This is identical in the concrete classes, because the base
 *  class has no concept of a result pointer
 */
void IntOptionResult::writeDefault()
{
  if (m_result)
  {
    std::cerr << "       (default=" << *m_result << ")" << std::endl;
  }
}

//! Converts a string value into an integer value, and writes it to the result
/*! Increments the counter for the result being set, verifies value, saves
 *  it as the raw value, and writes the integer form to the result pointer if
 *  it was set.
 *
 *  @exception InvalidOptionError If non-numeric data is in value
 *  @see validateArgumentCount
 */
void IntOptionResult::setVal(const std::string & name,
                             const std::string & val)
{
  int retval = 0;
  m_rawval = val;

  incrementCount(name);

  validateArgumentCount(name, val);

  if (val.length())
  {
    // Validate that string is only integer characters
    if (val.find_first_not_of("0123456789-") != val.npos)
    {
      throw InvalidOptionError("Non-numeric value in " + val + " for " + name,
                               "IntOptionResult::setVal");
    }
    
    retval = atoi(val.c_str());
  }
  else
  {
    // if this name is a negation set result to 0, otherwise 1
    retval = !isNegation(name);
  }

  if (m_result)
  {
    *m_result = retval;
  }
}




//! Constructor sets defaults for float class, constructs baseclass
FloatOptionResult::FloatOptionResult(const std::string & optionHelp,
                                     float * result)
  : OptionResult(optionHelp)
{
  m_result = result;
  m_type = 'f';
}

//! Empty deconstructor
FloatOptionResult::~FloatOptionResult()
{ }

//! If a result pointer is set, prints out its value
/*! Note: This is identical in the concrete classes, because the base
 *  class has no concept of a result pointer
 */
void FloatOptionResult::writeDefault()
{
  if (m_result)
  {
    std::cerr << "       (default=" << *m_result << ")" << std::endl;
  }
}

//! Converts a string value into an float value, and writes it to the result
/*! Increments the counter for the result being set, verifies value, saves
 *  it as the raw value, and writes the float form to the result pointer if
 *  it was set.
 *
 *  @see validateArgumentCount
 *  @exception InvalidOptionError If non-float data is in value
 */
void FloatOptionResult::setVal(const std::string & name,
                               const std::string & val)
{
  float retval = 0;
  m_rawval = val;

  incrementCount(name);

  validateArgumentCount(name, val);

  if (val.length())
  {
    // Validate that string is only float characters
    if (val.find_first_not_of("0123456789-.") != val.npos)
    {
      throw InvalidOptionError("Non-float value in " + val + " for " + name,
                               "FloatOptionResult::setVal");
    }
    
    // Convert string to float
    retval = atof(val.c_str());
  }
  else
  {
    // if this name is a negation set result to 0, otherwise 1
    retval = !isNegation(name);
  }

  if (m_result)
  {
    *m_result = retval;
  }
}





//! Constructor sets defaults for string class, constructs base class
StringOptionResult::StringOptionResult(const std::string & optionHelp,
                                       std::string * result)
  : OptionResult(optionHelp)
{
  m_result = result;
  m_type = 's';
}

//! Empty deconstructor
StringOptionResult::~StringOptionResult()
{ }

//! If a result pointer is set, prints out its value
/*! Note: This is identical in the concrete classes, because the base
 *  class has no concept of a result pointer
 */
void StringOptionResult::writeDefault()
{
  if (m_result && m_result->length())
  {
    std::cerr << "       (default=" << *m_result << ")" << std::endl;
  }
}

//! Writes the value to the result pointer
/*! Saves a copy of the value as the raw value and writes the value unchanged
 *  to the result pointer. 
 *
 *  @see validateArgumentCount
 */
void StringOptionResult::setVal(const std::string & name,
                                const std::string & val)
{
  std::string retval = "";
  m_rawval = val;

  incrementCount(name);

  validateArgumentCount(name, val);

  if (val.length())
  {
    retval = val;
  }
  else
  {
    // if this name is a negation set result to "", otherwise "1"
    if (isNegation(""))
    {
      retval = "";
    }
    else
    {
      retval = "1";
    }
  }
  
  // Save in result
  if (m_result)
  {
    *m_result = retval;
  }
}

