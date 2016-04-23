class Field:
	def __init__(self, mod):
		self._mod = mod

	def arrayToInt(self, b):
		'''
		Cast an array of bytes to a number
		'''
		import binascii
		return int(binascii.hexlify(b), 16)

	def intToArray(self, n):
		'''
		Cast a number into an array of bytes
		'''
		s = '%x' % n
		if len(s) & 1:
			s = '0' + s
		return s.decode('hex')    

	def applyFunc(self, func, x, y):
		'''
		@param func - A function to perform on two numbers and a modulus
		@param x - An array of bytes
		@param y - An array of bytes
		'''
		size = max((len(x),len(y)))
		
		# Convert to numbers
		x = self.arrayToInt(x) % self._mod
		y = self.arrayToInt(y) % self._mod
		
		# Apply function
		ret = bytearray(self.intToArray(func(x, y, self._mod) % self._mod))
		if len(ret) < size:
			# Pad to size
			ret = bytearray([0] * (size - len(ret))) + ret
		return ret
	
	def add(self, x, y):
		import gmpy2
		def wrapper(x, y, n):
			return gmpy2.add(x, y)
		# Apply function on gmpy2's add
		return self.applyFunc(wrapper, x, y)

	def sub(self, x, y):
		import gmpy2
		def wrapper(x, y, n):
			return gmpy2.sub(x, y)	
		# Apply function on gmpy2's sub
		return self.applyFunc(wrapper, x, y)

	def mult(self, x, y):
		import gmpy2
		def wrapper(x, y, n):
			return gmpy2.mul(x, y)		
		# Apply function on gmpy2's mul
		return self.applyFunc(wrapper, x, y)   

	def div(self, x, y):
		import gmpy2
		# Apply function on gmpy2's divm
		return self.applyFunc(gmpy2.divm, x, y)    

	def pow(self, x, y):
		import gmpy2
		# Apply function on gmpy2's pow
		return self.applyFunc(gmpy2.powmod, x, y)



class OneTwoOT:
# https://www.youtube.com/watch?v=pIi-YTBBolU    

	def __init__(self, socket):
		self._socket = socket

	def send(self, message0, message1, g, field, n):
		# field.pow is a functon that takes 2 n-byte strings, x, y and outputs x^y 
		# field.div is a functon that takes 2 n-byte strings, x, y and outputs x/y 
		# n = length of message in bytes or size of prime in bytes, whichever is larger
		import os
		import otp
		
		pad = otp.OTP()
		
		# Choose a random n-byte x
		x = os.urandom(n)
		
		# X = g^x mod n
		X = field.pow(g, x)
		
		# Send X to Bob
		self._socket.send(X)
		
		# Recieve Y from Bob
		Y = self._socket.recv(n)
		
		# K0 = Y^x mod n,  K1 = (Y/X)^x mod n
		K0, K1 = field.pow(Y, x), field.pow(field.div(Y, X), x)
		
		# C0 = Enc_K0(message0) ,  C1 = Enc_K1(message1)
		C0, C1 = pad.encrypt(message0, n, K0)[0], pad.encrypt(message1, n, K1)[0]

		# Send C0 and C1 to Bob
		self._socket.send(C0)
		self._socket.send(C1)

	def recv(self, b, g, field, n):
		
		import os
		import otp
		pad = otp.OTP()
		
		# Recieve X from Alice
		X = self._socket.recv(n)
		
		# Choose a random n-byte x
		y = os.urandom(n)
		
		# gy = g^y mod n
		gy = field.pow(g, y)
		
		if b:
			# b = 1, Y = X * gy mod n = X * g^y mod n
			Y = field.mult(X, gy)
		else:
			# b = 0, Y = gy = g^y mod n
			Y = gy
		
		# Send Alice Y
		self._socket.send(Y)
		
		# Kb = X^y mod n
		Kb = field.pow(X, y)

		# Get C0 and C1 from Alice
		C0 = self._socket.recv(n)
		C1 = self._socket.recv(n)
		
		if b:
			# b = 1, Cb = C1
			Cb = C1
		else:
			# b = 0, Cb = C0
			Cb = C0

		
		# return Dec_Kb(Cb)
		return str(pad.decrypt(Cb, n, Kb)[0])