/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

//! @file 
//! @brief Defines the enum MessageLevel

#ifndef MESSAGELEVEL_HH
#define MESSAGELEVEL_HH 1

//! Message and filter levels for messages.
/*  Message levels are listed from least verbose/most critical to most verbose/
 *  least critical. There is a gap between each message level so that custom
 *  levels can be set in between the standard levels.
 *
 *  Filter levels set on the command line get a 10x multiplier
 *
 *  Note: If there are any additions to this list, make sure to add it
 *  to LogMsg::getMessageLevelStr() as well
 */
enum MessageLevel
{
  MESSAGE_LEVEL_DEFAULT           = -1, // Flag value
  MESSAGE_LEVEL_OFF               =  0,
  MESSAGE_LEVEL_DEBUG_FATAL       =  10,
  MESSAGE_LEVEL_DEBUG_ERROR       =  20,
  MESSAGE_LEVEL_DEBUG_WARNING     =  30,
  MESSAGE_LEVEL_DEBUG_INFORMATION =  40,
  MESSAGE_LEVEL_DEBUG_LOW         =  50,
  MESSAGE_LEVEL_DEBUG_MEDIUM      =  60,
  MESSAGE_LEVEL_DEBUG_HIGH        =  70
};

#endif
