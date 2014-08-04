./configure \
--user=loong \
--group=loong \
--prefix=/opt/loong \
--conf-path=etc/taotie.conf \
--error-log-path=log/error.log \
--http-log-path=log/access.log \
--pid-path=var/run/taotie.pid \
--http-client-body-temp-path=/tmp/client_body_temp \
--http-proxy-temp-path=/tmp/proxy_temp \
--http-fastcgi-temp-path=/tmp/fastcgi_temp \
--http-uwsgi-temp-path=/tmp/uwsgi_temp \
--http-scgi-temp-path=/tmp/scgi_temp \
--add-module=../mod/ndk \
--add-module=../mod/echo \
--add-module=../mod/headers \
--add-module=../mod/array \
--add-module=../mod/misc \
--add-module=../mod/memc \
--add-module=../mod/nck \
--add-module=../mod/srcache \
--add-module=../mod/form

make
