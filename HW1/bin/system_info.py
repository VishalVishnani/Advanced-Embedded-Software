import subprocess
import platform
import sys

with open ("/proc/version","r") as myfile:
	data=myfile.readlines()


data1=''.join(data)
data2=data1.split(" ")

file=open("system_info.txt","w")
file.write("Operating System Type: ")
file.write(data2[0])

file.write("\n\nOperating System Distribution: ")
file.write(data1[78:84])


file.write("\n\nOperating System Version: ")
file.write(data2[2])

file.write("\n\nKernel GCC Version: ")
file.write(data2[4])
file.write(" ")
file.write(data2[5])
file.write(" ")
file.write(data2[6])

file.write("\n\nKernel Build Time: ")
file.write(data2[13])
file.write(" ")
file.write(data2[14])
file.write(" ")
file.write(data2[15])
file.write(" ")
file.write(data2[16])
file.write(" ")
file.write(data2[17])
file.write(" ")
file.write(data2[18])

a=platform.uname()
a1=' '.join(a)
file.write("\nInformation on the kernel: ")
file.write(a1)

file.write("\n\nSystem Architecture Information: ")
file.write(a[4])
file.write(" ")
file.write(sys.byteorder)


file.write("\n\nCPU information: \n")
m=subprocess.Popen("lscpu",stdout=subprocess.PIPE)
n=m.stdout.read()
file.write(n)
