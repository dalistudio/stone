石场系统
wangdali@qq.com

集成了keepalived + nginx + php-fpm + phpmyadmin等，还有一个sms短信网关的代码。

文件简介：
3rd/ 目录，保存这第三方的库。最后将解压到release/下，用于打包deb。
bixi/ 目录，这个即为php相关的代码。
DEBIAN 目录，用于deb打包的一些脚本。
doc/ 目录，项目相关的文档。
etc/ 目录，用于deb打包，复制到release/下使用。
sms/ 目录，短信网关相关代码。
taotie/ 目录，这个即为nginx相关的代码。
web/ 目录，这个为html文件的目录。
release/ 目录，这个用于临时保存deb文件，方便打包。
release.sh 文件，用于自动编译相关代码和生成stone.deb文件。

