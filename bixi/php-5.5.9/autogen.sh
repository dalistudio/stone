./configure \
--prefix=/opt/loong \
--sbindir=/opt/loong/bin \
--with-config-file-path=/opt/loong/etc \
--with-mcrypt=/usr/local/libmcrypt \
--with-zlib \
--with-openssl \
--with-mysql \
--with-mysqli \
--with-mysql-sock \
--with-gd \
--with-jpeg-dir=/usr/lib \
--with-gettext \
--with-curl \
--with-pdo-mysql \
--with-bz2 \
--enable-sockets \
--enable-bcmath \
--enable-xml \
--enable-zip \
--enable-fpm \
--enable-mbstring \
--enable-gd-native-ttf \
--enable-pdo

make
