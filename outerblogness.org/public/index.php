<?php
/*
Outer Blogness
(c) 2011 Main Street Plaza
mainstreetplaza.com
*/
require_once("mainclude/aggregate.inc");
define('MAGPIE_CACHE_ONLY', true);
include ('header.php');
include ('sidebarL.php');
include ('sidebarR.inc');
?>
<!--  The Posts Loop - Begin -->
  <div class="column1">
	  <h2>Recent Posts</h2>
    <?php foreach (getItems("PostMo") as $item) { ?>
      <h3><a href='<?php echo $item->link ?>'><?php echo $item->title ?></a></h3><!-- Print the title of the post -->
      <div class="author">by&nbsp;<?php echo $item->author ?> @ <?php echo $item->name ?></div>
			<br />
			<div class="summary"><em><?php echo $item->summary . "(" . date('j M',($item->adjustedTimeStamp)) . ")" ?></em></div><br />
    <?php } ?>
  </div> <!-- end column1 -->
<!--  The Posts Loop - End -->
<!--  The Comments Loop - Begin -->
	<div class="column2">
	  <h2>Recent Comments</h2>
	  <?php foreach (getItems("comments","106","10") as $item) { ?>
  	  <div><?php echo $item->author;
			$sumlen = strlen(trim(strip_tags($item->parentTitle)));
			  If ($item->parentTitle !== null) {
				  If ($sumlen >= "40") {
					  echo " on \"" . substr($item->parentTitle, 0, 40) . "...\" @ <a href=\"" . $item->link . "\">" . $item->name . "</a></div>"; }
					Else {
					  echo " on \"" . $item->parentTitle . "\" @ <a href=\"" . $item->link . "\">" . $item->name . "</a></div>"; }
				}
				Else {
					  echo " @ <a href=\"" . $item->link . "\">" . $item->name . "</a></div>"; } ?>
			<div class="summary"><em><?php $sumlen = strlen(trim(strip_tags($item->summary)));
				  If ($sumlen >= "100") {
					  echo substr($item->summary, 0, 100) . "..."; }
					Else {
						echo $item->summary; } ?></em></div>
		<?php } ?>
	</div> <!-- End - column2 -->
<!--  The Posts Loop - End -->
<?php include ('footer.php') ?>
