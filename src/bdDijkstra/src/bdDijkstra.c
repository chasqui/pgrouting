/*PGR-GNU*****************************************************************
File: bdDijkstra.c

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2016 Celia Virginia Vergara Castillo
Mail: vicky_vergara@hotmail.com


------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

********************************************************************PGR-GNU*/

#include "postgres.h"
#include "executor/spi.h"
#include "funcapi.h"
#include "utils/array.h"
#include "catalog/pg_type.h"
#if PGSQL_VERSION > 92
#include "access/htup_details.h"
#endif
#include "fmgr.h"

#include <string.h>
#include "./../../common/src/debug_macro.h"
#include "./../../common/src/e_report.h"
#include "./../../common/src/time_msg.h"
#include "./../../common/src/pgr_types.h"
#include "./../../common/src/postgres_connection.h"
#include "./../../common/src/edges_input.h"

#include "./bdDijkstra_driver.h"

PG_FUNCTION_INFO_V1(bdDijkstra);
PGDLLEXPORT Datum bdDijkstra(PG_FUNCTION_ARGS);


/******************************************************************************/
/*                          MODIFY AS NEEDED                                  */
static
void
process(
        char* edges_sql,
        int64_t start_vid,
        int64_t end_vid,
        bool directed,
        bool only_cost,
        General_path_element_t **result_tuples,
        size_t *result_count) {


    pgr_SPI_connect();

    PGR_DBG("Load data");
    pgr_edge_t *edges = NULL;
    size_t total_edges = 0;

    if (start_vid == end_vid) {
        pgr_SPI_finish();
        return;
    }

    pgr_get_edges(edges_sql, &edges, &total_edges);
    PGR_DBG("Total %ld edges in query:", total_edges);

    if (total_edges == 0) {
        PGR_DBG("No edges found");
        pgr_SPI_finish();
        return;
    }

    PGR_DBG("Starting processing");
    clock_t start_t = clock();
    char *log_msg = NULL;
    char *notice_msg = NULL;
    char *err_msg = NULL;
    do_pgr_bdDijkstra(
            edges,
            total_edges,
            start_vid,
            end_vid,
            directed,
            only_cost,
            result_tuples,
            result_count,
            &log_msg,
            &notice_msg,
            &err_msg);

    time_msg(" processing pgr_bdDijkstra", start_t, clock());
    PGR_DBG("Returning %ld tuples", *result_count);

    if (err_msg) {
        if (*result_tuples) free(*result_tuples);
    }
    pgr_global_report(&log_msg, &notice_msg, &err_msg);

    pfree(edges);
    pgr_SPI_finish();
}
/*                                                                            */
/******************************************************************************/

PGDLLEXPORT Datum bdDijkstra(PG_FUNCTION_ARGS) {
    FuncCallContext     *funcctx;
    uint32_t            call_cntr;
    uint32_t            max_calls;
    TupleDesc           tuple_desc;

    /**************************************************************************/
    /*                          MODIFY AS NEEDED                              */
    /*                                                                        */
    General_path_element_t  *result_tuples = 0;
    size_t result_count = 0;
    /*                                                                        */
    /**************************************************************************/

    if (SRF_IS_FIRSTCALL()) {
        MemoryContext   oldcontext;
        funcctx = SRF_FIRSTCALL_INIT();
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);


        /**********************************************************************/
        /*                          MODIFY AS NEEDED                          */
        /*
           edges_sql TEXT,
    start_vid BIGINT,
    end_vid BIGINT,
    directed BOOLEAN DEFAULT true,
    only_cost BOOLEAN DEFAULT false,
         **********************************************************************/


        PGR_DBG("Calling process");
        // Code standard:
        // Use same order as in the query
        // Pass the array and it's size on the same line

        process(
                pgr_text2char(PG_GETARG_TEXT_P(0)),
                PG_GETARG_INT64(1),
                PG_GETARG_INT64(2),
                PG_GETARG_BOOL(3),
                PG_GETARG_BOOL(4),
                &result_tuples,
                &result_count);

        /*                                                                    */
        /**********************************************************************/

        funcctx->max_calls = (uint32_t)result_count;
        funcctx->user_fctx = result_tuples;
        if (get_call_result_type(fcinfo, NULL, &tuple_desc)
                != TYPEFUNC_COMPOSITE) {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("function returning record called in context "
                         "that cannot accept type record")));
        }

        funcctx->tuple_desc = tuple_desc;
        MemoryContextSwitchTo(oldcontext);
    }

    funcctx = SRF_PERCALL_SETUP();
    call_cntr = funcctx->call_cntr;
    max_calls = funcctx->max_calls;
    tuple_desc = funcctx->tuple_desc;
    result_tuples = (General_path_element_t*) funcctx->user_fctx;

    if (call_cntr < max_calls) {
        HeapTuple    tuple;
        Datum        result;
        Datum        *values;
        bool*        nulls;

        /**********************************************************************/
        /*                          MODIFY AS NEEDED                          */
        /*
               OUT seq INTEGER,
    OUT path_seq INTEGER,
    OUT node BIGINT,
    OUT edge BIGINT,
    OUT cost FLOAT,
    OUT agg_cost FLOAT
         ***********************************************************************/

        values = palloc(6 * sizeof(Datum));
        nulls = palloc(6 * sizeof(bool));


        size_t i;
        for (i = 0; i < 6; ++i) {
            nulls[i] = false;
        }

        // postgres starts counting from 1
        values[0] = Int32GetDatum(call_cntr + 1);
        values[1] = Int32GetDatum(result_tuples[call_cntr].seq);
        values[2] = Int64GetDatum(result_tuples[call_cntr].node);
        values[3] = Int64GetDatum(result_tuples[call_cntr].edge);
        values[4] = Float8GetDatum(result_tuples[call_cntr].cost);
        values[5] = Float8GetDatum(result_tuples[call_cntr].agg_cost);
        /**********************************************************************/

        tuple = heap_form_tuple(tuple_desc, values, nulls);
        result = HeapTupleGetDatum(tuple);
        SRF_RETURN_NEXT(funcctx, result);
    } else {
        PGR_DBG("cleanup");
        if (result_tuples) free(result_tuples);

        SRF_RETURN_DONE(funcctx);
    }
}
