#!/bin/bash

echo "====[ Generating rippled.cfg ]==========="
cat > /opt/ripple/conf/rippled.cfg <<- EOF
[server]
ip = 0.0.0.0
port_rpc
port_peer
port_ws

[port_rpc]
port = 5005
admin = 127.0.0.1,172.17.0.1
protocol = http

[port_peer]
port = 51235
protocol = peer

[port_ws]
port = 5006
protocol = ws

[node_size]
medium

[node_db]
type=RocksDB
path=/opt/ripple/data/rocksdb
open_files=2000
filter_bits=12
cache_mb=256
file_size_mb=8
file_size_mult=2

[database_path]
/opt/ripple/data/

[ledger_history]
full

[debug_logfile]
/opt/ripple/log/debug.log

[sntp_servers]
time.windows.com
time.apple.com
time.nist.gov
pool.ntp.org

[ips_fixed]
n1 51235
v1 51235
v2 51235
v3 51235

[peer_private]
1

[validators]
n9Lfxm6xTbyi47pzWVPoEuu2sMgHFFLPZBKi9KUUURkcCRG4Utpv v1
n94LGhadjn9RbCWSbdTSwe28ESnwJ2G15fzJBiQWG3fW1tkTpFRc v2
n9KegxFLBdpccaq4p2dYULVqDraryh5x9exxgZzncvNdUp5sJQhT v3

[validation_quorum]
2

[rpc_startup]
{ "command": "log_level", "severity": "trace" }

[ssl_verify]
1
EOF

if [ $DIVIDEND_PUBLIC ]
then
  cat >> /opt/ripple/conf/rippled.cfg <<- EOF
[dividend_account]
public_key=$DIVIDEND_PUBLIC
EOF
if [ $DIVIDEND_SECRET ]
then
  cat >> /opt/ripple/conf/rippled.cfg <<- EOF
secret_key=$DIVIDEND_SECRET
EOF
fi
fi  

if [ $VALIDATION_SEED ]
then
  cat >> /opt/ripple/conf/rippled.cfg <<- EOF
[validation_seed]
$VALIDATION_SEED
[validation_public_key]
$VALIDATION_KEY
EOF
else
  cat >> /opt/ripple/conf/rippled.cfg <<- EOF
[transaction_db]
type=mysql
host=$MYSQL_HOST
port=3306
username=$MYSQL_USER
password=$MYSQL_PASS
database=transaction
EOF
fi
echo "====[ rippled.cfg ]======================"
cat /opt/ripple/conf/rippled.cfg
echo "========================================="

if [ $LOAD_TYPE ]
then
/opt/ripple/bin/rippled --conf /opt/ripple/conf/rippled.cfg $LOAD_TYPE
else
/opt/ripple/bin/rippled --conf /opt/ripple/conf/rippled.cfg
fi

