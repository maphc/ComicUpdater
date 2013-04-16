import PyV8
import re
import urllib
import pprint
import string
import datetime

def analyzePages(s,name):
	ctx=PyV8.JSContext()
	ctx.enter()
	ctx.eval(s)
	result=ctx.eval(name)
	ctx.leave()
	
	return result

def getPages(html):
	
	evalstr=re.findall(r'eval.*(?=\n)',html)
		
	if len(evalstr)>0:
		pages=analyzePages(evalstr[0],"pages")
		return pages[1:-1].decode('utf8').encode('gbk')
	else:
		#fn=os.getcwd()+os.path.sep+"err_"+datetime.datetime.today().strftime('%Y-%m-%d_%H-%M-%S')+"log"
		fn=string.join([os.getcwd(),os.path.sep,'err_html_',datetime.datetime.today().strftime('%Y-%m-%d_%H-%M-%S'),'.log'],'')
		with open(fn,'w') as f:
			f.write(html)
		return ""
