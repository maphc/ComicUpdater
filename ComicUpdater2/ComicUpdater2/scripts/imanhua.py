import PyV8
import re
import string
import os
import sys
import datetime
import pprint
from logging import *
import logging.config

#DEBUG=True
#datetime.datetime.today().strftime('%Y-%m-%d_%H-%M-%S')

#LOG_NAME=fn = string.join([os.getcwd(), os.path.sep, 'py_debug_', datetime.datetime.today().strftime('%Y-%m-%d'), '.log'], '')
#FORMAT='%(asctime)s %(levelname)s %(module)s %(lineno)d %(message)s'
#logging.basicConfig(filename=LOG_NAME,format=FORMAT,level=logging.DEBUG)
logging.config.fileConfig('logging.conf')


		
def analyzePages(s):
	debug("begin analyzePages ");
	ctx=PyV8.JSContext()
	debug("after JSContext");
	ctx.enter()
	debug("after enter");
	ctx.eval(s)
	debug("after eval(s) ");
	pages=ctx.eval('cInfo.files.toString()')
	cid=ctx.eval('cInfo.cid')
	bid=ctx.eval('cInfo.bid')
	debug('bid[{0}]cid[{1}]'.format(bid,cid))
	ctx.leave()
	debug("after leave");
	del ctx
	return pages,bid,cid
		
def logHtml(s):
	with open(r'z:\log.html','w') as f:
		f.write(s)
	
def getResult(s):
	pages,bid,cid=analyzePages(s)
	debug("after analyzePages")
	l=pages.decode('utf8').encode('gbk').split(',')
	result=','.join([r"http://c4.mangafiles.com/Files/Images/{0}/{1}/{2}".format(bid,cid,i) for i in l])
	return result

def getPages(html):
	try:
		#logHtml(html);
		#evalstr=re.findall(r'var cInfo=.*?(?=</script>)',html)
		evalstr=re.findall(r'eval\(function.*?(?=</script>)',html)
		if len(evalstr) > 0:
			debug("eval function find")
			result=getResult(evalstr[0]);
			
		else:
			cInfoStr=re.findall(r'var cInfo=.*?(?=</script>)',html)
			if len(cInfoStr)>0:
				debug("plaintext js find")
				result=getResult(cInfoStr[0]);
			else:
				debug("len(evalstr) <= 0")
				#fn=os.getcwd()+os.path.sep+"err_"+datetime.datetime.today().strftime('%Y-%m-%d_%H-%M-%S')+"log"
				fn = string.join([os.getcwd(), os.path.sep, 'err_html_', datetime.datetime.today().strftime('%Y-%m-%d_%H-%M-%S'), '.log'], '')
				with open(fn, 'w') as errf:
					errf.write(html)
				result= ""
	except BaseException as inst:
			debug("\nexception [{0}][{1}]".format( str(type(inst)) , str(inst.args) ))
			result=''
	
	return result


if __name__=="__main__":
	with open(r'z:\html.txt','r') as f:
		s=getPages(f.read().decode('gbk'))
		pprint.pprint(s)