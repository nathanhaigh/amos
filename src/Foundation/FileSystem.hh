/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

//! @file
//! @brief FileSystem class interface

#ifndef FILESYSTEM_HH
#define FILESYSTEM_HH 1

#include <unistd.h>
#include <stdio.h>

#include <string>
#include <fstream>
#include "Exceptions.hh"

//! Simple Wrappers for checking various attributes of the file system
/*! All methods are static, so instantiating this class is not required
 *  (methods can be accessed via result = FileSystem::methodName())
 */
class FileSystem
{
public: 
  FileSystem();
  
  // File functions
  static bool doesFileExist(const char *pathname);
  static bool isReadableFile(const char *filename);
  static bool isExecutableFile(const char *filename);
  static bool isWritableFile(const char *filename);
  static bool isCreatableFile(const char *filename);

  static bool isDirectory(const char *dirname);
  static bool isReadableDir(const char *dirname);
  static bool isWritableDir(const char *dirname);
  static bool isCreatableDir(const char *dirname);
  static bool isCreatablePath(const char *pathname);

  static char * getAbsolutePath();
  static int getPID();
  static std::string getPIDString();

  static std::ofstream * openOutputFile(const std::string & filename);
  static std::ifstream * openInputFile(const std::string & filename);
};

#endif
