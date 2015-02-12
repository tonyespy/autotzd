/*
 *  Autotzd - automatic timezone detection
 *
 *  This file was originally sourced from timed, see top-level
 *  README file for more details.
 *
 *  Copyright (C) 2009-2011 Nokia Corporation.
 *  Copyright (C) 2015 Canonical, Inc.
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

#include "adaptor.h"
#include "autotzd.h"

#include "../common/log.h"

#include <QMetaType>
int main(int ac, char **av)
{

#if F_IMAGE_TYPE
  log_notice("image_type='%s'", image_type.c_str()) ;
#endif

  try
  {
    Autotzd *server = new Autotzd(ac,av) ;
    int result = server->exec() ;
    log_info("server finished; exiting...");
    delete server ;
    UnixSignal::uninitialize() ;
    return result ;
  }
  catch(const iodata::validator::exception &e)
  {
    log_critical("%s", e.info().c_str()) ;
  }
  catch(const iodata::exception &e)
  {
    log_critical("iodata::exception %s", e.info().c_str()) ;
  }
  catch(const std::exception &e)
  {
    log_critical("oops, unknown std::exception: %s", e.what()) ;
  }
  catch(...)
  {

    log_critical("oops, unknown exception of unknown type ...") ;
  }
  return 1 ;
}
