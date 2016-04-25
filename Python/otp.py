class VerifyException(Exception):
	pass

class NotEnoughKeyException(Exception):
	pass

import operator
strop = lambda op: lambda a, b: str(bytearray(map(lambda x, y: op(x, y), bytearray(a), bytearray(b))))
xorop = strop(operator.xor)
andop = strop(operator.and_)
orop = strop(operator.or_)


class OTP:
	def __init__(self, filename = None, defaultLength = None, socket = None,  overwrites = 1):
		'''
		@param filename - The file to use as the key
		@param defaultLength - The default length to use if none is supplied (If None the length used is the length of the message)
		@param socket - The socket to transmit encrypted messages over
		@param overwrites - The number of times the data should be overwritten (0 means the key won't be overwritten)
		'''
		if filename:
			import os
			self._pad = os.path.realpath(filename)
		else:
			self._pad = None
			
		self._length = defaultLength
		self._socket = socket
		
		self.overwrites = overwrites
		
	def setPad(self, filename):
		'''
		Sets a new key file
		
		@param filename - The file to use as the key
		'''
		import os
		self._pad = os.path.realpath(filename)
	
	def setSocket(self, socket):
		'''
		Sets a socket for the send and recv functions
		
		@param socket - The socket to use
		'''
		self._socket = socket	

	def getBytes(self, num, delete = True):
		'''
		Gets and returns the last num bytes of a file, deletes them if self.delete is True, and overwrites the bytes self.overwrites times
		
		@param num - The number of bytes to retrive
		@param delete - Whether or not to delete the retrived bytes, THIS IS UNSECURE if False
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
	
	def deleteBytes(self, num):
		with open(self._pad, "rb+") as f:
			import os
			for i in range(self.overwrites):
				f.seek(-num, os.SEEK_END)
				garbage = os.urandom(num)
				f.write(garbage)
			f.seek(-num, os.SEEK_END)
			f.truncate()

	def _getLength(self, msg, length):
		if length is None:
			if self._length:
				return self._length				
			else:
				# Default length is the length of the message
				return len(msg)		
		return length
	
	def _cleanMsg(self, msg, length):
		if len(msg) > length:
			# Trim msg to length
			msg = msg[:length]
		elif len(msg) < length:
			# Pad with zeros
			msg += '\0' * (length - len(msg))
		return msg
	
	def _getKey(self, key, length, delete = True):
		if key is None:
			# Default pad is the last bytes of the file
			return (self.getBytes(length, delete), None)
		else:
			# Check we have enough key
			if len(key) < length:
				raise NotEnoughKeyException()

			# break the key into the used and unused parts
			return (key[:length], key[length:])
		
	def applyFunc(self, msg, func, length, key = None, delete = True):
		'''
		Applies a given function to a message using the key
		
		@param msg - A string to apply the function to
		@param func - A function that takes two strings (the message and the key) that will be be applied to the message and the key
		@param length - The length of the output
		@param key - The key to be used in the function.  If None, the bytes from the end of the file are used
		@param delete - Whether or not to delete the otp bytes, THIS IS UNSECURE if False
		
		@return - A tuple of the resulting value with any unused key, the unused key is None if key = None
		
		Note:
		- The message will be truncated or padded to be of proper length
		- Will delete the bytes taken from the end of the file if key is None and delete is True
		'''
		
		length = self._getLength(msg, length)
		
		msg = self._cleanMsg(msg, length)
		
		key, unusedKey = self._getKey(key, length, delete)
			
		# Apply the function
		ret = (func(msg, key), unusedKey)
		return ret

	def encrypt(self, msg, length = None, key = None, delete = True):
		'''
		Encrypts the mesage using the One-Time-Pad
		
		@param msg - A string of the message to encrypt
		@param length - The length of the output.  Defaults to the length of the message if None and defaultLength was not supplied
		@param key - The key to use to encrypt the message.  Default to using the file
		@param delete - Whether or not to delete the otp bytes, THIS IS UNSECURE if False
		
		@return - A tuple containing the encrypted message with any unused key, the unused key is None if key = None
		'''		
		
		# Just use the applyFunc with xor
		return self.applyFunc(msg, xorop, length, key, delete)

	def decrypt(self, cipherText, length = None, key = None, delete = True):			
		'''
		Encrypts the mesage using the One-Time-Pad
		
		@param cipherText - A string of the encrypted message
		@param length - The length of the output.  Defaults to the length of the cipherText if None and defaultLength was not supplied
		@param key - The key to use to decrypt the message.  Default to using the file
		@param delete - Whether or not to delete the otp bytes, THIS IS UNSECURE if False
		
		@return - A tuple containing the decrypted message with any unused key, the unused key is None if key = None
		'''		
		# Decryption is the same as encryption
		return self.encrypt(cipherText, length, key, delete)

	def mac(self, msg, length = None, key = None, delete = True):
		'''
		Computes a MAC of the message
		
		@param msg - A string of the message to get the MAC of
		@param length - The length of the output.  Defaults to the length of the message if None and defaultLength was not supplied
		@param key - The key to use to get the MAC of the message.  Default to using the file.  Must be at least 2 * length
		@param delete - Whether or not to delete the otp bytes, THIS IS UNSECURE if False
		
		@return - A tuple containing the MAC of the message with any unused key, the unused key is None if key = None
		'''
		
		# Get the length to use
		length = self._getLength(msg, length)
		
		# Clean the message	
		msg = self._cleanMsg(msg, length)			
		
		# Get the key and seperate the unused part
		key, unusedKey = self._getKey(key, length * 2, delete)
			
		# Calculate the intermediate result, bitwise 'and' the message and the first part of the key
		inter, key = self.applyFunc(msg, andop, length, key)
		
		# Calculate the MAC, bitwise 'or' the message and the second part of the key
		res, key = self.applyFunc(inter, orop, length, key)
		
		# Assert we used all the key
		assert key == ''
		
		return (res, unusedKey)

	def verify(self, msg, mac, length = None, key = None, delete = True):
		'''
		Verifies a MAC of the message
		
		@param msg - A string of the message to verify the MAC of
		@param mac - The MAC to check against
		@param length - The length of the output.  Defaults to the length of the message if None and defaultLength was not supplied
		@param key - The key to use to get the MAC of the message.  Default to using the file.  Must be at least 2 * length
		@param delete - Whether or not to delete the otp bytes on success, THIS IS UNSECURE if False
		
		@return - A tuple containing whether the verification was successful with any unused key, the unused key is None if key = None
		'''		
		# Get the length to use
		length = self._getLength(msg, length)
		
		# Clean the message	
		msg = self._cleanMsg(msg, length)			
		
		# Get the key and seperate the unused part
		key, unusedKey = self._getKey(key, length * 2, False)		
		
		# Calculate the MAC
		calcMac, key = self.mac(msg, length, key)
		
		# Assert we used all the key
		assert key == ''
		
		# Store the result
		res = (calcMac == mac)
		
		# If successful, delete is True and we used the pad, delete the bytes
		if res and delete and unusedKey is None:
			self.deleteBytes(length * 2)
		
		return (calcMac == mac, unusedKey)

	def macEncrypt(self, msg, length = None, key = None, delete = True):
		'''
		Compute the MAC and encrypt the entire message
		
		@param msg - A string of the message to MAC and encrypt
		@param length - The length of the output.  Defaults to the length of the message if None and defaultLength was not supplied
		@param key - The key to use to encrypt the message.  Default to using the file.  Must be at least 4 * length
		@param delete - Whether or not to delete the retrived bytes, THIS IS UNSECURE if False
		
		@return - A tuple containing the encrypted message + MAC digest with any unused key, the unused key is None if key = None
		'''
		# Get the length to use
		length = self._getLength(msg, length)
		
		# Clean the message	
		msg = self._cleanMsg(msg, length)			
		
		# Get the key and seperate the unused part	
		key, unusedKey = self._getKey(key, length * 4, delete)		
				
		# First get the MAC
		mac, key = self.mac(msg, length, key)				# Uses 0 to 2 * length
		
		# Them encrypt the digest
		enc, key = self.encrypt(msg + mac, length * 2, key)		# Uses 2 * length to 4 * length

		# Assert we used all the key
		assert key == ''
		
		return (enc, unusedKey)
		
		
	def decryptVerify(self, cipherText, length = None, key = None, delete = True):	
		'''
		Decrypts the entire message and verifies its integerty

		@param cipherText - A string of the encrypted message
		@param length - The length of the output.  Defaults to the length of the message if None and defaultLength was not supplied
		@param key - The key to use to decrypt and verify the message.  Default to using the file.  Must be at least 4 * length
		@param delete - Whether or not to delete the otp bytes on success, THIS IS UNSECURE if False

		@return - A tuple containing the decrypted message with any unused key, the decrypted message is None if verification fails, the unused key is None if key = None
		'''	
		
		# Get the length to use
		length = self._getLength(cipherText, length)					
		
		# Get the key and seperate the unused part
		key, unusedKey = self._getKey(key, length * 4, False)
		
		# Break the key into the one for decryption and the one for verification
		savedPad, key = key[:length*2], key[length*2:]
		
		# First decrypt the digest.  Note: Don't forget to use the second part of the key first!
		dec, key = self.decrypt(cipherText, length * 2, key)		# Need to use 2 * length to 4 * length
		
		# Then seperate the message from the MAC
		msg, mac = dec[:length], dec[length:length*2]
		
		# Finally verify the message.  Note: Don't forget to use the first part of the key second!
		vrfy, key = self.verify(msg, mac, length, savedPad)		# Need to use 0 to 2 * length
		
		# Assert we used all the key
		assert key == ''
		
		if vrfy:
			# If successful, delete is True and we used the pad, delete the bytes
			if delete and unusedKey is None:
				self.deleteBytes(length * 4)	
				
			return (msg, unusedKey)
		else:
			return (None, unusedKey)	

	def generate(self, length, filename = None):
		'''
		Generates a random file
		
		@param length - The amount of data in bytes to generate
		@param filename - The file to write to. Defaults to the original pad file
		'''
		import os
		
		if filename is None:
			filename = self._pad
		data = os.urandom(length)
		
		with open(filename, "wb") as f:
			f.write(data)

	def copy(self, filename):
		'''
		Creates a copy of the pad
		
		@param filename - The file to copy to
		'''
		import shutil
		shutil.copy2(self._pad, filename)
		
	def send(self, data, flags = 0):
		'''
		Send data across a secure channel. The socket must be connected to a remote socket. The optional flags argument has the same meaning as for recv() above. Returns the number of bytes sent. Applications are responsible for checking that all data has been sent; if only some of the data was transmitted, the application needs to attempt delivery of the remaining data. For further information on this concept, consult the Socket Programming HOWTO.
		
		@param data - The data to send across a secure channel
		'''
		if not self._socket:
			import socket
			raise socket.error
		
		# Create the digest
		enc = self.macEncrypt(data, self._length)[0]
		
		# Send it across
		return self._socket.send(enc, flags)
	
	def recv(self, bufsize = None, flags = 0):	
		'''
		Receive data from a secure channel. The return value is a string representing the data received. The maximum amount of data to be received at once is specified by bufsize. See the Unix manual page recv(2) for the meaning of the optional argument flags; it defaults to zero.
		
		@param bufsize - The amount of data to recive from a secure channel
		@except VerifyException - Raised if verification failed
		'''
		
		if not self._socket:
			import socket
			raise socket.error('Error No Socket')
		
		# Make the buffer at least as long as the incoming ciphertext
		if bufsize is None or bufsize < self._length * 2:
			bufsize = self._length * 2
		
		# Recieve the ciphertext
		cipherText = self._socket.recv(bufsize, flags)
		
		# Decrypt the ciphertext
		dec, key = self.decryptVerify(cipherText, self._length)
		
		if dec is None:
			raise VerifyException('Verification Failed')
		
		return dec
	
	
	def sync(self, stringLength = 16, flags = 0):
		'''
		Syncronizes two keys across a network
		
		@param stringLength - The length of the chunk of the key to send across to check
		@flags - the same meaning as for recv()
		
		@return The number of bytes the two pads had in common
		'''
		import socket
		if not self._socket:
			raise socket.error
		
		sent = self.getBytes(stringLength)
		
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
			trash = self.getBytes(remove)
			
		return common
			
	def resync(self, identity, syncSize = 16, syncSuccess = 8, maxTries = None, flags = 0):
		'''
		Tries to sync several times in order to align the keys
		
		@param identity - 0 or 1, ensure it is the opposite of the other party's
		@param syncSize - The length of the chunk of the key to send across to check
		@param syncSuccess - The minimum number of common bytes to consider a sucessful sync
		@param maxTries - The maximum number of tries to sync
		@flags - the same meaning as for recv()
		
		@return A tuple of the final differnce between syncCount - syncSuccess (Negative on failure) and the number of bytes used
		'''
		syncCount = 0
		count = 1
		bytesUsed = 0
		while syncCount < syncSuccess and (count <= maxTries or not maxTries): 
			if count % 2 == identity: 
				self.getBytes(syncSize*((count+identity)/2))
				bytesUsed += syncSize*((count+identity)/2)
			syncCount = self.sync(syncSize)
			bytesUsed += syncSize
			count += 1
			
		return (syncCount - syncSuccess, bytesUsed)
	
	def beat(self, sendQueue, recvQueue, role, period = .25):
		import time	
		import socket
		
		if not self._socket:
			raise socket.error
		
		self._socket.settimeout(period)
		
		start = time.time()
		lastTime = 0
		turnToSend = 0
		
		data = None
		
		
		while True:
			now = time.time()
			# Check if at least one period transpired
			if now - lastTime >= period:
				# Check to see if it is our turn to send
				if turnToSend == role:
					# Check to see if we need more data to send
					if data is None:
						try:
							data = sendQueue.get_nowait()
						except:
						
							try:
								data = sendQueue.pop()
							except:
								try:
									data = sendQueue.read(self._length)
								except:
									# No data, send null string
									data = ''
									
						#print 'Sending: %r' % data
						data = self.macEncrypt(data)[0]
					try:
						#print 'Sending: %r' % data
						# Try to send the data
						self._socket.send(data)
						
						lastTime = now
						
						# Set role to recieve
						turnToSend = 1 - role
					except socket.timeout:
						# On error resend next beat
						pass
					
				else:
					# Our turn to recieve
					try:
						# Try to recieve the data
						data = self.recv(self._length, flags=0)
						lastTime = now
						
						if data != '\0'*self._length:
							try:
								# Put the data in the recived queue
								recvQueue.put(data, True, period)
							except:
								try:
									recvQueue.append(data)
								except:
									try:
										recvQueue.write()
									except:
										pass
						
						# Set data to none since we don't need to resend
						data = None
					except (socket.timeout, VerifyException):
						pass
					# Set role to send
					turnToSend = role
					
			
		
	def heartbeat(self, sendQueue, recvQueue, role, period = .25):
		import thread
		
		thread.start_new_thread(self.beat, (sendQueue, recvQueue, role, period))
		
		return recvQueue
	        
	
			
			
			
			
		
		
		
		

