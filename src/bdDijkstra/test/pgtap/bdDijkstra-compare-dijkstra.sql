\i setup.sql

SELECT plan(1734);

SET client_min_messages TO ERROR;

UPDATE edge_table SET cost = cost + 0.001 * id * id, reverse_cost = reverse_cost + 0.001 * id * id;

CREATE or REPLACE FUNCTION bddijkstra_compare_dijkstra(cant INTEGER default 17)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
inner_sql TEXT;
dijkstra_sql TEXT;
bddijkstra_sql TEXT;
BEGIN

    FOR i IN 1.. cant LOOP
        FOR j IN 1.. cant LOOP

            -- DIRECTED
            inner_sql := 'SELECT id, source, target, cost, reverse_cost FROM edge_table';
            dijkstra_sql := 'SELECT * FROM pgr_dijkstra($$' || inner_sql || '$$, ' || i || ', ' || j
                || ', true)';

            bddijkstra_sql := 'SELECT * FROM pgr_MY_FUNCTION_NAME_LOWER($$' || inner_sql || '$$, ' || i || ', ' || j
                || ', true)';
            RETURN query SELECT set_eq(bddijkstra_sql, dijkstra_sql, MY_FUNCTION_NAME_LOWER_sql);

            dijkstra_sql := 'SELECT * FROM pgr_dijkstra($$' || inner_sql || '$$, ' || i || ', ' || j
                || ', true)';

            bddijkstra_sql := 'SELECT * FROM pgr_MY_FUNCTION_NAME_LOWER($$' || inner_sql || '$$, ' || i || ', ' || j
                || ', true)';
            RETURN query SELECT set_eq(bddijkstra_sql, dijkstra_sql, MY_FUNCTION_NAME_LOWER_sql);



            inner_sql := 'SELECT id, source, target, cost FROM edge_table';
            dijkstra_sql := 'SELECT * FROM pgr_dijkstra($$' || inner_sql || '$$, ' || i || ', ' || j
                || ', true)';

            bddijkstra_sql := 'SELECT * FROM pgr_MY_FUNCTION_NAME_LOWER($$' || inner_sql || '$$, ' || i || ', ' || j
                || ', true)';
            RETURN query SELECT set_eq(bddijkstra_sql, dijkstra_sql, MY_FUNCTION_NAME_LOWER_sql);



            -- UNDIRECTED
            inner_sql := 'SELECT id, source, target, cost, reverse_cost FROM edge_table';
            dijkstra_sql := 'SELECT * FROM pgr_dijkstra($$' || inner_sql || '$$, ' || i || ', ' || j
                || ', false)';

            bddijkstra_sql := 'SELECT * FROM pgr_MY_FUNCTION_NAME_LOWER($$' || inner_sql || '$$, ' || i || ', ' || j
                || ', false)';
            RETURN query SELECT set_eq(bddijkstra_sql, dijkstra_sql, MY_FUNCTION_NAME_LOWER_sql);

            dijkstra_sql := 'SELECT * FROM pgr_dijkstra($$' || inner_sql || '$$, ' || i || ', ' || j
                || ', false)';

            bddijkstra_sql := 'SELECT * FROM pgr_MY_FUNCTION_NAME_LOWER($$' || inner_sql || '$$, ' || i || ', ' || j
                || ', false)';
            RETURN query SELECT set_eq(bddijkstra_sql, dijkstra_sql, MY_FUNCTION_NAME_LOWER_sql);



            inner_sql := 'SELECT id, source, target, cost FROM edge_table';
            dijkstra_sql := 'SELECT * FROM pgr_dijkstra($$' || inner_sql || '$$, ' || i || ', ' || j
                || ', false)';

            bddijkstra_sql := 'SELECT * FROM pgr_MY_FUNCTION_NAME_LOWER($$' || inner_sql || '$$, ' || i || ', ' || j
                || ', false)';
            RETURN query SELECT set_eq(bddijkstra_sql, dijkstra_sql, MY_FUNCTION_NAME_LOWER_sql);


        END LOOP;
    END LOOP;

    RETURN;
END
$BODY$
language plpgsql;

SELECT * from bddijkstra_compare_dijkstra();


SELECT * FROM finish();
ROLLBACK;

