try:
	import xml.etree.cElementTree as ET
except ImportError:
	import xml.etree.ElementTree as ET
import sys
import os
import xml.dom.minidom

########################################
#Check Input Error
########################################
if len(sys.argv) != 5:
	print "Input Error!"
	print "Usage:"
	print "xx.py <Build mode: Homogeneous 0 / Heterogeneous 1> <Application Name> <Number of Groups> <Number of Slaves per Group>"
	quit()

mode 		= int(sys.argv[1])
appName		= sys.argv[2]
numGroup	= int(sys.argv[3])
numSlave	= int(sys.argv[4])

currDict	= os.getcwd()+"/"
currDict	= currDict[10:len(currDict)]
currDict	= currDict[0]+":"+currDict[1:len(currDict)]
#while currDict.find("/") >=0:
#	currDict = currDict[0:currDict.find("/")]+"\\"+currDict[(currDict.find("/")+1):len(currDict)]
if os.path.exists(appName):
	print "Application "+appName+" exists."
	quit()
os.makedirs(appName)

##########################################
#Modify hcl.h
##########################################
f			= open("./src/HCL/hcl_config.h","r+")
lineList	= f.readlines()
f.close()
f			= open("./src/HCL/hcl_config.h","w+")
for i in range(0,len(lineList)):
	if lineList[i].find("define NUMOFLOCALCORE") >= 0:
		lineList[i] = "#define NUMOFLOCALCORE "+str(numSlave)+"\r\n"
	if lineList[i].find("define NUMOFGROUP") >= 0:
		lineList[i] = "#define NUMOFGROUP "+str(numGroup)+"\r\n"
f.writelines(lineList)
f.close()

if mode == 0:
	WS 			= "./"+appName
	scriptFile	= appName+"/"+appName+"_script.xml"

	scrProject	= ET.Element("project")
	scrProject.set("name","SDK Script")
	scrProject.set("default","main")
	scrTarget	= ET.SubElement(scrProject,"target")
	scrTarget.set("name","main")
	#Create Hw Project
	scrCreateHwProject	= ET.SubElement(scrTarget,"createHwProject")
	scrCreateHwProject.set("projname","system_wrapper_hw_platform_0")
	scrCreateHwProject.set("hwspecpath",currDict+"system_wrapper.hdf")
	#Create Host bsp and app
	scrCreateBspProject	= ET.SubElement(scrTarget,"createBspProject")
	scrCreateBspProject.set("projname","Host_bsp")
	scrCreateBspProject.set("hwprojname","system_wrapper_hw_platform_0")
	scrCreateBspProject.set("processor","Host")
	scrCreateBspProject.set("os","standalone")
	srcCreateAppProject	= ET.SubElement(scrTarget,"createAppProject")
	srcCreateAppProject.set("projname","Host")
	srcCreateAppProject.set("hwprojname","system_wrapper_hw_platform_0")
	srcCreateAppProject.set("processor","Host")
	srcCreateAppProject.set("os","standalone")
	srcCreateAppProject.set("bspprojname","Host_bsp")
	srcCreateAppProject.set("template","Empty Application")
	srcCreateAppProject.set("language","C")	
	#Create Slave bsp and app
	scrCreateBspProject	= ET.SubElement(scrTarget,"createBspProject")
	each		= "Slave"
	processor	= "Slave1_Group0"
	scrCreateBspProject.set("projname",each+"_bsp")
	scrCreateBspProject.set("hwprojname","system_wrapper_hw_platform_0")
	scrCreateBspProject.set("processor",processor)
	scrCreateBspProject.set("os","standalone")
	srcCreateAppProject	= ET.SubElement(scrTarget,"createAppProject")
	srcCreateAppProject.set("projname",each)
	srcCreateAppProject.set("hwprojname","system_wrapper_hw_platform_0")
	srcCreateAppProject.set("processor",processor)
	srcCreateAppProject.set("os","standalone")
	srcCreateAppProject.set("bspprojname",each+"_bsp")
	srcCreateAppProject.set("template","Empty Application")
	srcCreateAppProject.set("language","C")		
	#Write SDK Script 
	scrTree = ET.ElementTree(scrProject)
	scrTree.write(scriptFile, encoding='utf-8')
	dom=xml.dom.minidom.parse(scriptFile)
	f			= file(scriptFile,"w+")
	f.write(dom.toprettyxml())
	f.close()
	#Create SDK Project
	sdkCmd		= "xsdk.bat -wait -script "+scriptFile+" -workspace "+appName
	os.system(sdkCmd)
	#Modify lscript.ld
	secListHostLocal	= [".text",".init",".fini",".ctors",".dtors",".rodata",".sdata2",".sbss2",".data",".got",".got1",".got2",".eh_frame",".jcr",".gcc_except_table",".sdata",".sbss",".tdata",".tbss",".bss"]
	secListHostGlobal	= [".heap",".stack"]
	secListSlaveLocal	= [".text",".init",".fini",".ctors",".dtors",".rodata",".sdata2",".sbss2",".data",".got",".got1",".got2",".eh_frame",".jcr",".gcc_except_table",".sdata",".sbss",".tdata",".tbss",".bss",".heap",".stack"]
	#Host
	ldFile		= appName+"/Host/src/lscript.ld"
	f			= open(ldFile,"r+")
	lineList	= f.readlines()
	f.close()
	f			= open(ldFile,"w+")
	shSize		= 10
	shSize		= shSize*1024*1024
	for i in range(0,len(lineList)):
		if lineList[i].find("_STACK_SIZE") == 0:
			lineList[i] = lineList[i][0:lineList[i].find("0x")]+hex(shSize)+lineList[i][lineList[i].find(";"):len(lineList[i])]
		if lineList[i].find("_HEAP_SIZE") == 0:
			lineList[i] = lineList[i][0:lineList[i].find("0x")]+hex(shSize)+lineList[i][lineList[i].find(";"):len(lineList[i])]
		if lineList[i][0] == ".":
			currSec = lineList[i][0:lineList[i].find(" ")]
			if currSec in secListHostLocal:
				while lineList[i][0] != "}": i = i+1
				lineList[i] = "} > "+"Host_local_memory_ilmb_bram_if_cntlr_Host_local_memory_dlmb_bram_if_cntlr"
			if currSec in secListHostGlobal:
				while lineList[i][0] != "}": i = i+1
				lineList[i] = "} > "+"GlobalMem"
	f.writelines(lineList)			
	f.close()
	#Slaves
	each = "Slave"
	ldFile = appName+"/"+each+"/src/lscript.ld"
	f = open(ldFile,"r+")
	lineList = f.readlines()
	f.close()
	f = open(ldFile,"w+")
	for l in range(0,len(lineList)):
		if lineList[l][0] == ".":
			currSec = lineList[l][0:lineList[l].find(" ")]
			if currSec in secListSlaveLocal:
				while lineList[l][0] != "}": l = l+1
				lineList[l] = "} > "+processor+"_local_memory_ilmb_bram_if_cntlr_"+processor+"_local_memory_dlmb_bram_if_cntlr"
	f.writelines(lineList)
	f.close()	
	#Add source file link
	#Host
	proFile		= appName+"/Host/.project"
	root		= ET.parse(proFile).getroot()
	linkedResources = ET.SubElement(root,"linkedResources")
	link		= ET.SubElement(linkedResources,"link")
	name		= ET.SubElement(link,"name")
	name.text	= "src/host_"+appName+".c"
	type		= ET.SubElement(link,"type")
	type.text	= "1"
	location	= ET.SubElement(link,"location")
	location.text	= currDict+"src\\"+appName+"\\host_"+appName+".c"
	string		= ET.tostring(root)
	string 		= xml.dom.minidom.parseString(string)
	f			= open(proFile,"w+")
	f.write(string.toprettyxml())
	f.close()	
	#Slave
	each = "Slave"
	proFile = appName+"/"+each+"/.project"
	root		= ET.parse(proFile).getroot()
	linkedResources = ET.SubElement(root,"linkedResources")
	link		= ET.SubElement(linkedResources,"link")
	name		= ET.SubElement(link,"name")
	name.text	= "src/kernel_"+appName+".c"
	type		= ET.SubElement(link,"type")
	type.text	= "1"
	location	= ET.SubElement(link,"location")
	location.text	= currDict+"src\\"+appName+"\\kernel_"+appName+".c"
	string		= ET.tostring(root)
	string		= xml.dom.minidom.parseString(string)
	f			= open(proFile,"w+")
	f.write(string.toprettyxml())
	f.close()
	#Build SDK Projects
	sdkCmd		= "xsdk.bat -wait -eclipseargs -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -build all -data "+appName+" -vmargs -Dorg.eclipse.cdt.core.console=org.eclipse.cdt.core.systemConsole"
	os.system(sdkCmd)
	quit()


########################################
#Create Application XML File
########################################
WS 			= "./"+appName
scriptFile	= appName+"/"+appName+"_script.xml"

scrProject	= ET.Element("project")
scrProject.set("name","SDK Script")
scrProject.set("default","main")
scrTarget	= ET.SubElement(scrProject,"target")
scrTarget.set("name","main")
#Create Hw Project
scrCreateHwProject	= ET.SubElement(scrTarget,"createHwProject")
scrCreateHwProject.set("projname","system_wrapper_hw_platform_0")
scrCreateHwProject.set("hwspecpath",currDict+"system_wrapper.hdf")
#Create Host bsp and app
scrCreateBspProject	= ET.SubElement(scrTarget,"createBspProject")
scrCreateBspProject.set("projname","Host_bsp")
scrCreateBspProject.set("hwprojname","system_wrapper_hw_platform_0")
scrCreateBspProject.set("processor","Host")
scrCreateBspProject.set("os","standalone")
srcCreateAppProject	= ET.SubElement(scrTarget,"createAppProject")
srcCreateAppProject.set("projname","Host")
srcCreateAppProject.set("hwprojname","system_wrapper_hw_platform_0")
srcCreateAppProject.set("processor","Host")
srcCreateAppProject.set("os","standalone")
srcCreateAppProject.set("bspprojname","Host_bsp")
srcCreateAppProject.set("template","Empty Application")
srcCreateAppProject.set("language","C")
#Create Slave bsp and app
for i in range(0,numGroup):
	for j in range(0,numSlave):
		scrCreateBspProject	= ET.SubElement(scrTarget,"createBspProject")
		each = "Slave"+str(j)+"_Group"+str(i)
		scrCreateBspProject.set("projname",each+"_bsp")
		scrCreateBspProject.set("hwprojname","system_wrapper_hw_platform_0")
		scrCreateBspProject.set("processor",each)
		scrCreateBspProject.set("os","standalone")
		srcCreateAppProject	= ET.SubElement(scrTarget,"createAppProject")
		srcCreateAppProject.set("projname",each)
		srcCreateAppProject.set("hwprojname","system_wrapper_hw_platform_0")
		srcCreateAppProject.set("processor",each)
		srcCreateAppProject.set("os","standalone")
		srcCreateAppProject.set("bspprojname",each+"_bsp")
		srcCreateAppProject.set("template","Empty Application")
		srcCreateAppProject.set("language","C")		
#Write SDK Script 
scrTree = ET.ElementTree(scrProject)
scrTree.write(scriptFile, encoding='utf-8')
dom=xml.dom.minidom.parse(scriptFile)
f			= file(scriptFile,"w")
f.write(dom.toprettyxml())
f.close()

########################################
#Create SDK Projects for Host and Slaves
########################################
sdkCmd		= "xsdk.bat -wait -script "+scriptFile+" -workspace "+appName
os.system(sdkCmd)
#Modify lscript.ld
secListHostLocal	= [".text",".init",".fini",".ctors",".dtors",".rodata",".sdata2",".sbss2",".data",".got",".got1",".got2",".eh_frame",".jcr",".gcc_except_table",".sdata",".sbss",".tdata",".tbss",".bss"]
secListHostGlobal	= [".heap",".stack"]
secListSlaveLocal	= [".text",".init",".fini",".ctors",".dtors",".rodata",".sdata2",".sbss2",".data",".got",".got1",".got2",".eh_frame",".jcr",".gcc_except_table",".sdata",".sbss",".tdata",".tbss",".bss",".heap",".stack"]
#Host
ldFile		= appName+"/Host/src/lscript.ld"
f			= open(ldFile,"r+")
lineList	= f.readlines()
f.close()
f			= open(ldFile,"w+")
shSize		= 10
shSize		= shSize*1024*1024
for i in range(0,len(lineList)):
	if lineList[i].find("_STACK_SIZE") == 0:
		lineList[i] = lineList[i][0:lineList[i].find("0x")]+hex(shSize)+lineList[i][lineList[i].find(";"):len(lineList[i])]
	if lineList[i].find("_HEAP_SIZE") == 0:
		lineList[i] = lineList[i][0:lineList[i].find("0x")]+hex(shSize)+lineList[i][lineList[i].find(";"):len(lineList[i])]
	if lineList[i][0] == ".":
		currSec = lineList[i][0:lineList[i].find(" ")]
		if currSec in secListHostLocal:
			while lineList[i][0] != "}": i = i+1
			lineList[i] = "} > "+"Host_local_memory_ilmb_bram_if_cntlr_Host_local_memory_dlmb_bram_if_cntlr"
		if currSec in secListHostGlobal:
			while lineList[i][0] != "}": i = i+1
			lineList[i] = "} > "+"GlobalMem"
f.writelines(lineList)			
f.close()			
#Slaves		
for i in range(0,numGroup):
	for j in range(0,numSlave):
		each = "Slave"+str(j)+"_Group"+str(i)
		ldFile = appName+"/"+each+"/src/lscript.ld"
		f = open(ldFile,"r+")
		lineList = f.readlines()
		f.close()
		f = open(ldFile,"w+")
		for l in range(0,len(lineList)):
			if lineList[l][0] == ".":
				currSec = lineList[l][0:lineList[l].find(" ")]
				if currSec in secListSlaveLocal:
					while lineList[l][0] != "}": l = l+1
					lineList[l] = "} > "+each+"_local_memory_ilmb_bram_if_cntlr_"+each+"_local_memory_dlmb_bram_if_cntlr"
		f.writelines(lineList)
		f.close()
#Add source file link
#Host
proFile		= appName+"/Host/.project"
root		= ET.parse(proFile).getroot()
linkedResources = ET.SubElement(root,"linkedResources")
link		= ET.SubElement(linkedResources,"link")
name		= ET.SubElement(link,"name")
name.text	= "src/host_"+appName+".c"
type		= ET.SubElement(link,"type")
type.text	= "1"
location	= ET.SubElement(link,"location")
location.text	= currDict+"src\\"+appName+"\\host_"+appName+".c"
string		= ET.tostring(root)
string 		= xml.dom.minidom.parseString(string)
f			= open(proFile,"w+")
f.write(string.toprettyxml())
f.close()
#Slaves
for i in range(0,numGroup):
	for j in range(0,numSlave):
		each = "Slave"+str(j)+"_Group"+str(i)
		proFile = appName+"/"+each+"/.project"
		root		= ET.parse(proFile).getroot()
		linkedResources = ET.SubElement(root,"linkedResources")
		link		= ET.SubElement(linkedResources,"link")
		name		= ET.SubElement(link,"name")
		name.text	= "src/kernel_"+appName+".c"
		type		= ET.SubElement(link,"type")
		type.text	= "1"
		location	= ET.SubElement(link,"location")
		location.text	= currDict+"src\\"+appName+"\\kernel_"+appName+".c"
		string		= ET.tostring(root)
		string		= xml.dom.minidom.parseString(string)
		f			= open(proFile,"w+")
		f.write(string.toprettyxml())
		f.close()
#Build all apps
sdkCmd		= "xsdk.bat -wait -eclipseargs -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -build all -data "+appName+" -vmargs -Dorg.eclipse.cdt.core.console=org.eclipse.cdt.core.systemConsole"
os.system(sdkCmd)
