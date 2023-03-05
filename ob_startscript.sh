#!/bin/sh

service php5-fpm start
service nginx start
cron
tail -f /var/log/cron.log
