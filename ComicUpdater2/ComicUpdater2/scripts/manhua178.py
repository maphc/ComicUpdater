#-*-?encoding:UTF-8?-*-
import re
import string
import datetime
import logging
import os
import logging.config
import sys
from logging import *
import win32com.server.util, win32com.client

#LOG_NAME=fn = string.join([os.getcwd(), os.path.sep, 'py_debug_', datetime.datetime.today().strftime('%Y-%m-%d'), '.log'], '')
#FORMAT='%(asctime)s %(levelname)s %(module)s %(lineno)d %(message)s'
#logging.basicConfig(filename=LOG_NAME,format=FORMAT,level=logging.DEBUG)
logging.config.fileConfig('logging.conf')
USE_PYV8=False

def dealWithPyV8(s,name):
	debug('PyV8 begin analyzePages')
	import PyV8
	with PyV8.JSContext() as ctx:
		debug('PyV8 enter JSContext')
		ctx.enter()
		debug('PyV8 ctx.enter()')
		ctx.eval(s)
		debug('PyV8 ctx.eval(s)')
		pages=ctx.eval(name)
		debug('PyV8 ctx.eval({0})'.format(name))
	del ctx
	del PyV8
	return pages[1:-1].decode('utf8').encode('gbk')
	pass

def dealWithMSJS(s,name):
	
	debug('MSJS begin analyzePages :'+name)
	
	ctx=win32com.client.Dispatch('MSScriptControl.ScriptControl')
	ctx.language = 'JavaScript'
	ctx.allowUI = False
	ctx.eval(s)
	pages=ctx.eval(name)
	
	return pages[1:-1].encode('gbk')
	pass

def analyzePages(s,name):
	if USE_PYV8:
		return dealWithPyV8(s,name)
	else:
		return dealWithMSJS(s,name)
	pass


def getPages(html):
	result=''
	
	try:
		evalstr=re.findall(r'eval.*(?=\n)',html)
		
		if len(evalstr)>0:
			debug('find eval')
			result=analyzePages(evalstr[0],"pages")
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
	debug('return :'+result)
	return result
