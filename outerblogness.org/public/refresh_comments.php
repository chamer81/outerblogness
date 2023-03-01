<?php
define('MAGPIE_CACHE_AGE', 1);
require_once("mainclude/aggregate.inc");
ini_set('error_reporting', E_WARNING);
$config = getConfig();
foreach ($config->getFeedArray("comments") as $cat) {
  foreach ($cat->feeds as $feed) {
    $rss = fetch_rss($feed->link);
    $ii++;
  }
}
echo "Processed $ii feeds for comments.";
?>