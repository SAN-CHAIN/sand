#!/bin/bash

echo "====[ Generating rippled.cfg ]==========="
if [ -z $CFG_FILE ]
then
CFG_FILE="/opt/ripple/conf/rippled.cfg"
fi
cat > $CFG_FILE <<- EOF
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

[validation_quorum]
EOF

if [ -z $VALIDATION_QUORUM ]
then
echo "2" >> $CFG_FILE
else
echo "$VALIDATION_QUORUM" >> $CFG_FILE
fi

if [ $IPS_FIXED ]
then
echo '[ips_fixed]' >> $CFG_FILE
OLD_IFS="$IFS"
IFS=","
ARR_IPS=($IPS_FIXED)
for IP in ${ARR_IPS[@]}
do
  IFS=":"
  IP_PORT=($IP)
  echo "${IP_PORT[0]} ${IP_PORT[1]}" >> $CFG_FILE
done
IFS="$OLD_IFS"
else
cat >> $CFG_FILE <<- EOF
[ips_fixed]
n1 51235
v1 51235
v2 51235
v3 51235
EOF
fi

if [ -z $PEER_PRIVATE ]
then
PEER_PRIVATE=1
fi
cat >> $CFG_FILE <<- EOF
[peer_private]
$PEER_PRIVATE
EOF

if [ $VALIDATORS ]
then
echo '[validators]' >> $CFG_FILE
OLD_IFS="$IFS"
IFS=","
ARR_VLD=($VALIDATORS)
CNT=1
for VLD in ${ARR_VLD[@]}
do
  echo "$VLD v${CNT}" >> $CFG_FILE
  CNT=$(($CNT+1))
done
IFS="$OLD_IFS"
else
cat >> $CFG_FILE <<- EOF
[validators]
n9K9ep8ueZ1NVprYUBNyD27RAvTspTE8vDfdv6AstwRQeeEjPMFJ V1
n9MT27s8NCamvnykv7z6fwZQ1S3etHA2uAvw6niGF2UihdppnS1T V2
n9LWwgAT1w83nNxWxQQoPj735yHEkdKnmScLSRNrPEAcEJco2NKe V3
EOF
fi

if [ -z $LOG_LEVEL ]
then
LOG_LEVEL="info"
fi

cat >> $CFG_FILE <<- EOF
[rpc_startup]
{ "command": "log_level", "severity": "$LOG_LEVEL" }
[ssl_verify]
1
EOF

if [ $DIVIDEND_PUBLIC ]
then
  cat >> $CFG_FILE <<- EOF
[dividend_account]
public_key=$DIVIDEND_PUBLIC
EOF
if [ $DIVIDEND_SECRET ]
then
  cat >> $CFG_FILE <<- EOF
secret_key=$DIVIDEND_SECRET
EOF
fi
fi


if [ $VALIDATION_SEED ]
then
  cat >> $CFG_FILE <<- EOF
[validation_seed]
$VALIDATION_SEED
[validation_public_key]
$VALIDATION_KEY
EOF
fi

if [ $MYSQL_HOST ]
then
  cat >> $CFG_FILE <<- EOF
[transaction_db]
type=mysql
host=$MYSQL_HOST
port=3306
username=$MYSQL_USER
password=$MYSQL_PASS
EOF
if [ $MYSQL_DATABASE ]
then
echo database=$MYSQL_DATABASE >> $CFG_FILE
else
echo database=transactions >> $CFG_FILE
fi
fi


echo "====[ rippled.cfg ]======================"
cat $CFG_FILE
echo "========================================="

if [ $LOAD_TYPE ]
then
/opt/ripple/bin/rippled --conf $CFG_FILE $LOAD_TYPE
else
/opt/ripple/bin/rippled --conf $CFG_FILE
fi