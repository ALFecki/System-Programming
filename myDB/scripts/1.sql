CREATE TABLE customer(c_custkey INTEGER, c_name STRING, c_address STRING, c_nationkey INTEGER, c_phone STRING, c_acctbal DOUBLE, c_mktsegment STRING, c_comment STRING) AS HEAP;
INSERT '/home/alex/dev/bsuir/System-Programming/myDB/test_tables/customer.tbl' INTO customer;

CREATE TABLE nation(n_nationkey INTEGER, n_name STRING, n_regionkey INTEGER, n_comment STRING) AS HEAP;
INSERT '/home/alex/dev/bsuir/System-Programming/myDB/test_tables/nation.tbl' INTO nation;

CREATE TABLE region (r_regionkey INTEGER, r_name STRING, r_comment STRING) AS HEAP;
INSERT '/home/alex/dev/bsuir/System-Programming/myDB/test_tables/region.tbl' INTO region;


CREATE TABLE test1(id DOUBLE, val STRING) AS HEAP;

SELECT n.n_nationkey FROM nation AS n WHERE (n.n_name = 'UNITED STATES');

SELECT n.n_name
FROM nation AS n, region AS r
WHERE (n.n_regionkey = r.r_regionkey) AND (n.n_nationkey > 5);


SELECT n.n_name 
FROM nation AS n 
WHERE (n.n_nationkey = 24)

SELECT n.n_name 
FROM nation
WHERE (n.n_nationkey = 24)

SELECT n.n_name FROM nation AS n WHERE (n.n_nationkey > 5)