/* filestore.h
 * This is the main package file. Include this file in other projects.
 * Only modify inside the header-end and body-end sections.
 */

#ifndef DRIVER_MNT_FILESTORE_H
#define DRIVER_MNT_FILESTORE_H

#include "bake_config.h"

#define DRIVER_MNT_FILESTORE_ETC ut_locate("driver.mnt.filestore", NULL, UT_LOCATE_ETC)

/* $header() */
/* Definitions that are required by package headers (native types) go here. */
/* $end */

#include "_type.h"
#include "_interface.h"
#include "_load.h"
#include "_binding.h"

#include <driver.mnt.filestore.c>

/* $body() */
/* Definitions here that need your package headers go here. */
/* $end */

#endif

