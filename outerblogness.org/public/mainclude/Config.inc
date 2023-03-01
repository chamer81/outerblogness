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
class Config {
  var $categories = array();
  var $cat;
  var $feed;
  var $feeds = array();
  var $curTag;
  var $curStyleName;
  var $category;
  var $parentFeed;

  function Config($file) {
    if (!$file) {
      die("Config: a file must be passed.\n");
    }
    if (!file_exists($file)) {
      die("Config: file {$file} does not exist!\n");
    }

    $parser = xml_parser_create();

    if (!is_resource($parser)) {
        $this->error( "Failed to create an instance of PHP's XML parser. " .
                      "http://www.php.net/manual/en/ref.xml.php",
                      E_USER_ERROR );
    }

    xml_set_object($parser, $this);
    xml_set_element_handler($parser, "startTag", "endTag");
    xml_set_character_data_handler($parser, "tagData");
    $status = xml_parse( $parser, file_get_contents($file));

    if (! $status ) {
        $errorcode = xml_get_error_code( $parser );
        if ( $errorcode != XML_ERROR_NONE ) {
            $xml_error = xml_error_string( $errorcode );
            $error_line = xml_get_current_line_number($parser);
            $error_col = xml_get_current_column_number($parser);
            $errormsg = "$xml_error at line $error_line, column $error_col";

            die( $errormsg );
        }
    }

    xml_parser_free( $parser );
    foreach($this->categories as $cat) {
      usort($cat->feeds, "feedSort");
    }
  }

  function startTag($parser, $tag, $attr) {
    $tag = strtolower($tag);
    $attr = array_change_key_case($attr, CASE_LOWER);
    $this->curTag = $tag;
    if ($tag == "category") {
      $this->category = new Category();
      $this->cat = $attr["name"];
      $this->category->isSearchable = ($attr["searchable"] == "false" ? false : true);
    } elseif ($tag == "feed") {
      $this->feed = new Feed();
    } elseif ($tag == "style") {
      $this->curStyleName = $attr["name"];
    }
  }

  function tagData($parser, $data) {
    if ($this->curTag == "link") {
      $this->feed->link .= $data;
    } elseif ($this->curTag == "title") {
      $this->feed->title .= $data;
    } elseif ($this->curTag == "offset") {
      $this->feed->offset .= $data;
    } elseif ($this->curTag == "style") {
      $this->feed->styles[$this->curStyleName] .= $data;
    } elseif ($this->curTag == "parentfeed") {
      $this->feed->parentFeed .= $data;
    } elseif ($this->curTag == "site") {
      $this->feed->site .= $data;
    } elseif ($this->curTag == "sidebartitle") {
      $this->feed->sidebarTitle .= $data;
    } elseif ($this->curTag == "type") {
        $this->feed->type .= $data;
    }
  }

  function endTag($parser, $tag) {
    $tag = strtolower($tag);
    if ($tag == "category") {
      $this->category->feeds = $this->feeds;
      $this->categories[$this->cat] = $this->category;
      $this->feeds = array();
    } else if ($tag == "feed") {
      if (!$this->feed->type) {
        $this->feed->type = 'post';
      }
      if (!$this->feed->sidebarTitle) {
        $this->feed->sidebarTitle = $this->feed->title;
      }
      $this->feeds[] = $this->feed;
    }
    $this->curTag = "";
  }

  function getFeedArray($category = null) {
    $ret = array();
    if ($category) {
      if (!empty($this->categories[$category])) {
        $ret = array($category => $this->categories[$category]);
      }
    } else {
      $ret = $this->categories;
    }
    return $ret;
  }

  function getFeed($link) {
    foreach ($this->getFeedArray() as $cat) {
      foreach ($cat->feeds as $feed) {
        if ($feed->link == $link) {
          return $feed;
        }
      }
    }
  }

  function getNameLinks($category = null, $prefix = "<li>", $suffix = "</li>") {
    $props = unserialize($_SESSION['session_props']);
    if (empty($this->categories)) {
      return null;
    }
    if ($category) {
      if (empty($this->categories[$category])) {
        return null;
      }
      usort($this->categories[$category]->feeds, "feedSort");
      foreach ($this->categories[$category]->feeds as $feed) {
        if ($feed->site) {
          if ($props->get('useRedirect')) {
            $ret[] = "$prefix<a href='redirect.php?url=".urlencode(trim($feed->site))."&feedurl=".urlencode(trim($feed->link))."&type=site'>$feed->sidebarTitle</a>$suffix";
          } else {
            $ret[] = "$prefix<a href='$feed->site'>$feed->sidebarTitle</a>$suffix";
          }
        }
      }
    } else {
      foreach ($this->categories as $cat) {
        usort($cat->feeds, "feedSort");
        foreach ($cat->feeds as $feed) {
          if ($props->get('useRedirect')) {
            $ret[] = "$prefix<a href='redirect.php?url=".urlencode(trim($feed->site))."&feedurl=".urlencode(trim($feed->link))."&type=site'>$feed->sidebarTitle</a>$suffix";
          } else {
            $ret[] = "$prefix<a href='$feed->site'>$feed->sidebarTitle</a>$suffix";
          }
        }
      }
    }
    return implode("", $ret);
  }


}

class Feed {
  var $link;
  var $title;
  var $offset;
  var $styles = array();
  var $parentFeed;
  var $site;
  var $sidebarTitle;
  var $type;
}

class Category {
  var $isSearchable = true;
  var $feeds = array();
}

function feedSort($obj1, $obj2) {
  return strcasecmp($obj1->sidebarTitle, $obj2->sidebarTitle);
}
?>
