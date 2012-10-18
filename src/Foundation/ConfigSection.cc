/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

//! @file 
//! @brief ConfigSection class implementation

#include "ConfigSection.hh"

//! Constructor
/*! Create new section.
 *  @param parent Pointer to parent section, or NULL
 *  @param sectionName Name of section
 */
ConfigSection::ConfigSection(ConfigSection * parent,
                             std::string sectionName)
{
  m_parent = parent;
  m_sectionName = sectionName;

  // Initialize the iterators to "NULL"
  m_sectionListIter = m_sectionList.end();
  m_valueMapIter = m_valueMap.end();
}

//! Destructor
/*! Deletes all subsections as well. */
ConfigSection::~ConfigSection()
{
  // Delete all of the children
  
  while (m_sectionList.size())
  {
    ConfigSection * tmpSection = m_sectionList.front();
    m_sectionList.pop_front();

    delete tmpSection;
  }
}

//! Simple Accessor to getSectionName
/*! @return Short Name of current Section (no parent information)
 *  @see getFullSectionName()
 */
std::string ConfigSection::getSectionName()
{
  return m_sectionName;
}


//! Adds a section to the list of sections
/*!
 *  @param newSection New section to add
 */
void ConfigSection::addSection(ConfigSection * newSection)
{
  m_sectionList.push_back(newSection);
  m_sectionListIter = m_sectionList.begin(); // Redundant in most cases
}


//! Creates a new section, adds it to the appropriate section lists
/*! sectionName is full name to section as in base.child.child2,
 *  and will automatically create base and base.child if needed.
 *  Trying to add an existing section will return a pointer to that section
 *
 *  @param sectionName Name of the section to add
 *  @return Returns a pointer to the child class created
 */
ConfigSection * ConfigSection::addNewSection(std::string sectionName)
{
  // Make sure there isn't a section named this already
  ConfigSection * retval = findSection(sectionName);

  if (!retval && sectionName.length())
  {
    std::string basesection = getBaseSectionName(sectionName);

    retval = findSection(basesection);
    if (!retval)
    {
      // Couldn't find the next hop in the list, add it
      retval = new ConfigSection(this, basesection);
      addSection(retval);
    }

    std::string childsection = getSubSectionName(sectionName);
    if (childsection.length())
    {
      // Recursively add children section
      retval = retval->addNewSection(childsection);
    }
  }

  return retval;
}

//! Extracts Section Name from raw string
/*! Takes raw string (from file) and removes brackets
 *  @param sectionName Name of Section in [SECTION_NAME] format
 *  @return Name of Section in SECTION_NAME format
 *  @exception InvalidConfigNameError If there isn't at least 1 
 *             character of section name
 */
std::string ConfigSection::parseSectionName(const char * sectionName)
{
  // Strip out name from inside of []
  std::string retval = sectionName;
  int startpos, endpos;

  startpos = retval.find('[') + 1; // +1 to skip it
  endpos = retval.find(']');

  // There should be at least one character of section name
  if ((endpos <= startpos + 1) || (startpos < 0))
  {
    throw InvalidConfigNameError(amosException::quote(sectionName));
  }

  retval = retval.substr(startpos, endpos-startpos);
  stripWhitespace(retval);

  return retval;
}

//! Recursively find and return proper section
/*! @param sectionName Section Name to search relative to current section
 *  @return Pointer to matching section or NULL if not found
 */
ConfigSection * ConfigSection::findSection(std::string sectionName)
{
  ConfigSection * retval = NULL;

  if (sectionName.length() == 0) 
  {
    // No Match on 0 length string
    
    retval = NULL;
  }
  else
  {
    std::string basesection = getBaseSectionName(sectionName);

    // Check the children

    std::list<ConfigSection *>::const_iterator hChildSection;
    for (hChildSection = m_sectionList.begin();
         hChildSection != m_sectionList.end();
         hChildSection++)
    {
      std::string childname = (*hChildSection)->getSectionName();
      if (childname == basesection)
      {
        // This is the right child

        // First check if this child is the full section requested
        // (avoids having to explicitly start every section path with ROOT.)
        if (childname == sectionName)
        {
          retval = *hChildSection;
        }
        else
        {
          retval = (*hChildSection)->findSection(getSubSectionName(sectionName));
        }
      }
    }
  }
  
  return retval;
}

//! Returns "base" from base[.child[.child2[...]]]
/*!
 * @exception InvalidConfigNameError On first character of sectionName is '.'
 */
std::string ConfigSection::getBaseSectionName(std::string sectionName)
{
  std::string retval = sectionName;
  int dotpos = sectionName.find('.');

  if (dotpos > 0)
  {
    retval = sectionName.substr(0,dotpos);
  }
  else if (dotpos == 0)
  {
    // First character can't be '.'
    throw InvalidConfigNameError(amosException::quote(sectionName));
  }
  // else not found

  return retval;
}


//! Strips "base." from base[.child[.child2[...]]]
/*!
 *  @exception InvalidConfigNameError On first character of sectionName is '.'
 */
std::string ConfigSection::getSubSectionName(std::string sectionName)
{
  std::string retval = "";
  int dotpos = sectionName.find('.');

  if (dotpos > 0)
  {
    retval = sectionName.substr(dotpos+1, sectionName.length());
  }
  else if (dotpos == 0)
  {
    // First character can't be '.'
    throw InvalidConfigNameError(amosException::quote(sectionName));
  }
  // else return ""

  return retval;
}

//! Parses a string, and adds variable=value into m_valueMap
/*! Used to extract (variable,value) pair from file. It does not do
 *  any variable substitutions, but it will strip away unneed whitespace.
 *
 *  @param varLine Raw line to parse (directly from file)
 *  @exception InvalidConfigNameError On first character of line is '='
 */
void ConfigSection::addVariable(const char * varLine)
{
  std::string raw = varLine;

  // First strip out comments
  int commentpos = raw.find(';');
  if (commentpos >= 0)
  {
    // this will try to erase beyond the end of the string, but is
    // handled gracefully
    raw.erase(commentpos, raw.length()); 
  }
  
  // Remove leading and trailing whitespace from line
  stripWhitespace(raw);

  // Make sure line isn't empty (or was a comment)
  if (raw.length())
  {
    int equalpos = raw.find('=');
    if (equalpos <= 0)
    {
      // First character can't be '='
      // but must contain an '=
      throw InvalidConfigNameError(amosException::quote(varLine));
    }

    // Separate variable and value pair
    std::string var = raw.substr(0, equalpos);
    std::string val = raw.substr(equalpos+1, raw.length() - equalpos+1);

    stripWhitespace(var);
    stripWhitespace(val);

    // var is guaranteed to have a value,
    // val is valid as an empty string ""
    m_valueMap[var] = val;
    
    // Reset iterator, although may be unnecessary
    m_valueMapIter = m_valueMap.begin();
  }
}

//! Strips leading and trailing white space from "interesting" characters
/*! Utility Function
 *  @param val Reference to string to strip
 */
void ConfigSection::stripWhitespace(std::string & val)
{
  std::string whitespace(" \t");

  if (val.length() > 0)
  {
    int startpos = val.find_first_not_of(whitespace);
    int endpos = val.find_last_not_of(whitespace);

    if (startpos < 0) 
    {
      // No interesting characters, startpos < 0 iff endpos < 0
      val = "";
    }
    else
    {
      val = val.substr(startpos, endpos-startpos+1);
    }
  }
}

//! Prints out full name of section by recursively prependind parent name
/*! @return Full name of section in SECTION_NAME.SUB_SECTION format
 *  @see getSectionName()
 */
std::string ConfigSection::getFullSectionName()
{
  std::string retval;
  if (m_parent)
  {
    // Only append if there is a parent to avoid showing implicit
    // [ROOT] Section
    retval = m_parent->getFullSectionName();
    if (retval.length()) retval += ".";

    retval += getSectionName();
  }

  return retval;
}

//! Prepends the full section name to a variable name
/*! 
 *  @param variableName Name of variable to prepend
 *  @return Full name of variable in SECTION_NAME.variableName form
 */
std::string ConfigSection::getFullVariableName(std::string variableName)
{
  std::string retval;
  std::string sectionName = getFullSectionName();

  if (sectionName.length()) retval = sectionName + ".";

  retval += variableName;
  return retval;
}

//! Dumps a section (and subsections) to an ostream
/*! Useful for debugging. 
 *  @param indentlevel Number of spaces to indent
 *  @param os Stream to output to
 */
void ConfigSection::printSection(int indentlevel, std::ostream & os)
{
  // Print Section Name
  os << getIndent(indentlevel-1) << "|-> [" << getSectionName() << "]";
  
  // Print Full Name
  os << " ([" << getFullSectionName() << "])" << std::endl;


  // Print each var,val pair
  std::map<std::string, std::string>::const_iterator mapiter;
  for (mapiter =  m_valueMap.begin();
       mapiter != m_valueMap.end();
       mapiter++)
  {
    os << getIndent(indentlevel) << "|-> " 
       << mapiter->first << "=" << mapiter->second << std::endl;
  }

  // Print Children
  std::list<ConfigSection *>::const_iterator hChildrenSection;
  for (hChildrenSection =  m_sectionList.begin();
       hChildrenSection != m_sectionList.end();
       hChildrenSection++)
  {
    (*hChildrenSection)->printSection(indentlevel+1, os);
  }
}

//! Gets a string of spaces (" ") 2*indentlevel characters long
std::string ConfigSection::getIndent(int indentlevel)
{
  std::string retval;

  for (int i=0; i < indentlevel; i++)
  {
    retval += "  ";
  }
  
  return retval;
}


//! Resolves the variable substitutions
/*! Works bottom-up (postfix) so that children are resolved
 *  before the current level. This way default rules can be specified
 *  at top level with specifics at lower levels. However, substitutions
 *  will be done and redone rather than saved away even in cases
 *  where there would be no conflict in scope. 
 *
 *  @exception CircularReferenceError On a reference than includes itself
 *  @exception MalformedSubstitutionError On a substitution that closes improperly
 */
void ConfigSection::resolveSubstitutions()
{
  // Resolve the children first
  std::list<ConfigSection *>::const_iterator hChildSection;
  for (hChildSection =  m_sectionList.begin();
       hChildSection != m_sectionList.end();
       hChildSection++)
  {
    (*hChildSection)->resolveSubstitutions();
  }

  // Foreach (var,val) of m_valueMap
  // while there is a variable substitution in val
  //   Resolve Variable
  //   Make Replacement

  std::map<std::string, std::string>::iterator mapiter;
  int beginpos, endpos;

  for (mapiter =  m_valueMap.begin();
       mapiter != m_valueMap.end();
       mapiter++)
  {
    // search for variable substitution 
    // search for }} first to allow for nested substitution
    
    while ((endpos = mapiter->second.find("}}")) >= 0)
    {
      // Check backwards to nearest {{
      beginpos = mapiter->second.rfind("{{" , endpos);

      // There better be a match and at least 1 character of variable name
      if ((endpos > beginpos+2) && (beginpos >= 0))
      {
        std::string replace =
               mapiter->second.substr(beginpos+2, endpos-beginpos-2);
        std::string replaceval = resolveVariable(replace, 
                                           getFullVariableName(mapiter->first));

        // Make sure that this isn't a circular replacement
        if (replaceval.find("{{" + replace + "}}") != replaceval.npos)
        {
          std::string error = amosException::quote(mapiter->second) + " in " +
                              amosException::quote(
                                         getFullVariableName(mapiter->first)); 

          throw CircularReferenceError(error);
        }

        // Erase everything in {{...}} and then insert replacement val
        mapiter->second.erase(beginpos, endpos-beginpos+2);
        mapiter->second.insert(beginpos, replaceval);
      }
      else
      {
        std::string error = amosException::quote(mapiter->second) + " in " +
                       amosException::quote(
                                       getFullVariableName(mapiter->first)); 

        throw MalformedSubstitutionError(error);
      }
    }
  }
}


//! Tries to resolve a variable from the current node up
/*! Recursively checks current node and then parent node for a variable
 *  and then returns its value
 *
 *  @param searchval Name of variable to search for (excludes {{...}})
 *  @return Value of variable with name searchval
 *  @exception UnresolvedSubstitution On can't find a match for a substitution
 */
std::string ConfigSection::resolveVariable(std::string searchval, 
                                           std::string searchVariable)
{
  std::string retval;

  // Search valueMap for the desired value
  std::map<std::string, std::string>::const_iterator searchiter;
  searchiter = m_valueMap.find(searchval);

  if (searchiter != m_valueMap.end() )
  {
    // Found a match

    retval = searchiter->second;
  }
  else if (m_parent)
  {
    // No match, search up tree
    
    retval = m_parent->resolveVariable(searchval, searchVariable);
  }
  else
  {
    // At root, but didn't find match
    std::string error = amosException::quote(searchval) + " in " +
                   amosException::quote(searchVariable);
                   
    throw UnresolvedSubstitutionError(error);
  }

  return retval;
}

//! Reset the iterators to the beginning for getNextSection and getNextVariable
/*!
 *  @see getNextSection
 *  @see getNextVariable
 */
void ConfigSection::resetIterators()
{
  m_sectionListIter = m_sectionList.begin();
  m_valueMapIter = m_valueMap.begin();
}

//! Returns a pointer to the next section in the section list
/*! The first time this is called, it returns the first section, after that
 *  it iterates through each section in the list.
 *
 *  @return The next section in the list, or NULL when list has been exhausted
 *  @see resetIterators
 */
ConfigSection * ConfigSection::getNextSection()
{
  ConfigSection * retval = NULL;

  if (m_sectionListIter != m_sectionList.end())
  {
    retval = *m_sectionListIter;
    m_sectionListIter++;
  }

  return retval;
}

//! Returns the next variable name in the variable map
/*! The first time it is called, it will return the name of the first variable
 *  in the map. Then it iterates until there are no more variables in the 
 *  current section. Use getValue to access the value of this variable. A
 *  result of "" always means the list has been exhausted because no variable
 *  can be named "" in the map. 
 *
 *  @return The next variable name or "" when list exhausted
 *  @see resetIterators
 *  @see getValue
 */
std::string ConfigSection::getNextVariable()
{
  std::string retval = "";

  if (m_valueMapIter != m_valueMap.end())
  {
    retval = m_valueMapIter->first;
    m_valueMapIter++;
  }

  return retval;
}

//! Returns the value for a variable from the map at the current section
/*! Note: Returns "" if not found which is indistinquishable from a variable
 *  having a value of "". Use hasVariable first to check for its existence.
 *
 *  @param variableName Name of variable to get value of
 *  @see hasVariable
 */
std::string ConfigSection::getValue(std::string variableName)
{
  return m_valueMap[variableName];
}

//! Checks for the existence of a variable in section map
/*! Only checks for existence, use getValue if this returns true to
 *  access its value.
 *
 *  @param variableName Name of variable
 *  @return Boolean indicating presense of variable
 *  @see getValue
 */
bool ConfigSection::hasVariable(std::string variableName)
{
  bool retval = false;

  std::map<std::string, std::string>::const_iterator foundPair;

  foundPair = m_valueMap.find(variableName);

  if (foundPair != m_valueMap.end())
  {
    retval = true;
  }

  return retval;
}
