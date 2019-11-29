//
// Created by pieromack on 13/11/19.
//

#include "Query.h"

const QString Query::DROP_TABLE_PACKET = "DROP TABLE IF EXISTS %1;";
const QString Query::CREATE_TABLE_PACKET = "CREATE TABLE %1 ("
                                           "id_packet int auto_increment, "
                                           "hash_fcs varchar(8) NOT NULL, "
                                           "mac_addr varchar(17) NOT NULL, "
                                           "pos_x REAL(5,2) NOT NULL, "
                                           "pos_y REAL(5,2) NOT NULL, "
                                           "timestamp timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, "
                                           "ssid varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL, "
                                           "hidden int NOT NULL, "
                                           "PRIMARY KEY (id_packet) "
                                           ");";
const QString Query::DROP_TABLE_BOARD = "DROP TABLE IF EXISTS board_%1;";
const QString Query::CREATE_TABLE_BOARD = "CREATE TABLE board_%1 ("
                                          "id_board int NOT NULL, "
                                          "pos_x REAL(5,2) NOT NULL, "
                                          "pos_y REAL(5,2) NOT NULL,"
                                          "a int NOT NULL, "
                                          "PRIMARY KEY (id_board) "
                                          ");";
const QString Query::SELECT_ALL_PACKET = "SELECT * FROM %1;";
const QString Query::SELECT_ALL_BOARD = "SELECT id_board, pos_x, pos_y, a FROM board_%1;";
const QString Query::INSERT_BOARD = "INSERT INTO board_%1 (id_board, pos_x, pos_y, a) VALUES (:id_board, :pos_x, :pos_y, :a);";

const QString Query::SELECT_TIMING_COUNT_BUCKET_FREQ = "SELECT timing, COUNT(*)\n"
                                                       "FROM (SELECT mac_addr,\n"
                                                       "             FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), :sec)) AS timing,\n"
                                                       "             COUNT(DISTINCT timing)                                                    AS freq\n"
                                                       "      FROM (SELECT mac_addr,\n"
                                                       "            FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), :bucket)) AS timing\n"
                                                       "            FROM %1\n"
                                                       "            WHERE timestamp BETWEEN :fd AND :sd\n"
                                                       "            GROUP BY mac_addr, UNIX_TIMESTAMP(timestamp) DIV :bucket\n"
                                                       "            ORDER BY timing) as eL\n"
                                                       "      GROUP BY mac_addr, FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), :sec))\n"
                                                       "     ) AS mac_count\n"
                                                       "WHERE mac_count.freq >= :freq\n"
                                                       "GROUP BY timing\n"
                                                       "ORDER BY timing;";
const QString Query::SELECT_MAC_COUNT_HIDDEN_ORDERBY_FREQ = "SELECT mac_addr,\n"
                                                            "       COUNT(*) AS freq,\n"
                                                            "       hidden\n"
                                                            "FROM (SELECT mac_addr,\n"
                                                            "             FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), :sec)) AS timing,\n"
                                                            "             COUNT(DISTINCT timing)                                              AS freq,\n"
                                                            "             hidden\n"
                                                            "      FROM (SELECT mac_addr,\n"
                                                            "                   FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), :freq)) AS timing,\n"
                                                            "                   COUNT(DISTINCT timestamp),\n"
                                                            "                   hidden\n"
                                                            "            FROM %1\n"
                                                            "            WHERE timestamp BETWEEN :fd AND :sd\n"
                                                            "            GROUP BY mac_addr, UNIX_TIMESTAMP(timestamp) DIV :bucket\n"
                                                            "            ORDER BY timing) as eL\n"
                                                            "      GROUP BY mac_addr, FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), :sec))\n"
                                                            "     ) AS mac_count\n"
                                                            "WHERE mac_count.freq >= :freq\n"
                                                            "GROUP BY mac_addr\n"
                                                            "ORDER BY freq DESC;";
const QString Query::SELECT_MAC_COUNT_HIDDEN_ORDERBY_MAC = "SELECT mac_addr,\n"
                                                           "       COUNT(*) AS freq,\n"
                                                           "       hidden\n"
                                                           "FROM (SELECT mac_addr,\n"
                                                           "             FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), :sec)) AS timing,\n"
                                                           "             COUNT(DISTINCT timing)                                              AS freq,\n"
                                                           "             hidden\n"
                                                           "      FROM (SELECT mac_addr,\n"
                                                           "                   FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), :bucket)) AS timing,\n"
                                                           "                   COALESCE(COUNT(DISTINCT timestamp)  ,0),\n"
                                                           "                   hidden\n"
                                                           "            FROM %1\n"
                                                           "            WHERE timestamp BETWEEN :fd AND :sd\n"
                                                           "            GROUP BY mac_addr, UNIX_TIMESTAMP(timestamp) DIV :bucket\n"
                                                           "            ORDER BY timing) as eL\n"
                                                           "      GROUP BY mac_addr, FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), :sec))\n"
                                                           "     ) AS mac_count\n"
                                                           "WHERE mac_count.freq >= :freq\n"
                                                           "GROUP BY mac_addr\n"
                                                           "ORDER BY mac_addr;";

const QString Query::SELECT_MAC_TIMING_AVGPOS = "SELECT mac_addr,\n"
                                                "       FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), :sec)) AS timing,\n"
                                                "       avg(pos_x)                                                                    as pos_x,\n"
                                                "       avg(pos_y)                                                                    as pos_y\n"
                                                "FROM %1\n"
                                                "WHERE timestamp >= :fd\n"
                                                "  AND timestamp < :sd\n"
                                                "GROUP BY mac_addr, UNIX_TIMESTAMP(timestamp) DIV :sec\n"
                                                "ORDER BY timing;";
const QString Query::SELECT_MAC_TIMING_AVGPOS_SINGLE = "SELECT mac_addr,\n"
                                                       "       FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), 60)) AS timing,\n"
                                                       "       avg(pos_x)                                                                    as pos_x,\n"
                                                       "       avg(pos_y)                                                                    as pos_y\n"
                                                       "FROM %1\n"
                                                       "WHERE timestamp >= :fd\n"
                                                       "  AND timestamp < :sd\n"
                                                       "  AND mac_addr = :mac\n"
                                                       "GROUP BY UNIX_TIMESTAMP(timestamp) DIV 60\n"
                                                       "ORDER BY timing;";
const QString Query::SELECT_COUNT_LIVE = "SELECT COUNT(*)\n"
                                         "FROM (SELECT mac_addr,\n"
                                         "             FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), 300)) AS timing,\n"
                                         "             COUNT(DISTINCT timing)                                                   AS freq\n"
                                         "      FROM (SELECT mac_addr,\n"
                                         "                   FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), 60)) AS timing,\n"
                                         "                   avg(pos_x)                                                                    AS pos_x,\n"
                                         "                   avg(pos_y)                                                                    AS pos_y\n"
                                         "            FROM %1\n"
                                         "            WHERE timestamp BETWEEN :fd AND :sd\n"
                                         "            GROUP BY mac_addr, UNIX_TIMESTAMP(timestamp) DIV 60\n"
                                         "            ORDER BY timing) as eL\n"
                                         "      GROUP BY mac_addr, FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), :sec))\n"
                                         "     ) AS mac_count\n"
                                         "WHERE mac_count.freq >= :freq\n"
                                         "GROUP BY timing;";
const QString Query::SELECT_MAC_TIMING_LASTPOS = "SELECT mac_addr, timing, pos_x, pos_y\n"
                                                 "FROM (SELECT hash_fcs,\n"
                                                 "             mac_addr,\n"
                                                 "             avg(pos_x) as pos_x,\n"
                                                 "             avg(pos_y) as pos_y,\n"
                                                 "             timestamp,\n"
                                                 "             ssid,\n"
                                                 "             hidden,\n"
                                                 "             MAX(FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), 60)))\n"
                                                 "                 OVER (PARTITION BY mac_addr) AS timing\n"
                                                 "      FROM %1\n"
                                                 "      WHERE timestamp >= :fd AND timestamp < :sd\n"
                                                 "      GROUP BY mac_addr, UNIX_TIMESTAMP(timestamp) DIV 60\n"
                                                 "      ORDER BY timing) AS s2\n"
                                                 "WHERE FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), 60)) = timing\n"
                                                 "ORDER BY mac_addr, timestamp DESC;";

const QString Query::SELECT_HIDDEN_NOT_MAC = "SELECT * FROM %1 WHERE hidden='1' AND timestamp >= :fd AND timestamp <= :sd AND mac_addr != :mac;";
const QString Query::SELECT_HIDDEN_MAC = "SELECT * FROM %1 WHERE mac_addr = :mac AND timestamp >= :fd AND timestamp <= :sd;";


