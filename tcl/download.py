import sys
import os

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

if mode == 0:
	cmd = "data2mem.bat -bm " + currDict + appName + "/system_wrapper_hw_platform_0/system_wrapper_bd.bmm "
	cmd = cmd + "-bt " + currDict + appName + "/system_wrapper_hw_platform_0/system_wrapper.bit "
	#Host
	cmd = cmd + "-bd " + currDict + appName + "/Host/Debug/Host.elf "
	cmd = cmd + "tag system_i_Host "
	#Slaves
	for i in range(0,numGroup):
		for j in range(0,numSlave):
			each = "Slave"
			processor = "Slave"+str(j)+"_Group"+str(i)
			cmd = cmd + "-bd " + currDict + appName + "/" + each + "/Debug/" + each + ".elf "
			cmd = cmd + "tag system_i_" + processor + " "
	cmd = cmd + "-o b "	+ currDict + appName + "/system_wrapper_hw_platform_0/download.bit"	
	os.system(cmd)
	tclArg = currDict+appName
	cmd = "vivado -mode batch -source ./program.tcl -tclargs " + tclArg
	os.system(cmd)
	quit()
##########################################
#Create download.bit
##########################################
cmd = "data2mem.bat -bm " + currDict + appName + "/system_wrapper_hw_platform_0/system_wrapper_bd.bmm "
cmd = cmd + "-bt " + currDict + appName + "/system_wrapper_hw_platform_0/system_wrapper.bit "
#Host
cmd = cmd + "-bd " + currDict + appName + "/Host/Debug/Host.elf "
cmd = cmd + "tag system_i_Host "
#Salves
for i in range(0,numGroup):
	for j in range(0,numSlave):
		each = "Slave"+str(j)+"_Group"+str(i)
		cmd = cmd + "-bd " + currDict + appName + "/" + each + "/Debug/" + each + ".elf "
		cmd = cmd + "tag system_i_" + each + " "
cmd = cmd + "-o b "	+ currDict + appName + "/system_wrapper_hw_platform_0/download.bit"	
os.system(cmd)
tclArg = currDict+appName
cmd = "vivado -mode batch -source ./program.tcl -tclargs " + tclArg
os.system(cmd)