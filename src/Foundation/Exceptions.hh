/*
 * Copyright (c) 2003, The Institute for Genomic Research (amos).  All
 * rights reserved.
 */

// $Id$

//! @file
//! @brief Interface for all exceptions

#ifndef EXCEPTIONS_HH
#define EXCEPTIONS_HH 1

#include <string>
#include <iostream>

//! Base class for all amos Exceptions
class amosException
{
public:
  amosException(std::string msg, std::string module);

  std::string getErrorMsg() const;
  std::string getModule() const;
  static std::string quote(std::string msg);

  void setRawErrorMsg(std::string msg);
  void setModule(std::string module);

private:
  std::string m_errorMsg;
  std::string m_module;
};

std::ostream & operator<< (std::ostream & os, const amosException & exception);

////////////////////////////////////////////////////////////////////////

//! File IO Errors
class FileIOError : public amosException
{
public:
  FileIOError(std::string msg, std::string module);
};

//! Can't read data from a file
class UnableToReadError : public FileIOError
{
public:
  UnableToReadError(std::string msg, std::string module);
};

//! Can't open a file
class UnableToOpenError : public FileIOError
{
public:
  UnableToOpenError(std::string msg, std::string module);
};

////////////////////////////////////////////////////////////////////////

//! Base class for errors with options
class OptionsError : public amosException
{
public:
  OptionsError(std::string msg, std::string module="Options");
};

//! Program should exit(0)
class ExitProgramNormally : public OptionsError
{
public:
  ExitProgramNormally(std::string msg = "", std::string module = "");
};

//! Invalid Option (not specified as acceptable)
class InvalidOptionError : public OptionsError
{
public:
  InvalidOptionError(std::string msg, std::string module="Options");
};

//! An option requires a value, but none was given
class MissingRequiredParameterError : public OptionsError
{
public:
  MissingRequiredParameterError(std::string msg, std::string module="Options");
};

////////////////////////////////////////////////////////////////////////

//! Base class for errors in configuration files
class ConfigFileError : public amosException
{
public:
  ConfigFileError(std::string msg, std::string module="ConfigFile");

};

//! Errors where there are two sections by the same name
class DuplicateSectionError : public ConfigFileError
{
public:
  DuplicateSectionError(std::string msg, std::string module="ConfigFile");
};

//! Errors where a line in a config file is too long to read
class ConfigFileLineTooLong : public ConfigFileError
{
public:
  ConfigFileLineTooLong(std::string msg, std::string module="ConfigFile");
};

//! Errors where variable substitution references itself
class CircularReferenceError : public ConfigFileError
{
public:
  CircularReferenceError(std::string msg, std::string module="ConfigSection");
};


//! Errors where substitution is malformed
class MalformedSubstitutionError : public ConfigFileError
{
public:
  MalformedSubstitutionError(std::string msg, std::string module="ConfigSection");
};


//! Errors where a substitution can't be processed
class UnresolvedSubstitutionError : public ConfigFileError
{
public:
  UnresolvedSubstitutionError(std::string msg, std::string module="ConfigSection");
};

//! Some other error in how something is named
class InvalidConfigNameError : public ConfigFileError
{
public:
  InvalidConfigNameError(std::string msg, std::string module="ConfigSection");
};

//! Illegal Configuration
class InvalidConfiguration : public ConfigFileError
{
public:
  InvalidConfiguration(std::string msg, std::string module="ConfigSection");
};

#endif
