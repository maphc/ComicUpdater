#-*-?encoding:UTF-8?-*-
import re
import string
import datetime
import logging
import os
import logging.config
import sys
from logging import *


#LOG_NAME=fn = string.join([os.getcwd(), os.path.sep, 'py_debug_', datetime.datetime.today().strftime('%Y-%m-%d'), '.log'], '')
#FORMAT='%(asctime)s %(levelname)s %(module)s %(lineno)d %(message)s'
#logging.basicConfig(filename=LOG_NAME,format=FORMAT,level=logging.DEBUG)
logging.config.fileConfig('logging.conf')


def getPages(html):
	debug("manhua178_s begin")
	result=''
	
	try:
		evalstr=re.findall(r'eval.*(?=\n)',html)
		
		if len(evalstr)>0:
			debug('find eval')
			result=evalstr[0]
			debug('pages length :{0}'.format( len(result) ))
			#result= pages[1:-1].decode('utf8').encode('gbk')
			
		else:
			debug('len(evalstr)<=0')
			#fn=os.getcwd()+os.path.sep+"err_"+datetime.datetime.today().strftime('%Y-%m-%d_%H-%M-%S')+"log"
			fn=string.join([os.getcwd(),os.path.sep,'err_html_',datetime.datetime.today().strftime('%Y-%m-%d_%H-%M-%S'),'.log'],'')
			with open(fn,'w') as f:
				f.write(html)
			result= ""
	except :
		info=sys.exc_info()
		error('exception :{0}-{1}'.format( info[0], info[1]  ),exc_info=1)	
	return result
