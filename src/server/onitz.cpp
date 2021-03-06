/*
 *  Autotzd - automatic timezone detection
 *
 *  This file was originally sourced from timed, see top-level
 *  README file for more details.
 *
 *  Copyright (C) 2009-2011 Nokia Corporation.
 *
 *  autotzd is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License
 *  version 2.1 as published by the Free Software Foundation.
 *
 *  autotzd is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY;  without even the implied warranty  of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU Lesser General Public License  for more details.
 *
 *  You should have received a copy of the GNU  Lesser General Public
 *  License along with autotzd. If not, see http://www.gnu.org/licenses/
 *
 */

#include <sys/time.h>

#include <pcrecpp.h>

#include "../common/log.h"

#include "onitz.h"
#include "misc.h"

cellular_handler::cellular_handler()
{
}

cellular_handler *cellular_handler::static_object = NULL ;

cellular_handler *cellular_handler::object()
{
  if(static_object==NULL)
    static_object = new cellular_handler ;
  return static_object ;
}

void cellular_handler::uninitialize()
{
  if(static_object==NULL)
    return ;
  delete static_object ;
  static_object = NULL ;
}

cellular_handler::~cellular_handler()
{
}
