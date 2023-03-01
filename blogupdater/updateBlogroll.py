#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
from os import listdir
from os.path import join, dirname, exists, isfile
import xml.dom.minidom
from xml.dom.minidom import Node
import codecs
from urllib2 import urlopen
from urllib import urlencode, urlopen
from shutil import copy
import feedfinder
import BeautifulSoup

# this is a test
realBlogroll = "../outerblogness.org/public/blogroll.inc"
realFeedConfig = "../outerblogness.org/public/config/feedConfig.xml"
intermediateBlogroll = "tmp/blogroll.inc"
intermediateFeedConfig = "tmp/feedConfig.xml"
tmpDir = "tmp"

# Use the local feedfinder.py:
source_tree_script = join(dirname(__file__), "feedfinder.py")
if exists(source_tree_script):
    sys.path.insert(0, dirname(source_tree_script))
    try:
        import feedfinder
    finally:
        del sys.path[0]
else:
    print "make sure feedfinder.py is in the current directory"


def get_child_text(pnode, keyname):
    for child in pnode.childNodes:
        if child.nodeName == keyname:
            for grandchild in child.childNodes:
                if grandchild.nodeType == Node.TEXT_NODE:
                    return grandchild.data
    return "unknown"

def create_data_child(doc, pnode, keyname, keyval):
    childElem = doc.createElement(keyname)
    pnode.appendChild(childElem)
    textNode = doc.createTextNode(keyval)
    childElem.appendChild(textNode)

def addFeedNode(feedDoc, categoryNode, feedtype, linkElem, site_title, siteURL, offset):
    placed_it = False
    feedElem = feedDoc.createElement("feed")
    create_data_child(feedDoc, feedElem, "type", feedtype)
    create_data_child(feedDoc, feedElem, "link", linkElem)
    create_data_child(feedDoc, feedElem, "title", site_title)
    create_data_child(feedDoc, feedElem, "site", siteURL)
    create_data_child(feedDoc, feedElem, "offset", offset)
    categoryNode.appendChild(feedElem)

def openFeedDoc(inFile):
    if exists(inFile):
        return xml.dom.minidom.parse(inFile)
    return xml.dom.minidom.parseString('<feeds><category name="PostMo"/><category name="comments"/></feeds>')

def writeFeedDoc(feedDoc, outFile):
    xmlstring = feedDoc.toxml()
    outputFile = codecs.open(outFile, "w", "utf-8")
    outputFile.write(xmlstring)
    outputFile.close()


def checkBlogroll():
    notFoundFile = "notfound.txt"
    changedFile = "changed.txt"
    rollIn = codecs.open(realBlogroll, 'r', "utf-8")
    rollOut = codecs.open(intermediateBlogroll, 'w', "utf-8")
    notFoundOut = codecs.open(join(tmpDir, notFoundFile), 'w', "utf-8")
    changedOut = codecs.open(join(tmpDir, changedFile), 'w', "utf-8")

    # create the new feed config file:
    feedDoc = openFeedDoc(realFeedConfig)
    for categoryNode in feedDoc.getElementsByTagName("category"):
        if categoryNode.getAttribute("name") == "PostMo":
            postMoNode = categoryNode
        if categoryNode.getAttribute("name") == "comments":
            commentsNode = categoryNode

    for line in rollIn:
        title_start = line.find('">')
        title_end = line.find('</a>')
        addFeeds = False
        if title_start < title_end:
            line_title = line[title_start+2:title_end]
            print line_title

            url_start = line.find('<a href="')
            url_end = line.find('">')
            if url_start < url_end:
                line_url = line[url_start+9:url_end].strip()
                print line_url
                
                try:
                    # find the blog's current title
                    soup = BeautifulSoup.BeautifulSoup(urlopen(line_url))
                    site_title = soup.title.string.strip()
                    print 'Found site: %s\n' % site_title
                    print 'for url: %s\n' % line_url
                except:
                    notFoundOut.write('%s' % line)
                    site_title = ''
                if site_title == 'Blog nicht gefunden.' or site_title == 'Blogger: Anmelden':
                    notFoundOut.write('%s' % line)
                elif site_title.startswith(line_title):
                    print 'OK'
                    rollOut.write(line)
                    addFeeds = True
                elif len(site_title) > 0:
                    rollOut.write(line)
                    changedOut.write('%s => %s\n' % (line_title, site_title))
                    addFeeds = True
                # now add the feeds:
                if addFeeds is True:
                    try:
                        feeds = feedfinder.feeds(line_url)
                        print 'found feeds %s\n' % feeds
                        mainFeed = None
                        commentsFeed = None
                        for feed in feeds:
                            print "found feed %s" % feed
                            if "comments" in feed:
                                commentsFeed = feed
                            else:
                                mainFeed = feed
                        if mainFeed is not None:
                            print "adding main feed: %s" % mainFeed
                            addFeedNode(feedDoc, postMoNode, "post", mainFeed, site_title, line_url, "0")
                        if commentsFeed is not None:
                            print "adding comments feed: %s" % commentsFeed
                            addFeedNode(feedDoc, commentsNode, "comments", commentsFeed, site_title, line_url, "0")
                        writeFeedDoc(feedDoc, intermediateFeedConfig)
                    except:
                        notFoundOut.write("%s - feed timed out\n" % line)

    changedOut.close()
    notFoundOut.close()
    rollOut.close()
    rollIn.close()
    copy(intermediateBlogroll, realBlogroll)
    copy(intermediateFeedConfig, realFeedConfig)

        
def updateBlogroll(argv):
    if len(sys.argv) < 3:
        print "usage: python updateBlogroll.py [post|comments|delete] url [offset]"
        sys.exit(1)
    
    feedtype = unicode(sys.argv[1], "utf-8")
    print "feedtype: " + feedtype
    baresite = sys.argv[2]
    site = unicode(baresite, "utf-8")
    print "baresite: " + baresite
    print "site: " + site
    if len(sys.argv) > 3:
        offset = unicode(sys.argv[3], "utf-8")
    else:
        offset = "0"
        
    if feedtype != "delete":
        # find the blog's title and feeds
        soup = BeautifulSoup.BeautifulSoup(urlopen(baresite))
        site_title = soup.title.string.strip()
        print site_title
        try:
            feeds = feedfinder.feeds(baresite)
            print 'found feeds %s\n' % feeds
            link = unicode(feeds[0], "utf-8")
        except:
            print("timed out finding feeds")

        if len(feeds) < 1:
            print "no feeds found"
            sys.exit(1)
        if len(feeds) is 1:
            linkElem = feeds[0]
        else:
            i = 0
            for foundfeed in feeds:
                print "(" + str(i) + ") " + foundfeed
                i += 1
            rawFeedIndex = raw_input("> ")
            feedIndex = int(rawFeedIndex)
            linkElem = feeds[feedIndex]
            print "you have chosen number " + rawFeedIndex + " corresponding to " + linkElem

    # open the blogroll file and add/remove the blog
    rollIn = codecs.open(realBlogroll, 'r', "utf-8")
    if feedtype == "delete":
        with codecs.open(intermediateBlogroll, 'w', "utf-8") as rollOut:
            for line in rollIn:
                if line.find(baresite) != -1:
                    print "deleting"
                else:
                    rollOut.write(line)
    if feedtype == "post":
        with codecs.open(intermediateBlogroll, 'w', "utf-8") as rollOut:
            inserted = False
            new_line = '<li><a href="' + site + '">' + site_title + '</a></li>\n'
            for line in rollIn:
                title_start = line.find('">')
                title_end = line.find('</a>')
                if title_start < title_end:
                    line_title = line[title_start+2:title_end]
                    print line_title
                    if (line_title.upper() > site_title.upper()) and (inserted == False):
                        rollOut.write(new_line)
                        rollOut.write(line)
                        inserted = True
                        print "adding to blogroll"
                    else:
                        rollOut.write(line)
            # write at end if not alphabetically before any other blogs:
            if inserted == False:
                rollOut.write(new_line)            
    copy(intermediateBlogroll, realBlogroll)

    # open the feed file and add/remove the blog
    feedDoc = xml.dom.minidom.parse(realFeedConfig)
    for categoryNode in feedDoc.getElementsByTagName("category"):
        categoryName = categoryNode.getAttribute("name")
        print categoryName
        if feedtype == "delete":
            print "delete sequence"
            for feedNode in categoryNode.childNodes:
                if feedNode.nodeName == "feed":
                    currentSite = get_child_text(feedNode, "site")
                    #print currentSite + " : " + site
                    if currentSite == site:
                        print "deleting"
                        categoryNode.removeChild(feedNode)
                        break
        if (((categoryName == "PostMo") and (feedtype == "post")) or ((categoryName == "comments") and (feedtype == "comments"))):
            print "category matches type"
            # create the node
            placed_it = False
            feedElem = feedDoc.createElement("feed")
            create_data_child(feedDoc, feedElem, "type", feedtype)
            create_data_child(feedDoc, feedElem, "link", linkElem)
            create_data_child(feedDoc, feedElem, "title", site_title)
            create_data_child(feedDoc, feedElem, "site", site)
            create_data_child(feedDoc, feedElem, "offset", offset)
            for feedNode in categoryNode.childNodes:
                #print "feed node tag name: " + feedNode.nodeName
                if feedNode.nodeName == "feed":
                    currentType = get_child_text(feedNode, "type")
                    currentTitle = get_child_text(feedNode, "title")
                    if currentTitle == site_title:
                        print "Already listed"
                        sys.exit(1)
                    if currentTitle.upper() > site_title.upper() and currentTitle is not "unknown":
                        print "inserting before " + currentTitle
                        categoryNode.insertBefore(feedElem, feedNode)
                        placed_it = True
                        break
                    else:
                        print currentTitle + " < " + site_title
            if placed_it is False:
                print "inserting at end"
                categoryNode.appendChild(feedElem)

    # write out the file
    xmlstring = feedDoc.toxml()
    print "created xml string"
    outputFile = codecs.open(intermediateFeedConfig, "w", "utf-8")
    outputFile.write(xmlstring)
    outputFile.close()
    copy(intermediateFeedConfig, realFeedConfig)
    print "done"
    

def main(argv):
    if len(sys.argv) == 1:
        checkBlogroll()
    else:
        updateBlogroll(argv)

if __name__ == "__main__":
    __file__ = sys.argv[0]
    sys.exit( main(sys.argv) )


