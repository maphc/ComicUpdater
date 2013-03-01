import PyV8
import re

def analyzePages(s,name):
	ctx=PyV8.JSContext()
	ctx.enter()
	ctx.eval(s)
	
	return ctx.eval(name)

def getPages(html):
	//s=open(r'z:\178.html','r').read()
	//print "new"
	with open(r'z:\test.html','wb') as f:
		f.write(html)
	evalstr=re.findall(r'eval.*(?=\n)',html)
	if len(evalstr)>0:
		pages=analyzePages(evalstr[0],"pages")
		print pages[1:len(pages)-1]
	else:
		print "Can't find evalstr"
