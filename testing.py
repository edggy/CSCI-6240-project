import otp

pad11 = otp.OTP('otp11.pad', 32)
pad11.generate(1024*16)
pad11.copy('otp21.pad')
pad12 = otp.OTP('otp12.pad', 64)
pad12.generate(1024*16)
pad12.copy('otp22.pad')
pad21 = otp.OTP('otp21.pad', 32)
pad22 = otp.OTP('otp22.pad', 64)

msg = 'Hello World!'
length = 32

if len(msg) < length:
	msg += '\0' *  (length - len(msg))
elif len(msg) > length:
	msg = msg[:length]


mac = pad11.mac(msg, length)[0]
enc = pad12.encrypt(msg + mac, length * 2)[0]
dec = pad22.decrypt(enc, length * 2)[0]
print 'msg = %r' % msg
print 'mac = %r' % mac
print 'enc = %r' % enc
print 'dec = %r' % dec
print 'Verify = %r' % pad21.verify(dec[:length], dec[length:], length)[0]


print 'Encrypted text == decrypted text: %r' % (msg[:length] == dec[:length])

msg2 = 'Testing, Testing'
length = 32
enc, pad = pad11.macEncrypt(msg2, length)
#print enc[5], (enc[5] + 1) % 256
#enc[5] = (enc[5] + 1) % 256
dec, pad = pad21.decryptVerify(enc, length)
print 'msg = %r' % msg2
print 'enc = %r' % enc
print 'dec = %r' % dec





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

offset = 17
syncSize = 16
syncSuccess = 8
maxTries = None

import oneTwoOT
import socket
import threading
import thread
from time import sleep

prime = 179424673
f = oneTwoOT.Field(prime)

_lock = threading.Lock()

def printLock(string):
	with _lock:
		print string
		
def inputLock(prompt = None):
	with _lock:
		ret = raw_input(prompt)
	return ret

def alice():
	s1 = socket.socket()
	s1.bind(("localhost",9999))
	s1.listen(10)
	sc, address = s1.accept()
	pad11.setSocket(sc)
	printLock('Connection from: %s:%s' % address)
	if offset > 0: pad11.getBytes(offset)
	syncCount = pad11.resync(0, syncSize, syncSuccess, maxTries)
	#syncCount = 0
	#count = 1
	#while syncCount < syncSuccess: 
		#if count % 2 == 0: pad11.getBytes(syncSize*(count/2))
		#syncCount = pad11.sync(syncSize)
		#count += 1
	printLock('Alice Sync = %s, %s' % syncCount)
	ot = oneTwoOT.OneTwoOT(pad11)
	ot.send('Bit 0', 'Bit 1', '\003', f, 10)
	while not data['done']:
		sleep(1)	
	sc.close()
	s1.close()

def bob():
	s2 = socket.socket()
	s2.connect(("localhost",9999))
	pad21.setSocket(s2)
	if offset < 0: pad21.getBytes(-offset)
	syncCount = pad21.resync(1, syncSize, syncSuccess, maxTries)
	#syncCount = 0
	#count = 1
	#while syncCount < syncSuccess: 
		#if count % 2 == 1: pad21.getBytes(syncSize*((count+1)/2))
		#syncCount = pad21.sync(syncSize)
		#count += 1
	printLock('Bob Sync = %s, %s' % syncCount)
	ot = oneTwoOT.OneTwoOT(pad21)
	b = int(inputLock('Which bit do you want(0/1): '))
	ot_data = ot.recv(b, '\003', f, 10)
	printLock('Bob got: %s' % ot_data)
	data['done'] = True
	s2.close()
	


data = {'done':False}
thread.start_new_thread(alice, ())
thread.start_new_thread(bob, ())

while not data['done']:
	sleep(1)



