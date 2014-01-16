import re
import string
import os
import sys
import datetime
import pprint
from logging import *
import logging.config
import base64
#import chardet

#DEBUG=True
#datetime.datetime.today().strftime('%Y-%m-%d_%H-%M-%S')

#LOG_NAME=fn = string.join([os.getcwd(), os.path.sep, 'py_debug_', datetime.datetime.today().strftime('%Y-%m-%d'), '.log'], '')
#FORMAT='%(asctime)s %(levelname)s %(module)s %(lineno)d %(message)s'
#logging.basicConfig(filename=LOG_NAME,format=FORMAT,level=logging.DEBUG)
logging.config.fileConfig('logging.conf')


def getPics(h):
	abc='kkk'
    #return base64.decodestring(m[0])
    return ''
		
svrListJs=r"http://img_v1.dm08.com/img_v1/cn_130713.js"