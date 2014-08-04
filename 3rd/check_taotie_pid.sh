#!/bin/bash 

# 检查 taotie pid是否为0
# 为0则尝试启动 taotie，等待3秒，再次检查。
# 如果 未能启动 taotie，则关闭 keepalived。
A=`ps -C taotie --no-header |wc -l`              
if [ $A -eq 0 ];then                      
       /opt/loong/bin/taotie
       sleep 3
       if [ `ps -C taotie --no-header |wc -l` -eq 0 ];then
             killall keepalived          
       fi
fi 
