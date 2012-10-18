/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

//! @file 
//! @brief ConfigFile class implementation

/*! @example example-configfile.cc
 *  Demonstrates configuration file handling.\n\n
 *  Various methods are called to pull information out of the configuration
 *  file.
 */

/*! @example example-configfile.ini
 *  An example configuration file.\n\n
 *  Note that sections and variables are supported, as well as allowing for
 *  runtime substitution of variable values. ConfigFile supports multiple
 *  substitutions on the same line, nested substitutions, and the ability to
 *  set default rules. Notice how in this example NIH.workdir, TIGR.workdir
 *  and TIGR.updated.workdir are all set to the same rule, but in the output
 *  the substitutions evaluate differently. This is possible because
 *  while the default rule is written in the topmost section, the
 *  rules are evaluated using the most local values of ORG and {{ORG}}_DIR
 *  relative to the child where the rule is referenced.
 */

/*! @example example-configfile.out
 *  Sample output from example-config.\n\n
 *  Notice how NIH.workdir, TIGR.workdir, and TIGR.update.workdir all have 
 *  different values.
 */

#include "ConfigFile.hh"
#include <cstring>

//! Constructor
/*! Creates a ConfigFile class. Also loads, parses, and resolves
 *  variable substitution.
 *
 *  @param filename Filename of config file to load
 *  @exception UnableToReadError On failing FileSystem::isReadAbleFile
 *  @exception UnableToOpenError On failing ifstream::is_open
 */
ConfigFile::ConfigFile(const std::string & filename)
{
  m_filename = filename;
  m_linenum = 1;

  m_configFile.open(filename.c_str(), std::ios::in);

  if (!m_configFile.is_open())
  {
    throw UnableToOpenError(amosException::quote(filename),
                            "ConfigFile");
  }

  try
  {
    parseConfig();
    m_rootSection->resolveSubstitutions();
  }
  catch (ConfigFileError e)
  {
    m_configFile.close();
    if (m_rootSection) 
    {
      delete m_rootSection;
      m_rootSection = NULL;
    }

    char buffer[16];
    sprintf(buffer, "%d", m_linenum);

    e.setRawErrorMsg(e.getErrorMsg() 
                     + (std::string) " in file " 
                     + amosException::quote(filename)
                     + (std::string) " on line "
                     + buffer);
    throw e;
  }

  m_configFile.close();
}

//! Deconstructor also deletes m_rootSection
ConfigFile::~ConfigFile()
{
  m_configFile.close();
  if (m_rootSection) delete m_rootSection;
}

//! Prints the configuration from the rootSection down
void ConfigFile::printConfiguration()
{
  if (m_rootSection) m_rootSection->printSection();
}

//! Reads the configuration file line by line and extracts data
/*!
 *  @exception InvalidConfigNameError On an empty section name
 */
void ConfigFile::parseConfig()
{
  char buffer [1025];
  m_rootSection = new ConfigSection(NULL, "ROOT");
  ConfigSection * currentSection = m_rootSection;

  while (m_configFile.getline(buffer, sizeof(buffer)))
  {
    // Check to see if there is any data
    int bufferlen = strlen(buffer);
    if (bufferlen)
    {
      if (bufferlen == sizeof(buffer))
      {
        if (buffer[bufferlen-1] != '\n')
        {
          throw ConfigFileLineTooLong(buffer);
        }
      }

      switch(buffer[0])
      {
        case '[':
          {
            std::string currentSectionName = 
                     ConfigSection::parseSectionName(buffer);
            if (currentSectionName.length())
            {
              currentSection = m_rootSection->findSection(currentSectionName);

              if (!currentSection)
              {
                currentSection = m_rootSection->
                                   addNewSection(currentSectionName);
                
              }
              else
              {
                throw DuplicateSectionError(
                             amosException::quote(currentSectionName));

              }
            }
            else
            {
              throw InvalidConfigNameError(amosException::quote(buffer),
                                           "ConfigFile");
            }
            break;
          }

        case ';':
          {
            // Ignore Comments
            break;
          }

        default:
          {
            currentSection->addVariable((const char *)buffer);
          }
      };
    }
    else
    {
      // Blank Line, Ignore
    }

    m_linenum++;
  }
}

//! Finds a section from ROOT section
ConfigSection * ConfigFile::findSection(std::string sectionName)
{
  ConfigSection * retval = NULL;

  if (m_rootSection) retval = m_rootSection->findSection(sectionName);

  return retval;
}

//! Returns ROOT section
ConfigSection * ConfigFile::getRootSection()
{
  return m_rootSection;
}
