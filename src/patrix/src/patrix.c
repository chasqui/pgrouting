/*PGR-GNU*****************************************************************
File: patrix.c

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer: 
Copyright (c) 2015 Patrick Morales
Mail: 

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

#include "./../../common/src/postgres_connection.h"
#include "utils/array.h"


#include "./../../common/src/debug_macro.h"
#include "./../../common/src/time_msg.h"
#include "./../../common/src/pgr_types.h"
#include "./../../common/src/postgres_connection.h"
#include "./../../common/src/edges_input.h"
#include "./../../common/src/arrays_input.h"

#include "./patrix_driver.h"

Datum PGDLLEXPORT Datum patrix(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(patrix);


/*******************************************************************************/
/*                          MODIFY AS NEEDED                                   */
static
void
process( char* edges_sql,
        int64_t start_vid,
        ArrayType *ends,
        bool directed,
        General_path_element_t **result_tuples,
        size_t *result_count)
{
    pgr_SPI_connect();

    size_t size_end_vidsArr = 0;
    int64_t* end_vidsArr = 
        pgr_get_bigIntArray(&size_end_vidsArr, ends);


    PGR_DBG("Load data");
    pgr_edge_t_patrix *edges_patrix = NULL;   // Patrix : cambio del typo de estructura
    size_t total_edges = 0;

    pgr_get_edges_patrix(edges_sql, &edges_patrix, &total_edges);
    PGR_DBG("Total %ld edges in query:", total_edges);

    if (total_edges == 0) {
        PGR_DBG("No edges found");
        (*result_count) = 0;
        (*result_tuples) = NULL;
        pgr_SPI_finish();
        return;
    }

    PGR_DBG("Starting processing");
    char *err_msg = NULL;
    char *log_msg = NULL;

    // Code standard:
    // Pass the arrays and the sizes on the same line
    clock_t start_t = clock();


    do_pgr_patrix(
    		edges_patrix, total_edges,
            start_vid,
            end_vidsArr, size_end_vidsArr,
            directed,
            result_tuples,
            result_count,
            &log_msg,
            &err_msg);
    time_msg("processing pgr_patrix", start_t, clock());

    elog(DEBUG1,"Hola Patrix!!");
    elog(DEBUG1,"%s",log_msg);


    /*
     // This ones are for returning the info to postgres


    typedef struct {
        int seq;
        int64_t start_id;
        int64_t end_id;
        int64_t node;
        int64_t edge;
        double cost;
        double agg_cost;
    } General_path_element_t;

     */

    // elog(DEBUG1,result_tuples->seq);
/*    elog(DEBUG1,*result_tuples->start_id);
    elog(DEBUG1,*result_tuples->end_id);
    elog(DEBUG1,*result_tuples->node);
    elog(DEBUG1,*result_tuples->edge);
    elog(DEBUG1,*result_tuples->cost);

*/


    PGR_DBG("Returning %ld tuples\n", *result_count);
    PGR_DBG("LOG: %s\n", log_msg);
    if (log_msg) free(log_msg);

    if (err_msg) {
        if (*result_tuples) free(*result_tuples);
        if (end_vidsArr) free(end_vidsArr);
        elog(ERROR, "%s", err_msg);
        free(err_msg);
    }

    pfree(edges_patrix);
    pgr_SPI_finish();
}
/*                                                                            */
/******************************************************************************/

PGDLLEXPORT Datum
patrix(PG_FUNCTION_ARGS) {
    FuncCallContext     *funcctx;
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
    start_pid BIGINT,
    end_pids ANYARRAY,
    directed BOOLEAN DEFAULT true,
         **********************************************************************/

        PGR_DBG("Calling process");
        // Code standard:
        // Use same order as in the query
        // Pass the array and it's size on the same line
        process(
                text_to_cstring(PG_GETARG_TEXT_P(0)),
                PG_GETARG_INT64(1),
                PG_GETARG_ARRAYTYPE_P(2),
                PG_GETARG_BOOL(3),
                &result_tuples,
                &result_count);

        /*                                                                             */
        /*******************************************************************************/

#if PGSQL_VERSION > 95
        funcctx->max_calls = result_count;
#else
        funcctx->max_calls = (uint32_t)result_count;
#endif
        funcctx->user_fctx = result_tuples;
        if (get_call_result_type(fcinfo, NULL, &tuple_desc) != TYPEFUNC_COMPOSITE)
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("function returning record called in context "
                         "that cannot accept type record")));

        funcctx->tuple_desc = tuple_desc;
        MemoryContextSwitchTo(oldcontext);
    }

    funcctx = SRF_PERCALL_SETUP();
    tuple_desc = funcctx->tuple_desc;
    result_tuples = (General_path_element_t*) funcctx->user_fctx;

    if (funcctx->call_cntr < funcctx->max_calls) {
        HeapTuple    tuple;
        Datum        result;
        Datum        *values;
        bool*        nulls;
        size_t call_cntr = funcctx->call_cntr;

        /*******************************************************************************/
        /*                          MODIFY!!!!!                                        */
        /*  This has to match you ouput otherwise the server crashes                   */
        /*
           OUT seq INTEGER,
           OUT path_seq INTEGER,
           OUT start_vid BIGINT,
           OUT end_vid BIGINT,
           OUT node BIGINT,
           OUT edge BIGINT,
           OUT cost FLOAT,
           OUT agg_cost FLOAT
         ********************************************************************************/


        values = palloc(8 * sizeof(Datum));
        nulls = palloc(8 * sizeof(bool));

        size_t i;
        for(i = 0; i < 8; ++i) {
            nulls[i] = false;
        }


        // postgres starts counting from 1
        values[0] = Int32GetDatum(call_cntr + 1);
        values[1] = Int32GetDatum(result_tuples[call_cntr].seq);
        values[2] = Int64GetDatum(result_tuples[call_cntr].start_id);
        values[3] = Int64GetDatum(result_tuples[call_cntr].end_id);
        values[4] = Int64GetDatum(result_tuples[call_cntr].node);
        values[5] = Int64GetDatum(result_tuples[call_cntr].edge);
        values[6] = Float8GetDatum(result_tuples[call_cntr].cost);
        values[7] = Float8GetDatum(result_tuples[call_cntr].agg_cost);
        /*******************************************************************************/

        tuple = heap_form_tuple(tuple_desc, values, nulls);
        result = HeapTupleGetDatum(tuple);
        SRF_RETURN_NEXT(funcctx, result);
    } else {
        SRF_RETURN_DONE(funcctx);
    }
}
