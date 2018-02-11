/**
 * Adding some extra math/stat functions to SQlite
 * License: MIT
 *
 * Build with: gcc -fPIC -shared stats.c -o stats.so $(pkg-config --cflags sqlite3.pc) -lm
 * Load with:  SELECT load_extension('stats');
 *
 * Note: load_extension takes the full path (minus the suffix) to find the shared library
 **/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sqlite3ext.h>

SQLITE_EXTENSION_INIT1

typedef struct SAggCtx SAggCtx;
struct SAggCtx {
    double square_sum;
    int count;
};

/** RMS Step function, add the scalar value to our context */
static void rms_step(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    // Somehow this starts off initialized... (values set to zero)
    SAggCtx* p = (SAggCtx*)sqlite3_aggregate_context(context, sizeof(*p));

    if (!p) {
        sqlite3_result_error_nomem(context);
        return;
    }

    switch(sqlite3_value_type(argv[0])) {
        case SQLITE_INTEGER:
        case SQLITE_FLOAT:
        {
            double rVal = sqlite3_value_double(argv[0]);
            p->square_sum = p->square_sum + (rVal * rVal);
            p->count++;
            break;
        }
        default:
        {
            fprintf(stdout, "Invalid input to RMS function\n");
        }
    }
}

/** Reduce scalars to calculate the Root Mean Squared of all the values */
static void rms_finalize(sqlite3_context* context)
{
    SAggCtx* p = (SAggCtx*)sqlite3_aggregate_context(context, sizeof(*p));
    if (p->count > 0)
    {
        sqlite3_result_double(context, sqrt(p->square_sum / p->count));
    }
    else
    {
        sqlite3_result_double(context, -1.0);
    }
}

int sqlite3_extension_init(
    sqlite3 *db,
    char **pzErrMsg,
    const sqlite3_api_routines *pApi
)
{
    int rc = SQLITE_OK;
    SQLITE_EXTENSION_INIT2(pApi);

    sqlite3_create_function(db, "rms",  1, SQLITE_UTF8 | SQLITE_DETERMINISTIC, NULL,
        NULL,
        &rms_step,
        &rms_finalize
    );
    return rc;
}

/* vim: ts=4 sw=4 sts=4 expandtab ffs=unix ft=c :
 */
