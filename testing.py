import otp

pad11 = otp.OTP('otp11.pad')
pad11.generate(1024)
pad11.copy('otp21.pad')
pad12 = otp.OTP('otp1.pad')
pad12.generate(1024)
pad12.copy('otp22.pad')
pad21 = otp.OTP('otp21.pad')
pad22 = otp.OTP('otp22.pad')

msg = 'Hello World!'
length = 10

if len(msg) < length:
	msg += '\0' *  (length - len(msg))
elif len(msg) > length:
	msg = msg[:length]

print 'msg =', msg
mac = pad11.mac(msg, length)[0]
print 'mac =', mac
enc = pad12.encrypt(msg + mac, length * 2)[0]
print 'enc =', enc
dec = pad22.decrypt(enc, length * 2)[0]
print 'dec =', dec
print 'Verify =', pad21.verify(dec[:length], dec[length:], length)[0]


print msg[:length] == dec[:length]

msg2 = 'Testing, Testing, 1, 2, 3'
length = 30
enc, pad = pad11.macEncrypt(msg2, length)
print enc[5], (enc[5] + 1) % 256
enc[5] = (enc[5] + 1) % 256
dec, pad = pad21.decryptVerify(enc, length)
print 'msg =', msg2
print 'enc =', enc
print 'dec =', dec

#import binascii

#def hexint(b):
	#return int(binascii.hexlify(b), 16)

#def dump(n): 
	#s = '%x' % n
	#if len(s) & 1:
		#s = '0' + s
	#return s.decode('hex')

#num = hexint(msg2)
#print num
#print dump(num)

# Must get gmpy2 to run the oneTwoOT
# python -m pip install gmpy2

import oneTwoOT
import socket
prime = 179424673
f = oneTwoOT.Field(prime)
print prime
print 2**(8*3)
print 2**(8*4)
print 2**(8*5)
def alice():
	s1 = socket.socket()
	s1.bind(("localhost",9999))
	s1.listen(10)
	sc, address = s1.accept()
	#pad11.setSocket(sc)
	print 'Connection from: %s:%s' % address
	ot = oneTwoOT.OneTwoOT(sc)
	ot.send('HiXXX', 'HeyXX', '\003', f, 10)
	while not data['done']:
		sleep(1)	
	sc.close()
	s1.close()

def bob():
	s2 = socket.socket()
	s2.connect(("localhost",9999))
	#pad12.setSocket(s2)
	ot = oneTwoOT.OneTwoOT(s2)
	print 'Bob got: %s' % ot.recv(1, '\003', f, 10)
	data['done'] = True
	s2.close()
	
import thread

data = {'done':False}
thread.start_new_thread(alice, ())
thread.start_new_thread(bob, ())

from time import sleep

while not data['done']:
	sleep(1)



