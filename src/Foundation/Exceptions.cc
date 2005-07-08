/*
 * Copyright (c) 2003, The Institute for Genomic Research (amos).  All
 * rights reserved.
 */

// $Id$

//! @file 
//! @brief Implements all exception classes

#include "Exceptions.hh"

amosException::amosException(std::string msg, std::string module)
{
  m_errorMsg = "Error: " + msg;
  m_module = module;
}

//! Gets the error message 
std::string amosException::getErrorMsg() const
{
  return m_errorMsg;
}

//! Gets the module name
std::string amosException::getModule() const
{
  return m_module;
}

//! Sets the error message
void amosException::setRawErrorMsg(std::string msg)
{
  m_errorMsg = msg;
}

void amosException::setModule(std::string module)
{
  m_module = module;
}

//! Static method to enclose a string in quotes "..."
std::string amosException::quote(std::string msg)
{
  return (std::string) "\"" +
         (std::string) msg  + 
         (std::string) "\"";
}

//! Easy way to print exception error messages
std::ostream & operator<< (std::ostream & os, const amosException & exception)
{
  os << exception.getErrorMsg()
     << " in module "
     << exception.getModule();

  return os;
}

////////////////////////////////////////////////////////////////////////

FileIOError::FileIOError(std::string msg, std::string module)
  : amosException("IO Error: " + msg, module)
{ }

UnableToReadError::UnableToReadError(std::string msg, std::string module)
  : FileIOError("Unable to read: " + msg, module)
{ }

UnableToOpenError::UnableToOpenError(std::string msg, std::string module)
  : FileIOError("Unable to open: " + msg, module)
{ }

////////////////////////////////////////////////////////////////////////

OptionsError::OptionsError(std::string msg, std::string module)
  : amosException("Options: " + msg, module)
{ }

ExitProgramNormally::ExitProgramNormally(std::string msg, std::string module)
  : OptionsError(msg, module)
{ }

InvalidOptionError::InvalidOptionError(std::string msg, std::string module)
  : OptionsError("Invalid Option: " + msg, module)
{ }

MissingRequiredParameterError::MissingRequiredParameterError(std::string msg,
                                                             std::string module)
  : OptionsError("Missing Required Parameter: " + msg, module)
{ }

////////////////////////////////////////////////////////////////////////

ConfigFileError::ConfigFileError(std::string msg, std::string module)
  : amosException("ConfigFile: " + msg, module)
{ }

CircularReferenceError::CircularReferenceError(std::string msg, std::string module)
  : ConfigFileError("Circular Reference: " + msg, module)
{ }

DuplicateSectionError::DuplicateSectionError(std::string msg, std::string module)
  : ConfigFileError("Duplicate Section Found: " + msg, module)
{ }

ConfigFileLineTooLong::ConfigFileLineTooLong(std::string msg, std::string module)
  : ConfigFileError("Line Too Long To Read: " + msg, module)
{ }

MalformedSubstitutionError::MalformedSubstitutionError(std::string msg, 
                                                       std::string module)
  : ConfigFileError("Malformed Substitution: " + msg, module)
{ }

UnresolvedSubstitutionError::UnresolvedSubstitutionError(std::string msg, 
                                                         std::string module)
  : ConfigFileError("Unresolved Substitution: " + msg, module)
{ }

InvalidConfigNameError::InvalidConfigNameError(std::string msg, std::string module)
  : ConfigFileError("Invalid Name: " + msg, module)
{ }

InvalidConfiguration::InvalidConfiguration(std::string msg, std::string module)
  : ConfigFileError("Invalid Configuration: " + msg, module)
{ }
