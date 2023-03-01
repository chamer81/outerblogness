<?php
//refresh the cache every 50 minutes
define('MAGPIE_CACHE_AGE', 1);
require_once("mainclude/aggregate.inc");
ini_set('error_reporting', E_WARNING);
$conobj = getConfig();
foreach ($conobj->getFeedArray() as $cat) {
  foreach ($cat->feeds as $feed) {
    $rss = fetch_rss($feed->link);
    $ii++;
  }
}
echo "Processed $ii feeds - posts and comments.";
?>