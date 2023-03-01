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
class ChannelItem {
  var $link;
  var $title;
  var $content;
  var $summary;
  var $author;
  var $name;
  var $timeStamp;
  var $adjustedTimeStamp;
  var $fullSummary;
  var $offset;
  var $styles = array();
  var $parentTitle;
  var $feedLink;
  var $type;

  function ChannelItem($item, $name, $offset, $styles, $parentFeedLink, $feedLink, $type = null) {
    $props = unserialize($_SESSION['session_props']);
    $this->type = $type;
    $this->feedLink = $feedLink;
    $this->styles = $styles;
    $this->offset = $offset;
    if ($props->get('useRedirect')) {
      $this->link = $props->get('redirect')."?url=".urlencode(trim($item['link']))
        ."&feedurl=".urlencode(trim($feedLink))
        ."&type=$this->type";
    } else {
      $this->link = $item['link'];
    }
    $this->title = $item['title'];
    if ($parentFeedLink != null) {
      $rss = fetch_rss($parentFeedLink);
      if (is_array($rss->items)) {
        foreach ($rss->items as $rssItem) {
          if (strncmp($rssItem['link'], $this->link, strlen($rssItem['link'])) == 0) {
            $this->parentTitle = $rssItem['title'];
          }
        }
      }
    }
    if ($this->parentTitle == null) {
      $this->parentTitle = null;
    }
    $this->content = $item['content']['encoded'];
    if ($this->content == null) {
      $this->content = $item["atom_content"];
    }
    $this->author = $item['author_name'];
    if ($this->author == null) {
      $this->author = $item['dc']['creator'];
    }
    if ($this->content == null) {
      $sumlen = strlen(trim(strip_tags($item["summary"])));
      $tsum = substr(strip_tags($item["summary"]), 0, $props->get("summaryLength"));
    } else {
      $sumlen = strlen(trim(strip_tags($this->content)));
      $tsum = substr(strip_tags($this->content), 0, $props->get("summaryLength"));
    }
    if ($sumlen > $props->get("summaryLength")) {
      $this->summary = substr($tsum, 0, strrpos($tsum, " ")) . "...";
    } else {
      $this->summary = $tsum;
    }
    $this->name = $name;
    if ($item["date_timestamp"]) {
      $this->timeStamp = $item["date_timestamp"];
    } else if ($item["issued"] != null) {
      $this->timeStamp = strtotime(
        str_replace("T", " ", substr(trim($item["issued"]), 0, 19)));
    } else if ($item["dc"]["date"] != null) {
      $this->timeStamp = strtotime(
        str_replace("T", " ", substr($item["dc"]["date"], 0, 19)));
    } else if ($item["pubdate"]) {
      $this->timeStamp = strtotime($item["pubdate"]);
    } else if ($item["published"]) {
      $this->timeStamp = strtotime(
        str_replace("T", " ", substr(trim($item["published"]), 0, 19)));
    } else if ($item["updated"]) {
      $this->timeStamp = strtotime(
        str_replace("T", " ", substr(trim($item["updated"]), 0, 19)));
    }
    $this->adjustedTimeStamp = strtotime("+$offset hours", $this->timeStamp);
    $this->fullSummary = $item["summary"];
  }

  function getFormattedTimeStamp ($format = null, $adjust = false) {
    if ($format == null) {
      $format = "M, jS Y h:i:s A";
    }
    if ($adjust) {
      return date($format, $this->adjustedTimeStamp);
    }
    return date($format, $this->timeStamp);
  }

  function getContent($strip = true) {
    if ($strip) {
      return strip_tags($this->content);
    }
    return $this->content;
  }

  function getSearchText() {
    if ($this->content) {
      return $this->getContent(true);
    }
    return strip_tags($this->fullSummary);
  }

  function getStyle($name) {
    return $this->styles[$name];
  }
}

class Channel {
  var $channelItems;

  function Channel($category = null, $postCountLimit = null, $sortDesc = true) {
    $props = unserialize($_SESSION['session_props']);
    $chconf = getConfig();
    foreach ($chconf->getFeedArray($category) as $cat) {
      foreach ($cat->feeds as $feed) {
        $rss = fetch_rss($feed->link);
        if (is_array($rss->items)) {
          if ($postCountLimit === 0) {
            $items = $rss->items;
          } else if ($postCountLimit) {
            $items = array_slice($rss->items, 0, $postCountLimit);
          } elseif ($props->get('feedSortSliceSize')) {
            $items = array_slice($rss->items, 0, $props->get('feedSortSliceSize'));
          } else {
            $items = $rss->items;
          }
          foreach ($items as $rssItem) {
            $this->channelItems[] = new ChannelItem($rssItem, $feed->title,
              $feed->offset, $feed->styles, $feed->parentFeed, $feed->link, $feed->type);
          }
        }
      }
    }
    if (!empty($this->channelItems)) {
      if ($sortDesc) {
        usort($this->channelItems, "itemSortDesc");
      } else {
        usort($this->channelItems, "itemSortAsc");
      }
    }
  }

  function getItems($length = null) {
    if (empty($this->channelItems)) {
      return array();
    }
    if ($length === 0) {
      return $this->channelItems;
    }
    $props = unserialize($_SESSION['session_props']);
    if (!$length) {
      $length = $props->get("channelSliceSize");
    }
    if (!$length) {
      return $this->channelItems;
    }
    return array_slice($this->channelItems, 0, $length);
  }
}
?>
