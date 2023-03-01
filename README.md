# outerblogness
Very old and disorganized code for outerblogness.org

This code currently runs in a VM using docker-compose.

The docker image is built using the code in the outerblogness.org directory.

The VM runs an nginx server, and the outerblogness.org.conf file is the configuration file that is sym-linked into /etc/nginx/sites-enabled/ to route requests to the docker container.

The blogupdater folder contains code to update the feed configuration with new feeds (or to remove feeds).
