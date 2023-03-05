#!/bin/sh
# This is a script to clear the very old client files from the outerblogness cache.

CACHE_DIR=/var/www/outerblogness.org/public/cache
currentsecs=$(date +%s)

for f in $CACHE_DIR/*
do
  lastuse=$(stat -c "%Y" ${f})
  timesinceuse=$(($currentsecs - $lastuse))
  if [ "$timesinceuse" -gt 2678400 ]; then
    rm ${f}
    #ls -l ${f}
  fi
done

