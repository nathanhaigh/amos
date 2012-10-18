/*
 * Copyright (c) 2003, The Institute for Genomic Research (amos).  All
 * rights reserved.
 */

// $Id$

//! @file 
//! @brief FileSystem class implementation

#include "FileSystem.hh"
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#define PATH_DELIMINATOR '/'

//! All methods are static so instantiating this class is not required
FileSystem::FileSystem()
{

}

bool FileSystem::doesFileExist(const char *filename)
{
  return !access(filename, F_OK);
}

bool FileSystem::isReadableFile(const char *filename)
{
  return !access(filename, R_OK);
}

bool FileSystem::isExecutableFile(const char *filename)
{
  return !access(filename, X_OK);
}

bool FileSystem::isWritableFile(const char *filename)
{
  return !access(filename, W_OK);
}


bool FileSystem::isCreatableFile(const char *filename)
{
  bool retval;

  if (doesFileExist(filename))
  {
  retval = isWritableFile(filename);
  }
  else
  {
    // Check to see if path was given
    const char * end_of_path = strrchr(filename, PATH_DELIMINATOR);
    
    if (end_of_path)
    {
      // copy from beginning of string to end of path
      char path_name [1024];
      int len = 1024;

      if ((end_of_path - filename) < (len-1))
       len = end_of_path - filename;

      strncpy(path_name, filename, (end_of_path - filename));
      path_name[end_of_path-filename] = '\0';

      retval = isWritableDir(path_name);
    }
    else
    {
      // No path info was given, assume "./"
      retval = isWritableDir("./");
    }
  }
    
  return retval;
}


// Directory calls: 
//  check if it is a directory and then wrap appropriate file calls

bool FileSystem::isDirectory(const char *dirname)
{
  bool retval = false;

  if (doesFileExist(dirname))
  {
    struct stat buf;

    if (!stat(dirname, &buf))
    {
      retval = S_ISDIR(buf.st_mode);
    }
  }
  
  return retval;
}

bool FileSystem::isReadableDir(const char *dirname)
{
  return isDirectory(dirname) &&
         isReadableFile(dirname) &&
         isExecutableFile(dirname); 
}

bool FileSystem::isWritableDir(const char *dirname)
{
  return isDirectory(dirname) && 
         isWritableFile(dirname) &&
         isExecutableFile(dirname);
}

bool FileSystem::isCreatableDir(const char *dirname) 
{
  return isCreatableFile(dirname);
}

bool FileSystem::isCreatablePath(const char *pathname)
{
  return isCreatableFile(pathname);
}


//! Returns a pointer to a static buffer containing the absolute path of the CWD
/*! Note: The buffer will be overwritten by each call to getAbsolutePath
 */
char * FileSystem::getAbsolutePath()
{
  static char path[1024];

  getcwd(path, sizeof(path));

  return path;
}

int FileSystem::getPID()
{
  return getpid();
}

std::string FileSystem::getPIDString()
{
  char buffer[64];
  sprintf(buffer, "%d", getPID());

  return (std::string) buffer;
}

//! Tries to open a file for appending, and throws an exception if it cant
/*!
 *  @param filename Name of file to open
 *  @exception UnableToOpenError If the file can't be opened for some reason
 *  @returns Pointer to ofstream of filename or NULL if filename == ""
 */
std::ofstream * FileSystem::openOutputFile(const std::string & filename)
{
  std::ofstream * retval = NULL;

  if (filename.length())
  {
    retval = new std::ofstream (filename.c_str(), std::ios::app);

    if (!retval || !retval->is_open())
    {
      throw UnableToOpenError(amosException::quote(filename),
                              "Logger::openOutputFile");
    }
  }

  return retval;
}


//! Tries to open a file for reading, and throws an exception if it cant
/*! Wraps ifstream constructor.
 *
 *  @param filename Name of file to open
 *  @exception UnableToOpenError If the file can't be opened for some reason
 *  @returns Pointer to ifstream of filename or NULL if filename == ""
 */
std::ifstream * FileSystem::openInputFile(const std::string & filename)
{
  std::ifstream * retval = NULL;

  if (filename.length())
  {
    retval = new std::ifstream (filename.c_str());

    if (!retval || !retval->is_open())
    {
      throw UnableToOpenError(amosException::quote(filename),
                              "Logger::openIntputFile");
    }
  }

  return retval;
}
