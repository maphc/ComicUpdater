#-*-?encoding:UTF-8?-*-
import re
import string
import datetime
import logging
import os
import logging.config
import sys
from logging import *
import base64

#LOG_NAME=fn = string.join([os.getcwd(), os.path.sep, 'py_debug_', datetime.datetime.today().strftime('%Y-%m-%d'), '.log'], '')
#FORMAT='%(asctime)s %(levelname)s %(module)s %(lineno)d %(message)s'
#logging.basicConfig(filename=LOG_NAME,format=FORMAT,level=logging.DEBUG)
logging.config.fileConfig('logging.conf')


def getPics(html):
	try:
		#debug("xxbh_s begin: "+html)
		lines=html.split('\n')
		debug('lines: %d'%(len(lines),));
		for line in lines:
			m=re.findall(r'var qTcms_S_m_murl_e="([^"]*)"', line)
			if len(m)>0:
				debug('find: '+m[0])
				break
		result=','.join(base64.decodestring(m[0]).split('$qingtiandy$'))
	except :
		info=sys.exc_info()
		error('exception :{0}-{1}'.format( info[0], info[1]  ),exc_info=1)	
	debug('return :'+result)
	return result