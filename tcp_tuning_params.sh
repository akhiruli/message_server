#!/bin/bash

echo "net.core.wmem_max=33554432" >> /etc/sysctl.conf
echo "net.core.rmem_max = 33554432" >> /etc/sysctl.conf
echo "net.ipv4.tcp_rmem = 4096 87380 33554432" >> /etc/sysctl.conf
echo "net.ipv4.tcp_wmem = 4096 65536 33554432" >> /etc/sysctl.conf

echo 1 > /proc/sys/net/ipv4/tcp_tw_reuse

echo 10000000 > /proc/sys/fs/file-max

echo "fs.file-max = 100000" >> /etc/sysctl.conf

sysctl -p
