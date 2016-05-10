/*
 * props.c - Functions to return SiriDB properties.
 *
 * author       : Jeroen van der Heijden
 * email        : jeroen@transceptor.technology
 * copyright    : 2016, Transceptor Technology
 *
 * changes
 *  - initial version, 17-03-2016
 *
 */
#include <siri/db/props.h>
#include <logger/logger.h>
#include <siri/version.h>
#include <siri/grammar/grammar.h>
#include <stdio.h>
#include <uuid/uuid.h>
#include <siri/db/time.h>
#include <assert.h>
#include <uv.h>
#include <procinfo/procinfo.h>
#include <string.h>
#include <siri/siri.h>

#define SIRIDB_PROP_MAP(NAME, LEN)      \
if (map)                                \
{                                       \
    qp_add_type(packer, QP_MAP2);       \
    qp_add_raw(packer, "name", 4);      \
    qp_add_raw(packer, NAME, LEN);      \
    qp_add_raw(packer, "value", 5);     \
}

static void prop_buffer_path(siridb_t * siridb, qp_packer_t * packer, int map);
static void prop_buffer_size(siridb_t * siridb, qp_packer_t * packer, int map);
static void prop_dbname(siridb_t * siridb, qp_packer_t * packer, int map);
static void prop_dbpath(siridb_t * siridb, qp_packer_t * packer, int map);
static void prop_libuv(siridb_t * siridb, qp_packer_t * packer, int map);
static void prop_log_level(siridb_t * siridb, qp_packer_t * packer, int map);
static void prop_max_open_files(siridb_t * siridb, qp_packer_t * packer, int map);
static void prop_mem_usage(siridb_t * siridb, qp_packer_t * packer, int map);
static void prop_open_files(siridb_t * siridb, qp_packer_t * packer, int map);
static void prop_pool(siridb_t * siridb, qp_packer_t * packer, int map);
static void prop_server(siridb_t * siridb, qp_packer_t * packer, int map);
static void prop_timezone(siridb_t * siridb, qp_packer_t * packer, int map);
static void prop_time_precision(
        siridb_t * siridb,
        qp_packer_t * packer,
        int map);
static void prop_uptime(siridb_t * siridb, qp_packer_t * packer, int map);
static void prop_uuid(siridb_t * siridb, qp_packer_t * packer, int map);
static void prop_version(siridb_t * siridb, qp_packer_t * packer, int map);
static void prop_who_am_i(siridb_t * siridb, qp_packer_t * packer, int map);

extern char * who_am_i;

void siridb_init_props(void)
{
    for (uint_fast16_t i = 0; i < KW_COUNT; i++)
        siridb_props[i] = NULL;

    siridb_props[CLERI_GID_K_BUFFER_PATH - KW_OFFSET] = prop_buffer_path;
    siridb_props[CLERI_GID_K_BUFFER_SIZE - KW_OFFSET] = prop_buffer_size;
    siridb_props[CLERI_GID_K_DBNAME - KW_OFFSET] = prop_dbname;
    siridb_props[CLERI_GID_K_DBPATH - KW_OFFSET] = prop_dbpath;
    siridb_props[CLERI_GID_K_LIBUV - KW_OFFSET] = prop_libuv;
    siridb_props[CLERI_GID_K_MAX_OPEN_FILES - KW_OFFSET] = prop_max_open_files;
    siridb_props[CLERI_GID_K_MEM_USAGE - KW_OFFSET] = prop_mem_usage;
    siridb_props[CLERI_GID_K_LOG_LEVEL - KW_OFFSET] = prop_log_level;
    siridb_props[CLERI_GID_K_OPEN_FILES - KW_OFFSET] = prop_open_files;
    siridb_props[CLERI_GID_K_POOL - KW_OFFSET] = prop_pool;
    siridb_props[CLERI_GID_K_SERVER - KW_OFFSET] = prop_server;
    siridb_props[CLERI_GID_K_TIMEZONE - KW_OFFSET] = prop_timezone;
    siridb_props[CLERI_GID_K_TIME_PRECISION - KW_OFFSET] = prop_time_precision;
    siridb_props[CLERI_GID_K_UPTIME - KW_OFFSET] = prop_uptime;
    siridb_props[CLERI_GID_K_UUID - KW_OFFSET] = prop_uuid;
    siridb_props[CLERI_GID_K_VERSION - KW_OFFSET] = prop_version;
    siridb_props[CLERI_GID_K_WHO_AM_I - KW_OFFSET] = prop_who_am_i;
}

static void prop_buffer_path(siridb_t * siridb, qp_packer_t * packer, int map)
{
    SIRIDB_PROP_MAP("buffer_path", 11)
        qp_add_string(packer, siridb->buffer_path);
}

static void prop_buffer_size(siridb_t * siridb, qp_packer_t * packer, int map)
{
    SIRIDB_PROP_MAP("buffer_size", 11)
    qp_add_int32(packer, (int32_t) siridb->buffer_size);
}

static void prop_dbname(siridb_t * siridb, qp_packer_t * packer, int map)
{
    SIRIDB_PROP_MAP("dbname", 6)
    qp_add_string(packer, siridb->dbname);
}

static void prop_dbpath(siridb_t * siridb, qp_packer_t * packer, int map)
{
    SIRIDB_PROP_MAP("dbpath", 6)
    qp_add_string(packer, siridb->dbpath);
}

static void prop_libuv(siridb_t * siridb, qp_packer_t * packer, int map)
{
    SIRIDB_PROP_MAP("libuv", 5)
    qp_add_string(packer, uv_version_string());
}

static void prop_log_level(siridb_t * siridb, qp_packer_t * packer, int map)
{
    SIRIDB_PROP_MAP("log_level", 9)
    qp_add_string(packer, Logger.level_name);
}

static void prop_max_open_files(siridb_t * siridb, qp_packer_t * packer, int map)
{
    SIRIDB_PROP_MAP("max_open_files", 14)
    qp_add_int32(packer, (int32_t) abs(siri.cfg->max_open_files));
}

static void prop_mem_usage(siridb_t * siridb, qp_packer_t * packer, int map)
{
    SIRIDB_PROP_MAP("mem_usage", 9)
    qp_add_int32(packer, (int32_t) (procinfo_total_physical_memory() / 1024));
}

static void prop_open_files(siridb_t * siridb, qp_packer_t * packer, int map)
{
    SIRIDB_PROP_MAP("open_files", 10)

    int open_files = procinfo_open_files(siridb->dbpath);

    if (    siridb->buffer_path != siridb->dbpath &&
            strncmp(
                siridb->dbpath,
                siridb->buffer_path,
                strlen(siridb->dbpath)))
        open_files += procinfo_open_files(siridb->buffer_path);

    qp_add_int16(packer, open_files);
}

static void prop_pool(siridb_t * siridb, qp_packer_t * packer, int map)
{
    SIRIDB_PROP_MAP("pool", 4)
    qp_add_int16(packer, (int16_t) siridb->server->pool);
}

static void prop_server(siridb_t * siridb, qp_packer_t * packer, int map)
{
    SIRIDB_PROP_MAP("server", 6)
    qp_add_string(packer, siridb->server->name);
}

static void prop_timezone(siridb_t * siridb, qp_packer_t * packer, int map)
{
    SIRIDB_PROP_MAP("timezone", 8)
    qp_add_string(packer, iso8601_tzname(siridb->tz));
}

static void prop_time_precision(
        siridb_t * siridb,
        qp_packer_t * packer,
        int map)
{
    SIRIDB_PROP_MAP("time_precision", 14)
    assert (siridb->time->precision >= SIRIDB_TIME_SECONDS &&
            siridb->time->precision <= SIRIDB_TIME_NANOSECONDS);
    qp_add_string(packer, siridb_time_short_map[siridb->time->precision]);
}

static void prop_uptime(siridb_t * siridb, qp_packer_t * packer, int map)
{
    SIRIDB_PROP_MAP("uptime", 6)
    qp_add_int32(packer, (int32_t) ((uint32_t) time(NULL) - siridb->start_ts));
}

static void prop_uuid(siridb_t * siridb, qp_packer_t * packer, int map)
{
    SIRIDB_PROP_MAP("uuid", 4)
    char uuid[37];
    uuid_unparse_lower(siridb->uuid, uuid);
    qp_add_string(packer, uuid);
}

static void prop_version(siridb_t * siridb, qp_packer_t * packer, int map)
{
    SIRIDB_PROP_MAP("version", 7)
    qp_add_string(packer, SDB_VERSION);
}

static void prop_who_am_i(siridb_t * siridb, qp_packer_t * packer, int map)
{
    SIRIDB_PROP_MAP("who_am_i", 8)
    qp_add_string(packer, who_am_i);
}
