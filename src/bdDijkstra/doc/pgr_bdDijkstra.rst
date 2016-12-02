..
   ****************************************************************************
    pgRouting Manual
    Copyright(c) pgRouting Contributors

    This documentation is licensed under a Creative Commons Attribution-Share
    Alike 3.0 License: http://creativecommons.org/licenses/by-sa/3.0/
   ****************************************************************************

.. _pgr_bdDijkstra:

pgr_bdDijkstra
===============================================================================

``pgr_bdDijkstra`` — Returns the shortest path(s) using Bidirectional Dijkstra algorithm.

.. figure:: ../../../doc/src/introduction/images/boost-inside.jpeg
   :target: http://www.boost.org/libs/graph/doc

   Boost Graph Inside


Synopsis
-------------------------------------------------------------------------------

Based on Dijkstra's algorithm, the bidirectional search finds a shortest path
a starting vertex (``start_vid``) to an ending vertex (``end_vid``).
It runs two simultaneous searches: one forward from the source, and one backward from the target,
stopping when the two meet in the middle.
This implementation can be used with a directed graph and an undirected graph.

Characteristics
-------------------------------------------------------------------------------

The main Characteristics are:

- Process is done only on edges with positive costs.
- Values are returned when there is a path.

- When the starting vertex and ending vertex are the same, there is no path.

  - The `agg_cost` the non included values `(v, v)` is `0`

- When the starting vertex and ending vertex are the different and there is no path:

  - The `agg_cost` the non included values `(u, v)` is :math:`\infty`

- Running time (worse case scenario): :math:`O((V \log V + E))`
- For large graphs where there is a path bewtween the starting vertex and ending vertex:

  - It is expected to terminate faster than pgr_dijkstra (one to one)


Signature Summary
-----------------

.. code-block:: none

    pgr_dijkstra(edges_sql, start_vid,  end_vid)
    pgr_bdDijkstra(edges_sql, start_vid, end_vid, directed)
    RETURNS SET OF (seq, path_seq, node, edge, cost, agg_cost)
        OR EMPTY SET

.. NOTE:: This signature is deprecated

    .. code-block:: sql

        pgr_bdDijkstra(sql, source integer, target integer, directed boolean, has_rcost boolean)
        RETURNS SET OF pgr_costResult

    - See :ref:`pgr_costResult <type_cost_result>`
    - See :ref:`bd_dijkstra_v2`



Signatures
-------------------------------------------------------------------------------

.. index::
    single: bdDijkstra(Minimal Use)

Minimal signature
.......................................

.. code-block:: none

    pgr_bdDijkstra(edges_sql, start_vid, end_vid)
    RETURNS SET OF (seq, path_seq, node, edge, cost, agg_cost) or EMPTY SET

The minimal signature is for a **directed** graph from one ``start_vid`` to one ``end_vid``:

:Example:

.. literalinclude:: doc-pgr_bdDijkstra.queries
   :start-after: -- q1
   :end-before: -- q2


.. index::
    single: bdDijkstra(Complete signature)

Complete Signature
.......................................

.. code-block:: none

    pgr_bdDijkstra(edges_sql, start_vid, end_vid, directed)
    RETURNS SET OF (seq, path_seq, node, edge, cost, agg_cost) or EMPTY SET

This signature finds the shortest path from one ``start_vid`` to one ``end_vid``:
  -  on a **directed** graph when ``directed`` flag is missing or is set to ``true``.
  -  on an **undirected** graph when ``directed`` flag is set to ``false``.

:Example:

.. literalinclude:: doc-pgr_bdDijkstra.queries
   :start-after: -- q2
   :end-before: -- q3



Description of the Signatures
-------------------------------------------------------------------------------

.. include:: ../../common/src/edges_input.h
    :start-after: basic_edges_sql_start
    :end-before: basic_edges_sql_end

.. include:: ../../dijkstra/sql/dijkstra.sql
    :start-after: pgr_dijkstra_parameters_start
    :end-before: pgr_dijkstra_parameters_end


Description of the return values
...............................................................................

Returns set of ``(seq, path_seq, node, edge, cost, agg_cost)``

============== ========== =================================================
Column         Type       Description
============== ========== =================================================
**seq**        ``INT``    Sequential value starting from **1**.
**path_seq**   ``INT``    Relative position in the path. Has value **1** for the beginning of a path.
**node**       ``BIGINT`` Identifier of the node in the path from ``start_vid`` to ``end_vid``.
**edge**       ``BIGINT`` Identifier of the edge used to go from ``node`` to the next node in the path sequence. ``-1`` for the last node of the path.
**cost**       ``FLOAT``  Cost to traverse from ``node`` using ``edge`` to the next node in the path sequence.
**agg_cost**   ``FLOAT``  Aggregate cost from ``start_v`` to ``node``.
============== ========== =================================================


Deprecated Signature
-------------------------------------------------------------------------------

:Example: Using the deprecated signature

The deprecated signature does not auto detects the existence of :code:`reverse_cost`

.. literalinclude:: doc-pgr_bdDijkstra.queries
   :start-after: -- q3
   :end-before: -- q4



See Also
-------------------------------------------------------------------------------

* The queries use the :ref:`sampledata` network.
* :ref:`pgr_dijkstra`
* http://www.cs.princeton.edu/courses/archive/spr06/cos423/Handouts/EPP%20shortest%20path%20algorithms.pdf
* https://en.wikipedia.org/wiki/Bidirectional_search

.. rubric:: Indices and tables

* :ref:`genindex`
* :ref:`search`

