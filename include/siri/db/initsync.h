/*
 * initsync.h - Initial replica synchronization
 *
 * author       : Jeroen van der Heijden
 * email        : jeroen@transceptor.technology
 * copyright    : 2016, Transceptor Technology
 *
 * changes
 *  - initial version, 22-07-2016
 *
 */
#pragma once

#include <stdio.h>
#include <uv.h>
#include <inttypes.h>
#include <siri/db/db.h>
#include <siri/db/series.h>
#include <siri/net/pkg.h>

typedef struct siridb_s siridb_t;
typedef struct sirinet_pkg_s sirinet_pkg_t;

typedef struct siridb_initsync_s
{
    FILE * fp;
    char * fn;
    int fd;
    long int size;
    uint32_t * next_series_id;
    sirinet_pkg_t * pkg;
} siridb_initsync_t;

siridb_initsync_t * siridb_initsync_open(siridb_t * siridb, int create_new);
void siridb_initsync_free(siridb_initsync_t ** initsync);
void siridb_initsync_run(uv_timer_t * timer);
void siridb_initsync_fopen(siridb_initsync_t * initsync, const char * opentype);
const char * siridb_initsync_sync_progress(siridb_t * siridb);
