#!/bin/sh
# wangdali@qq.com

# 删除发布目录和文件
rm -r release/
rm stone-1.0.0.deb

# 创建发布目录
mkdir -p release/opt/loong/
mkdir -p release/opt/loong/bin/
mkdir -p release/opt/loong/etc/
mkdir -p release/opt/loong/web/
mkdir -p release/opt/loong/log/
mkdir -p release/opt/loong/tmp/
mkdir -p release/opt/loong/var/run/
mkdir -p release/etc/init.d

# 生成 taoite 程序
cd taotie/nginx-1.4.5/
./autogen.sh
cd ../..

# 复制 taotie 到发布目录
cp taotie/nginx-1.4.5/objs/nginx release/opt/loong/bin/taotie

# 生成 bixi 程序
cd bixi/php-5.5.9/
./autogen.sh
cd ../..

# 复制 bixi 到发布目录 
cp bixi/php-5.5.9/sapi/fpm/php-fpm release/opt/loong/bin/bixi

# 生成 sms 程序
cd sms && make
cd ..

# 复制 sms 到发布目录
cp sms/sms release/opt/loong/bin/

# 复制 DEBIAN 到发布目录
cp -r DEBIAN release/

# 复制 etc 到发布目录 
cp -r etc/* release/

# 复制 web 到发布目录
cp -r web release/opt/loong/

# 解包第三方包
dpkg -x 3rd/libjpeg-turbo8_1.3.0-0ubuntu2_amd64.deb release
dpkg -x 3rd/libmcrypt4_2.5.8-3.1ubuntu1_amd64.deb release
### Keepalived 和 ipvsadm 相关包
dpkg -x 3rd/keepalived/iproute_3.12.0-2_all.deb release
dpkg -x 3rd/keepalived/ipvsadm_1.26-2ubuntu1_amd64.deb release
dpkg -x 3rd/keepalived/keepalived_1.2.7-1ubuntu1_amd64.deb release
dpkg -x 3rd/keepalived/libperl5.18_5.18.2-2ubuntu1_amd64.deb release
dpkg -x 3rd/keepalived/libsensors4_3.3.4-2ubuntu1_amd64.deb release
dpkg -x 3rd/keepalived/libsnmp30_5.7.2~dfsg-8.1ubuntu3_amd64.deb release
dpkg -x 3rd/keepalived/libsnmp-base_5.7.2~dfsg-8.1ubuntu3_all.deb release

# 解压 phpmyadmin 包到release的web目录下
unzip 3rd/phpMyAdmin-4.2.2-all-languages.zip -d release/opt/loong/web/
# 目录重命名
mv release/opt/loong/web/phpMyAdmin-4.2.2-all-languages release/opt/loong/web/phpmyadmin
# 复制配置文件
cp release/opt/loong/web/phpmyadmin/config.sample.inc.php release/opt/loong/web/phpmyadmin/config.inc.php


# 打包 deb 安装文件
dpkg -b release stone-1.0.0.deb

# 清理生成的文件
# bixi
#cd bixi/php-5.5.9/
#make clean
#cd ../..

# sms
#cd sms/
#make clean
#cd ..

# taotie
#cd taotie/nginx-1.4.5/
#make clean
#cd ../..


