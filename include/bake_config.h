/*
                                   )
                                  (.)
                                  .|.
                                  | |
                              _.--| |--._
                           .-';  ;`-'& ; `&.
                          \   &  ;    &   &_/
                           |"""---...---"""|
                           \ | | | | | | | /
                            `---.|.|.|.---'

 * This file is generated by bake.lang.c for your convenience. Headers of
 * dependencies will automatically show up in this file. Include bake_config.h
 * in your main project file. Do not edit! */

#ifndef DRIVER_MNT_FILESTORE_BAKE_CONFIG_H
#define DRIVER_MNT_FILESTORE_BAKE_CONFIG_H

/* Headers of public dependencies */
#include <corto>
#include <corto.c>
#include <bake.util>

/* Headers of private dependencies */
#ifdef DRIVER_MNT_FILESTORE_IMPL
/* No dependencies */
#endif

/* Convenience macro for exporting symbols */
#if DRIVER_MNT_FILESTORE_IMPL && defined _MSC_VER
#define DRIVER_MNT_FILESTORE_EXPORT __declspec(dllexport)
#elif DRIVER_MNT_FILESTORE_IMPL
#define DRIVER_MNT_FILESTORE_EXPORT __attribute__((__visibility__("default")))
#elif defined _MSC_VER
#define DRIVER_MNT_FILESTORE_EXPORT __declspec(dllimport)
#else
#define DRIVER_MNT_FILESTORE_EXPORT
#endif

#endif
