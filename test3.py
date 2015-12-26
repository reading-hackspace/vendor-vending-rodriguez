#/bin/python
#test3.py resides on ~/pi@vending raspberrypi in vending machine
#MTB 23-Dec-2015
#MTB currently vends using F or G key to B1 or C1 respectively
#code will also read cmd file set at http://134.0.17.81:8080/s4/home.jsp
#and vend B,C or D 1 accordingly
import serial, pycurl, os
from io import BytesIO #for pycurl
from time import sleep
ser = serial.Serial('/dev/ttyACM0', 9600)
ser.flushInput() # Clear the buffer


def vendb():
   sleep(2)
   ser.write(bytes("s",'ascii'))
   sleep(4)
   ser.write(bytes("8",'ascii'))
   sleep(3)
   ser.write(bytes("b",'ascii'))
   sleep(2)
   ser.write(bytes("1",'ascii'))
   sleep(2)
   ser.write(bytes("s",'ascii'))
   sleep(3)
   ser.flushInput() # Clear the buffer
def vendc():
   sleep(2)
   ser.write(bytes("s",'ascii'))
   sleep(4)
   ser.write(bytes("8",'ascii'))
   sleep(3)
   ser.write(bytes("c",'ascii'))
   sleep(2)
   ser.write(bytes("1",'ascii'))
   sleep(2)
   ser.write(bytes("s",'ascii'))
   sleep(3)
   ser.flushInput() # Clear the buffer

def vendd():
   sleep(2)
   ser.write(bytes("s",'ascii'))
   sleep(4)
   ser.write(bytes("8",'ascii'))
   sleep(3)
   ser.write(bytes("d",'ascii'))
   sleep(2)
   ser.write(bytes("1",'ascii'))
   sleep(2)
   ser.write(bytes("s",'ascii'))
   sleep(3)
   ser.flushInput() # Clear the buffer

def sendchar():
  ser.write(bytes(ch,'ascii'))

while True:
# sleep(1)
 ch = ser.read(1)

 if (ch==b'1'):
#  ser.write(bytes("s",'ascii'))
   print("got A")
#look for cmd from server
   buffer = BytesIO()
   c = pycurl.Curl()
   c.setopt(c.URL, "http://134.0.17.81:8080/s4/readvmcmd.jsp")
   c.setopt(c.WRITEDATA, buffer)
   c.perform()
   c.close()

   body = buffer.getvalue()
   print("START:")
   print(body.decode('iso-8859-1'))
   print(":END")
   bodyraw = (body.decode('iso-8859-1'))
   bodych = bodyraw[11:13]
   print("bodych:")
   print(bodych)
   print("bodyraw:")
   print(bodyraw)
   if (bodych=='B1'):
     print("Doing B1")
     vendb() #vend b1
   elif (bodych=='C1'):
     print("Doing C1")
     vendc() #vend c1
   elif (bodych=='D1'):
     print("Doing D1")
     vendd() #vend d1

   ser.flushInput() # Clear the buffer
#clear cmd from server
   buffer = BytesIO()
   c = pycurl.Curl()
   c.setopt(c.URL, "http://134.0.17.81:8080/s4/vendCLR.jsp")
   c.setopt(c.WRITEDATA, buffer)
   c.perform()
   c.close()


#   fname = "vmcmd"
#   dir_path = "/home/wwwdata/"
#   f = open(os.path.join(dir_path, fname), 'r')
#   cmd = f.read()
#   f.close()
#   cmd = cmd.strip()
#   print ("cmd:"+cmd)

 elif (ch==b'f'):
   vendb() #vend b1
 elif (ch==b'g'):
   vendc() #vend c1
 elif (ch==b'h'):
   vendd() #vend d1
# else:
#   sendchar()
#   ser.flushInput() # Clear the buffer

 print("loop")
 print(ch)


ser.close()
