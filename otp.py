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
		
		self.overwrites = 1
		

	def getBytes(self, num, delete = True):
		'''
		gets and returns the last num bytes of a file, deletes them if delete is True
		'''
		with open(self._pad, "rb+") as f:
			import os
			f.seek(-num, os.SEEK_END)
			res = bytearray(reversed(f.read()))
			if delete:
				for i in range(self.overwrites):
					f.seek(-num, os.SEEK_END)
					garbage = os.urandom(num)
					f.write(garbage)
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
		pad = bytearray(pad)
		msgLen = len(msg)
		if msgLen > length:
			# Trim msg to length
			msg = msg[:length]
		elif msgLen < length:
			# Pad with zeros
			msg += bytearray([0] * (length - msgLen))
			
		# Apply the function
		return (bytearray(func(msg, pad)), unusedPad)

	def encrypt(self, msg, length = None, pad = None, delete = True):
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
		return self.applyFunc(msg, xor, length, pad, delete)

	def decrypt(self, cipherText, length = None, pad = None, delete = True):
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
				
		return self.encrypt(cipherText, length, pad, delete)


	def mac(self, msg, length = None, pad = None, delete = True):
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
			pad = self.getBytes(length * 2, delete)	
			
		andop = lambda a, b: map(lambda x, y: x & y, a, b)
		orop = lambda a, b: map(lambda x, y: x | y, a, b)
		
		inter, pad = self.applyFunc(msg, andop, length, pad, delete)
		return self.applyFunc(inter, orop, length, pad, delete)

	def verify(self, msg, mac, length = None, pad = None, delete = True):
		if length is None:
			if self._length:
				length = self._length				
			else:
				# Default length is the length of the message
				length = len(msg)
		if pad is None:
			# Default pad is the last bytes of the file
			pad = self.getBytes(length * 2, delete)	
			
		if len(msg) < length:
			msg += '\0' *  (length - len(msg))
		elif len(msg) > length:
			msg = msg[:length]			
				
		calcMac, pad = self.mac(msg, length, pad, delete)
		return (calcMac == mac, pad)

	def macEncrypt(self, msg, length = None, pad = None, delete = True):
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
			pad = self.getBytes(length * 4, delete)			
				
		
		mac, pad = self.mac(msg, length, pad, delete)			# Uses 0 to 2 * length
		enc = self.encrypt(msg + mac, length * 2, pad, delete)		# Uses 2 * length to 4 * length
		return enc
		
		
	
	def decryptVerify(self, cipherText, length = None, pad = None, delete = True):
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
			pad = self.getBytes(length * 4, delete)
		
		savedPad, pad = pad[:length*2], pad[length*2:]
		
		dec, pad = self.decrypt(cipherText, length * 2, pad, delete)	# Need to use 2 * length to 4 * length
		msg, mac = dec[:length], dec[length:length*2]
		vrfy = self.verify(msg, mac, length, savedPad, delete)[0]		# Need to use 0 to 2 * length
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
	
	
	def sync(self, stringLength = 16, delete = True, flags = 0):
		import socket
		if not self._socket:
			raise socket.error
		
		sent = self.getBytes(stringLength, delete)
		
		sentLen = 0
		while sentLen < stringLength:
			sentLen = self._socket.send(sent[sentLen:], flags)
		
		if sentLen != stringLength:
			raise socket.error
		
		recvd = self._socket.recv(stringLength, flags)
		
		def commonOverlap(text1, text2):
			'''
			'Have some CoCo and CoCo'
			          'CoCo and CoCo is here.'
			-> 10
			'''
			text1 = str(text1)
			text2 = str(text2)		
			
			# Cache the text lengths to prevent multiple calls.  
			text1_length = len(text1)  
			text2_length = len(text2)  
			
			# Eliminate the null case.  
			if text1_length == 0 or text2_length == 0:  
				return None
		
			if text1 == text2:
				return 0
		
			queue = []
			for n, c in enumerate(text1):
				if c == text2[0]:
					queue.append(n)
					
			for index in queue:
				pattern = text1[index:]  
				found = text2.find(pattern)  
				if found >= 0:
					return index
			queue = []
			for n, c in enumerate(text2):
				if c == text1[0]:
					queue.append(n)

			for index in queue:
				pattern = text2[index:]  
				found = text1.find(pattern)  
				if found >= 0:
					return -index			
			return None		
		
		overlap = commonOverlap(sent, recvd)
		# sent  = 0,1,2,3,4,5,6   -> remove 1   ->    8,9,10,...
		# recvd = 1,2,3,4,5,6,7   -> nothing    ->    8,9,10,...
		# overlap = 1, len(sen)-len(rec) = 0  
		
		# sent  = 3,4,5,6,7,8   -> nothing    ->    9,10,11,...
		# recvd = 1,2,3,4,5,6   -> remove 2   ->    9,10,11,...
		# overlap = None
		
		# sent  = 3,4,5,6,7,8   -> nothing    ->    9,10,11,...
		# recvd = 0,1,2,3,4     -> remove 4   ->    9,10,11,...	
		# overlap = None
		
		# sent  = 0,1,2,3,4     -> remove 4   ->    9,10,11,...	
		# recvd = 3,4,5,6,7,8   -> nothing    ->    9,10,11,...
		# overlap = 3, len(sen)-len(rec) = 1
		
		# sent  = 0,1,2         -> remove 2   ->    5,6,7,8,9,10,11,...
		# recvd = 1,2,3,4       -> nothing    ->    5,6,7,8,9,10,11,...	
		# overlap = 1, len(sen)-len(rec) = -1
		
		# sent  = 0,1,2,3,4       -> nothing    ->    5,6,7,8,9,10,...
		# recvd = 1,2,3,4         -> remove 1   ->    5,6,7,8,9,10,...
		# overlap = 1, len(sen)-len(rec) = 1
		
		
		#  sent  = 1,2,3,4         -> remove 1   ->    5,6,7,8,9,10,...
		#  recvd = 0,1,2,3,4       -> nothing    ->    5,6,7,8,9,10,...
		# overlap = None, len(sen)-len(rec) = -1		
		
		# overlap = 1, len(sen)-len(rec) = 0 
		# overlap = 3, len(sen)-len(rec) = 1
		# overlap = 1, len(sen)-len(rec) = -1
		# overlap = None, len(sen)-len(rec) = -3
		
		# sent  = 0,1,2,3,4     -> nothing    ->    5,6,7,8,9,10,11,...
		# recvd = 0,1,2,3,4     -> nothing    ->    5,6,7,8,9,10,11,...	
		# overlap = 0, len(sen)-len(rec) = 0, nothing	
		
		# sent  = 0,1,2,3       -> remove 1   ->    5,6,7,8,9,10,11,...
		# recvd = 0,1,2,3,4     -> nothing    ->    5,6,7,8,9,10,11,...	
		# overlap = 0, len(sen)-len(rec) = -1, remove len(sen)-len(rec)
		
		# sent  = 0,1,2,3,4     -> nothing    ->    5,6,7,8,9,10,11,...
		# recvd = 0,1,2,3       -> remove 1   ->    5,6,7,8,9,10,11,...	
		# overlap = 0, len(sen)-len(rec) = 1, nothing			
		
		# sent  = 0,1,2,3       -> remove 1   ->    5,6,7,8,9,10,11,...
		# recvd = 1,2,3,4       -> nothing    ->    5,6,7,8,9,10,11,...	
		# overlap = 1, len(sen)-len(rec) = 0, remove overlap
		
		# sent  = 0,1,2         -> remove 2   ->    5,6,7,8,9,10,11,...
		# recvd = 1,2,3,4       -> nothing    ->    5,6,7,8,9,10,11,...	
		# overlap = 1, len(sen)-len(rec) = -1, remove overlap - (len(sen)-len(rec))
		
		# sent  = 0,1,2,3,4     -> nothing    ->    5,6,7,8,9,10,11,...
		# recvd = 1,2,3,4       -> nothing    ->    5,6,7,8,9,10,11,...	
		# overlap = 1, len(sen)-len(rec) = 1, remove overlap - (len(sen)-len(rec))
		
		# sent  = 1,2,3,4       -> nothing    ->    5,6,7,8,9,10,11,...
		# recvd = 0,1,2,3       -> remove 1   ->    5,6,7,8,9,10,11,...	
		# overlap = -1, len(sen)-len(rec) = 0, nothing	
		
		# sent  = 1,2,3,4       -> nothing    ->    5,6,7,8,9,10,11,...
		# recvd = 0,1,2,3,4     -> nothing    ->    5,6,7,8,9,10,11,...	
		# overlap = -1, len(sen)-len(rec) = -1, remove overlap - (len(sen)-len(rec))		
		
		# sent  = 1,2,3,4       -> nothing    ->    5,6,7,8,9,10,11,...
		# recvd = 0,1,2         -> remove 2   ->    5,6,7,8,9,10,11,...	
		# overlap = -1, len(sen)-len(rec) = 1, nothing	
		
		# overlap = 0, len(sen)-len(rec) = 0, nothing	
		# overlap = 0, len(sen)-len(rec) = 1, nothing			
		# overlap = 1, len(sen)-len(rec) = 1, nothing
		# overlap = None, len(sen)-len(rec) = 0, nothing	
		# overlap = None, len(sen)-len(rec) = -1, nothing			
		# overlap = None, len(sen)-len(rec) = 1, nothing	
		# overlap = 0, len(sen)-len(rec) = -1, remove 1	
		# overlap = 1, len(sen)-len(rec) = 0, remove 1
		# overlap = 1, len(sen)-len(rec) = -1, remove 2			
		
		if overlap is None:
			return 0
		
		remove = overlap - (len(sent)-len(recvd))
		common = min([len(sent),len(recvd)]) - abs(overlap)
		
		if remove > 0:
			trash = self.getBytes(remove, delete)
			
		return common
			
			
	
			
			
			
			
		
		
		
		

