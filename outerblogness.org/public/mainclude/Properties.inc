<?php
/**
 * 
 *  Copyright (c) 2000-2001 David Giffin
 *
 *  Licensed under the GNU GPL. For full terms see the file COPYING.
 * 
 *  Properties - reads java style properities files.
 * 
 *  $Id: Properties.inc,v 1.1.1.1 2005/03/14 00:44:01 Russ Exp $
 *
 * @version   $Id: Properties.inc,v 1.1.1.1 2005/03/14 00:44:01 Russ Exp $
 * @package Combine
 * @author   David Giffin <david@giffin.org>
 * @since    PHP 4.0
 * @copyright Copyright (c) 2000-2003 David Giffin : LGPL - See LICENCE
 *
 */


/**
 * Properties Class
 *
 * Similar to Java Properties or HashMap. Added some features to be more PHP.
 * Like toArray() returns a pointer to the internal PHP array so that it can be
 * manipulated easily.
 *
 *<code>
 *<?php
 *
 * require_once("Combine/runtime/Properties.php");
 *
 * $properties = new Properties();
 * $properties->loadProperties("combine/combine.properties");
 *
 * // get and set the log level
 * $oldLogLevel = $properties->get("combine.logLevel");
 * $properties->set("combine.logLevel",4);
 *
 * // Get an associative array of all properties which match
 * // the pattern "combine.database"
 * $matches = $properties->getMatch("combine.database");
 *
 *?>
 *</code>
 *
 * @author   David Giffin <david@giffin.org>
 * @package  Combine
 */
class Properties {

	var $file = "";
	var $properties = array();

	/**
	 * Constructor
	 *
	 * @param string $file The file name to load the properties from
	 */
	function Properties($file = null) {
		if ($file) {
			$this->file = $file;
			$this->loadProperties();
		}
	}

	/**
	 * Load Properties from a file
	 *
	 * @param string $file The file name to load the properties from
	 */
	function loadProperties($file = null) {
		if ($file) {
			$this->file = $file;
		}

		if (!file_exists($this->file)) {
			die("Properties: file {$this->file} does not exist!\n");
		}

		$lines = file($this->file);
		foreach ($lines as $line) {
			if (preg_match("/^#/",$line) || preg_match("/^\s*$/",$line)) {
				continue;
			}
			if (preg_match("/^([a-zA-Z0-9\.]+)\s*=(.*)/",$line,$matches)) {
				$this->properties[trim($matches[1])] = trim($matches[2]);
			}
		}
	}

	/**
	 * Get a property value
	 *
	 * @param string $match regex expression for matching keys
	 * @return array an associtive array of values
	 */
	function get($key) {
		if ( isSet($this->properties[$key]) && $this->properties[$key] ) {
			return $this->properties[$key];
		}
		return null;
	}


	/**
	 * Get Properites ehich match a pattern
	 *
	 * @param string $match regex expression for matching keys
	 * @return array an associtive array of values
	 */
	function getMatch($match) {
		$ret = array();
		foreach ($this->properties as $key => $value) {
			if (preg_match("/$match/",$key)) {
				$ret[$key] = $value;
			}
		}
		return $ret;
	}


	/**
	 * Set Properties from an Array
	 *
	 * @param array $values an associtive array of values
	 */
	function setFromArray($values) {
		foreach ($values as $key => $value) {
			$this->set($key, $value);
		}
	}


	/**
	 * Set a Property
	 *
	 * @param string $key The key for the property
	 * @param string $value The value to set the property
	 */
	function set($key,$value) {
		$this->properties[$key] = $value;
	}


	/**
	 * Get the internal PHP Array
	 *
	 * @return array an associtive array of values
	 */
	function &toArray() {
		return $this->properties;
	}


}

?>
