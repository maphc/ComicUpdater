import PyV8
import re
import string
import os
import sys
import datetime
import pprint

DEBUG=True

def logFile(s):
	dt=datetime.datetime.today().strftime('%Y-%m-%d_%H-%M-%S')
	if DEBUG==True:
		with open(r'z:\log.txt','a') as f:
			f.write('[{0}][imanhua]:{1}\n'.format(dt,str(s)))
			
	else:
		print s
		
def analyzePages(s):
	logFile("begin analyzePages :"+s);
	ctx=PyV8.JSContext()
	logFile("after JSContext");
	ctx.enter()
	logFile("after enter");
	ctx.eval(s)
	logFile("after eval(s) :"+str(dir(ctx.locals.cInfo)));
	pages=ctx.eval('cInfo.files.toString()')
	cid=ctx.eval('cInfo.cid')
	bid=ctx.eval('cInfo.bid')
	logFile('bid[{0}]cid[{1}]'.format(bid,cid))
	ctx.leave()
	logFile("after leave");
	
	return pages,bid,cid
		
def logHtml(s):
	with open(r'z:\log.html','w') as f:
		f.write(s)
def getPages(html):
	return bak(html)
	
def getResult(s):
	pages,bid,cid=analyzePages(s)
	logFile("after analyzePages")
	l=pages.decode('utf8').encode('gbk').split(',')
	result=','.join([r"http://c4.mangafiles.com/Files/Images/{0}/{1}/{2}".format(bid,cid,i) for i in l])
	return result

def bak(html):
	try:
		logHtml(html);
		#evalstr=re.findall(r'var cInfo=.*?(?=</script>)',html)
		evalstr=re.findall(r'eval\(function.*?(?=</script>)',html)
		if len(evalstr) > 0:
			logFile("eval function find")
			result=getResult(evalstr[0]);
			
		else:
			cInfoStr=re.findall(r'var cInfo=.*?(?=</script>)',html)
			if len(cInfoStr)>0:
				logFile("plaintext js find")
				result=getResult(cInfoStr[0]);
			else:
				logFile("len(evalstr) <= 0")
				#fn=os.getcwd()+os.path.sep+"err_"+datetime.datetime.today().strftime('%Y-%m-%d_%H-%M-%S')+"log"
				fn = string.join([os.getcwd(), os.path.sep, 'err_html_', datetime.datetime.today().strftime('%Y-%m-%d_%H-%M-%S'), '.log'], '')
				with open(fn, 'w') as errf:
					errf.write(html)
				result= ""
	except BaseException as inst:
			logFile("\nexception :"+str(type(inst)))
			logFile("\nexception :"+str(inst.args))
			result=''
	
	return result


if __name__=="__main__":
	with open(r'z:\html.txt','r') as f:
		s=getPages(f.read().decode('gbk'))
		pprint.pprint(s)