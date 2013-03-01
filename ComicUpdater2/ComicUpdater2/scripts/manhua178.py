import PyV8
import re
import string
import os
import sys
import datetime
import pprint

def logFile(s):
	with open(r'z:\log.txt','a') as f:
		f.write(s)
		f.write("\n");
		
def analyzePages(s,name):
	logFile("begin analyzePages");
	ctx=PyV8.JSContext()
	logFile("after JSContext");
	ctx.enter()
	logFile("after enter");
	ctx.eval(s)
	logFile("after eval(s)");
	result=ctx.eval(name)
	logFile("after eval(name)");
	ctx.leave()
	logFile("after leave");
	
	return result
		
def logHtml(s):
	with open(r'z:\log.html','w') as f:
		f.write(s)

def getPages(html):
	logHtml(html);
	evalstr=re.findall(r'eval.*(?=\n)',html)
	logFile("after evalstr")
	if len(evalstr) > 0:
		logFile("len(evalstr) > 0")
		pages=analyzePages(evalstr[0],"pages")
		logFile("after analyzePages")
		return pages[1:-1].decode('utf8').encode('gbk')
		
	else:
		logFile("len(evalstr) <= 0")
		#fn=os.getcwd()+os.path.sep+"err_"+datetime.datetime.today().strftime('%Y-%m-%d_%H-%M-%S')+"log"
		fn = string.join([os.getcwd(), os.path.sep, 'err_html_', datetime.datetime.today().strftime('%Y-%m-%d_%H-%M-%S'), '.log'], '')
		with open(fn, 'w') as errf:
			errf.write(html)
		return ""


def bak(html):
	with open(r'z:\log.txt','w') as f:
		f.write("begin manhua178.getPages")

		try:
			evalstr=re.findall(r'eval.*(?=\n)',html)
			f.write("\n manhua178.getPages evalstr :"+repr(evalstr))
			if len(evalstr)>0:
				f.write("\n manhua178.getPages len(evalstr)>0")
				pages=analyzePages(evalstr[0],"pages")
				f.write("\n manhua178.getPages finish succeed")
				return pages[1:-1].decode('utf8').encode('gbk')
			else:
				f.write("\n manhua178.getPages len(evalstr)<=0")
				#fn=os.getcwd()+os.path.sep+"err_"+datetime.datetime.today().strftime('%Y-%m-%d_%H-%M-%S')+"log"
				fn=string.join([os.getcwd(),os.path.sep,'err_html_',datetime.datetime.today().strftime('%Y-%m-%d_%H-%M-%S'),'.log'],'')
				with open(fn,'w') as errf:
					errf.write(html)
				return ""
			
		except BaseException as inst:
			f.write("\nexception :"+str(type(inst)))
			f.write("\nexception :"+str(inst.args))

if __name__=="__main__":
	with open(r'z:\log.txt','r') as f:
		s=getPages(f.read())
		pprint.pprint(s)