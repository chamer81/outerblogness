#!/bin/sh

cd /var/www/outerblogness.org/public

/usr/bin/php refresh_posts.php > /dev/null 2>&1


