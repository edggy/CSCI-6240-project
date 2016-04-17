import otp

pad11 = otp.OTP('otp11.pad', 32)
pad11.generate(1024)
pad11.copy('otp21.pad')
pad12 = otp.OTP('otp12.pad', 64)
pad12.generate(1024)
pad12.copy('otp22.pad')
pad21 = otp.OTP('otp21.pad', 32)
pad22 = otp.OTP('otp22.pad', 64)

msg = 'Hello World!'
length = 32

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
print 'dec =', dec, len(dec)
print 'Verify =', pad21.verify(dec[:length], dec[length:], length)[0]


print msg[:length] == dec[:length]

msg2 = 'Testing, Testing'
length = 32
enc, pad = pad11.macEncrypt(msg2, length)
#print enc[5], (enc[5] + 1) % 256
#enc[5] = (enc[5] + 1) % 256
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

def alice():
	s1 = socket.socket()
	s1.bind(("localhost",9999))
	s1.listen(10)
	sc, address = s1.accept()
	pad11.setSocket(sc)
	print 'Connection from: %s:%s' % address
	ot = oneTwoOT.OneTwoOT(pad11)
	ot.send('HiXXX', 'HeyXX', '\003', f, 10)
	while not data['done']:
		sleep(1)	
	sc.close()
	s1.close()

def bob():
	s2 = socket.socket()
	s2.connect(("localhost",9999))
	pad21.setSocket(s2)
	ot = oneTwoOT.OneTwoOT(pad21)
	print 'Bob got: %s' % ot.recv(0, '\003', f, 10)
	data['done'] = True
	s2.close()
	
import thread

data = {'done':False}
thread.start_new_thread(alice, ())
thread.start_new_thread(bob, ())

from time import sleep

while not data['done']:
	sleep(1)



