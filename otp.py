class VerifyException(Exception):
	pass

class OTP:
	def __init__(self, filename = None, defaultLength = None, socket = None):
		if filename:
			import os
			self._pad = os.path.realpath(filename)
		else:
			self._pad = None
			
		self._length = defaultLength
		self._socket = socket
		

	def getBytes(self, num, delete = True):
		'''
		gets and returns the last num bytes of a file, deletes them if delete is True
		'''
		with open(self._pad, "rb+") as f:
			import os
			f.seek(-num, os.SEEK_END)
			res = bytearray(reversed(f.read()))
			if delete: 
				f.seek(-num, os.SEEK_END)
				f.truncate()
		return res    

	def applyFunc(self, msg, func, length, pad = None, delete = True):	
		if pad is None:
			# Default pad is the last bytes of the file
			pad = self.getBytes(length, delete)
			unusedPad = None
		else:
			unusedPad = pad[length:]
			if len(unusedPad) <= 0:
				unusedPad = None
			# Trim pad to length
			pad = pad[:length]
		
		msg = bytearray(msg)
		msgLen = len(msg)
		if msgLen > length:
			# Trim msg to length
			msg = msg[:length]
		elif msgLen < length:
			# Pad with zeros
			msg += bytearray([0] * (length - msgLen))
			
		# Apply the function
		return (bytearray(func(msg, pad)), unusedPad)

	def encrypt(self, msg, length = None, pad = None):
		if length is None:
			if self._length:
				length = self._length				
			else:
				# Default length is the length of the message
				length = len(msg)
		if len(msg) < length:
			msg += '\0' *  (length - len(msg))
		elif len(msg) > length:
			msg = msg[:length]		
				
		xor = lambda a, b: map(lambda x, y: x ^ y, a, b)
		return self.applyFunc(msg, xor, length, pad)

	def decrypt(self, cipherText, length = None, pad = None):
		if length is None:
			if self._length:
				length = self._length			
			else:
				# Default length is the length of the message
				length = len(cipherText)
		if len(cipherText) < length:
			cipherText += '\0' *  (length - len(cipherText))
		elif len(cipherText) > length:
			cipherText = cipherText[:length]			
				
		return self.encrypt(cipherText, length, pad)


	def mac(self, msg, length = None, pad = None):
		if length is None:
			if self._length:
				length = self._length			
			else:
				# Default length is the length of the message
				length = len(msg)
				
		if len(msg) < length:
			msg += '\0' *  (length - len(msg))
		elif len(msg) > length:
			msg = msg[:length]			
				
		if pad is None:
			# Default pad is the last bytes of the file
			pad = self.getBytes(length * 2)	
			
		andop = lambda a, b: map(lambda x, y: x & y, a, b)
		orop = lambda a, b: map(lambda x, y: x | y, a, b)
		# MAC(msg) = (msg & OTP1) | OTP2
		# inter = msg & OTP1
		# MAC(msg) = inter | OTP2
		#if pad:
			## If pad is not none
			#try:
				## Treat it as a bytearray
				#pad = (pad[:length], pad[length:])
			#except:
				## If not, assume it is a tuple of byte arrays
				#pass
		#else:
			#pad = (None, None)
		inter, pad = self.applyFunc(msg, andop, length, pad)
		return self.applyFunc(inter, orop, length, pad)

	def verify(self, msg, mac, length = None, pad = None):
		if length is None:
			if self._length:
				length = self._length				
			else:
				# Default length is the length of the message
				length = len(msg)
		if pad is None:
			# Default pad is the last bytes of the file
			pad = self.getBytes(length * 2)	
			
		if len(msg) < length:
			msg += '\0' *  (length - len(msg))
		elif len(msg) > length:
			msg = msg[:length]			
				
		calcMac, pad = self.mac(msg, length, pad)
		return (calcMac == mac, pad)

	def macEncrypt(self, msg, length = None, pad = None):
		if length is None:
			if self._length:
				length = self._length			
			else:
				# Default length is the length of the message
				length = len(msg)
		if len(msg) < length:
			msg += '\0' *  (length - len(msg))
		elif len(msg) > length:
			msg = msg[:length]	
			
		if pad is None:
			# Default pad is the last bytes of the file
			pad = self.getBytes(length * 4)			
				
		
		mac, pad = self.mac(msg, length, pad)			# Uses 0 to 2 * length
		enc = self.encrypt(msg + mac, length * 2, pad)		# Uses 2 * length to 4 * length
		return enc
		
		
	
	def decryptVerify(self, cipherText, length = None, pad = None):
		# macEncrypt(msg) = encrypt(msg:mac(msg))
		# macEncrypt(msg) = (msg:mac(msg)) ^ OTP3
		# macEncrypt(msg) = (msg:((msg & OTP1) | OTP2)) ^ OTP3
		#xor = lambda a, b: map(lambda x, y: x ^ y, a, b)
		#andop = lambda a, b: map(lambda x, y: x & y, a, b)
		#orop = lambda a, b: map(lambda x, y: x | y, a, b)		
	
		if length is None:
			if self._length:
				length = self._length * 2			
			else:
				# Default length is the length of the message
				length = len(cipherText)   					
				
		if pad is None:
			# Default pad is the last bytes of the file
			pad = self.getBytes(length * 4)
		
		savedPad, pad = pad[:length*2], pad[length*2:]
		
		dec, pad = self.decrypt(cipherText, length * 2, pad)	# Need to use 2 * length to 4 * length
		msg, mac = dec[:length], dec[length:length*2]
		vrfy = self.verify(msg, mac, length, savedPad)[0]		# Need to use 0 to 2 * length
		if vrfy:
			return (msg, pad)
		else:
			return (None, pad)



	def generate(self, length):
		import os
		data = os.urandom(length)
		with open(self._pad, "wb") as f:
			f.write(data)

	def copy(self, filename):
		import shutil
		shutil.copy2(self._pad, filename)
		
	def setSocket(self, socket):
		self._socket = socket
		
	def send(self, data, flags = 0):
		'''
		Send data to the socket. The socket must be connected to a remote socket. The optional flags argument has the same meaning as for recv() above. Returns the number of bytes sent. Applications are responsible for checking that all data has been sent; if only some of the data was transmitted, the application needs to attempt delivery of the remaining data. For further information on this concept, consult the Socket Programming HOWTO.
		'''
		if not self._socket:
			import socket
			raise socket.error
		
		enc = self.macEncrypt(data, self._length)[0]
		return self._socket.send(enc, flags)
	
	def recv(self, bufsize = None, flags = 0):	
		'''
		Receive data from the socket. The return value is a string representing the data received. The maximum amount of data to be received at once is specified by bufsize. See the Unix manual page recv(2) for the meaning of the optional argument flags; it defaults to zero.
		'''
		if not self._socket:
			import socket
			raise socket.error('Error No Socket')
		
		if bufsize is None or bufsize < self._length:
			bufsize = self._length * 2
		
		cipherText = self._socket.recv(bufsize, flags)
		dec = self.decryptVerify(cipherText, self._length)
		
		if dec[0] is None:
			raise VerifyException('Verification Failed')
		
		return dec[0]

