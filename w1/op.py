import os
fd= os.open("file.txt",os.O_RDONLY)
data =  os.read(fd,128)
os.write(1,data)
os.close(fd)
