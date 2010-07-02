#!/usr/bin/python

import sys
import libxml2

doc = libxml2.parseFile( sys.argv[1] )
ctxt = doc.xpathNewContext()
ctxt.xpathRegisterNs('xhtml', 'http://www.w3.org/1999/xhtml')
#print ctxt.xpathEval('//xhtml:div[@id="maincol"]')[0].content
print ctxt.xpathEval('//xhtml:div[@id="wikicontent"]')[0]


