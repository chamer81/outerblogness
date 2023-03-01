<?php
/*
Blogroll Z rss/atom feed aggregator
Copyright (C) 2006  Russ Johnston

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
session_start();
ini_set('error_reporting', E_WARNING);

date_default_timezone_set(UTC);

require_once('magpierss/rss_fetch.inc');
require_once("Properties.inc");
require_once("Config.inc");
require_once("Channel.inc");

define('DEBUG', 0);
define('MAGPIE_USER_AGENT', "Outer Blogness ". strtotime("now"). " (+http://postmo.rameumptom.org; No cache)");
define('MAGPIE_OUTPUT_ENCODING', 'UTF-8');
define('MAGPIE_CACHE_ON', 1);
if ( !defined('ZEPTO_SUB_DIR') ) {
    define('ZEPTO_SUB_DIR', './');
}

function configure () {
  $reconfig = false;
  $configPath = realpath(ZEPTO_SUB_DIR . "config")."/";
  $configfile = $configPath . "feedConfig.xml";
  $ftime = filemtime($configfile);
  if (empty($_SESSION['session_config']) || empty($_SESSION['session_configtime']) || $ftime != $_SESSION['session_configtime']) {
    if (DEBUG == 1) {
      echo "<br>session config file time: ". $_SESSION['session_configtime'];
      echo "<br>config file time: $ftime";
      echo "<br>adding configuration to session. id = ".session_id();
    }
    $_SESSION['session_config'] = serialize(new Config($configfile));
    $_SESSION['session_configtime'] = $ftime;
  }
  if (DEBUG == 1) {
    echo "<br>config loaded";
  }
  
  $propfile = $configPath . "ma.props";
  $propfiletime = filemtime($propfile);
  if (DEBUG == 1) {
    echo "<br>Load properties";
  }
  if (empty($_SESSION['session_props']) || empty($_SESSION['session_proptime']) || $propfiletime != $_SESSION['session_proptime']) {
    if (DEBUG == 1) {
      echo "<br>adding props to session";
    }
    $_SESSION['session_props'] = serialize(new Properties($propfile));
    $_SESSION['session_proptime'] = $propfiletime;
  }
  $props = unserialize($_SESSION['session_props']);
  define('MAGPIE_CACHE_DIR', $props->get('cacheDirectory'));
  define('MAGPIE_CACHE_AGE', 1800);
  if (DEBUG == 1) {
    echo "<br>props: ". $_SESSION['session_props'];
    echo "<br>config: ".$_SESSION['session_config'];
  }
}
configure();

function itemSortDesc($obj1, $obj2) {
  return $obj2->adjustedTimeStamp - $obj1->adjustedTimeStamp;
}

function itemSortAsc($obj1, $obj2) {
  return $obj1->adjustedTimeStamp - $obj2->adjustedTimeStamp;
}

function getConfig () {
  return unserialize($_SESSION['session_config']);
}

function getItems($category = null, $size = null, $limit = null, $desc = true) {
  $mychannel = new Channel($category, $limit, $desc);
  return $mychannel->getItems($size);
}
?>