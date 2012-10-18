/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

//! @file 
//! @brief OptionResult class interface

#ifndef OPTIONRESULT_HH
#define OPTIONRESULT_HH 1

#include <string>
#include <map>
#include "Exceptions.hh"

//! Abstract Base class for managing setting results of option data
/*! Managed and created by the Options class
 */
class OptionResult
{
public:
  OptionResult(const std::string & optionHelp);
  virtual ~OptionResult();

  void addNameOrAlias(const std::string & name, bool hasNegation);
  void verifyType(char optionType);

  std::string getName() const;
  std::string getNextAlias();

  void resetAliasIter();
  bool isNegation(const std::string & name);

  int has_arg();
  void setHas_arg(int has_arg);

  //! Pure virtual function for converting a value into its resultant type
  virtual void setVal(const std::string & name, 
                      const std::string & val) = 0;

  int resultSet(const std::string & name);
  std::string getRawValue() const;

  void printHelp(bool isStandardOption);
  //! Pure virtual function for write the value of the resultpointer
  virtual void writeDefault() = 0;

protected:
  void incrementCount(const std::string & name);
  void validateArgumentCount(const std::string & name,
                             const std::string & val);
  //! Type of resultant
  char m_type;
  //! Raw value for option from command line
  std::string m_rawval;

private:
  //! If it takes an argument or is optional
  int m_has_arg;
  //! Main name of option
  std::string m_name;

  //! Help for the option
  std::string m_optionHelp;

  //! Counter for number of times the (positive) result was set
  int m_resultSet;
  //! Counter for number of times the negative result was set
  int m_negationSet;

  //! Maps aliases (including name) to if they are positive or negative
  std::map<std::string, bool> m_aliases;
  //! Iterator for aliasList
  std::map<std::string, bool>::const_iterator m_aliasesIter;
};



//! Class for managing results of Integers
class IntOptionResult : public OptionResult
{
public:
  IntOptionResult(const std::string & optionHelp,
                  int * result);
  ~IntOptionResult();

  void writeDefault();
  void setVal(const std::string & name,
              const std::string & val);

private:
  int * m_result;
};


//! Class for managing results of Floats
class FloatOptionResult : public OptionResult
{
public:
  FloatOptionResult(const std::string & optionHelp,
                    float * result);
  ~FloatOptionResult();

  void writeDefault();
  void setVal(const std::string & name,
              const std::string & val);

private:
  float * m_result;
};


//! Class for managing results of Floats
class StringOptionResult : public OptionResult
{
public:
  StringOptionResult(const std::string & optionHelp,
                     std::string * result);
  ~StringOptionResult();

  void writeDefault();
  void setVal(const std::string & name,
              const std::string & val);

private:
  std::string * m_result;
};


#endif
