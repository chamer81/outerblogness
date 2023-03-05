#!/bin/sh

cd /var/www/outerblogness.org/public

/usr/bin/php refresh_comments.php > /dev/null 2>&1


