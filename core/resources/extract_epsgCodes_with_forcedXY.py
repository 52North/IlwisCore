#! /usr/bin/env python
import re
import os

# Definition taken from
#
#   http://www.geotoolkit.org/modules/referencing/supported-codes.html
#
# download file and use extract_epsgCodes_with_forcedXY.py to extract
# http://www.geotoolkit.org/modules/referencing/supported-codes.html'


# extract pattern 
# <tr><td class="nospace">?</td><td><code>EPSG:2044</code>
regex = re.compile(
	r"""^
	(.*\?).*
	(?P<code><code>(?!<del>).*</code>)
	""",re.VERBOSE)
	
outputfile = "codes_with_latlon_order.csv";
with open("epsg_forced_xy.txt") as input:
	with open(outputfile,"a") as output:
		output.truncate()
		output.write("# generated file via extract_epsgCodes_with_forcedXY.py \n");
		for line in input:
			match = regex.match(line)
			if match:
				#print(match)
				s = match.group("code")
				code = s[6:len(s)-7]
				output.write(code + "\n")
		