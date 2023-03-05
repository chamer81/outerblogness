#
# outerblogness.org docker
#

FROM debian:8

RUN apt-get update; \
    apt-get install -y --force-yes \
    nginx \
    php5-fpm \
    php5-cli \
    cron

RUN rm /etc/nginx/sites-enabled/default
COPY outerblogness.org /var/www/outerblogness.org
COPY outerblogness.org.conf /etc/nginx/sites-enabled/outerblogness.org.conf
COPY refresh_posts.sh /usr/local/bin/refresh_posts.sh
COPY refresh_comments.sh /usr/local/bin/refresh_comments.sh

# see https://stackoverflow.com/questions/37458287/how-to-run-a-cron-job-inside-a-docker-container
COPY ob_cron /etc/cron.d/ob_cron

# Give execution rights on the cron job
RUN chmod 0644 /etc/cron.d/ob_cron

# Apply cron job
RUN crontab /etc/cron.d/ob_cron

# Create the log file to be able to run tail
RUN touch /var/log/cron.log

COPY ob_startscript.sh /usr/local/bin/ob_startscript.sh

# Expose the port for nginx to listen on
EXPOSE 80

CMD ["ob_startscript.sh"]


