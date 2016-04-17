class Field:
	def __init__(self, mod):
		self._mod = mod

	def arrayToInt(self, b):
		import binascii
		return int(binascii.hexlify(b), 16)

	def intToArray(self, n): 
		s = '%x' % n
		if len(s) & 1:
			s = '0' + s
		return s.decode('hex')    

	def applyFunc(self, func, x, y):
		size = max((len(x),len(y)))
		x = self.arrayToInt(x) % self._mod
		y = self.arrayToInt(y) % self._mod
		#print 'x = %s, y = %s' % (x, y)
		ret = bytearray(self.intToArray(func(x, y, self._mod) % self._mod))
		if len(ret) < size:
			ret = bytearray([0] * (size - len(ret))) + ret
		return ret

	#def __getattr__(self, name):
		
		#op = operator.__getattribute__(name)
		#return 
	
	def add(self, x, y):
		import gmpy2
		def wrapper(x, y, n):
			return gmpy2.add(x, y)
		return self.applyFunc(wrapper, x, y)

	def sub(self, x, y):
		import gmpy2
		def wrapper(x, y, n):
			return gmpy2.sub(x, y)		
		return self.applyFunc(wrapper, x, y)

	def mult(self, x, y):
		import gmpy2
		def wrapper(x, y, n):
			return gmpy2.mul(x, y)			
		return self.applyFunc(wrapper, x, y)   

	def div(self, x, y):
		import gmpy2
		return self.applyFunc(gmpy2.divm, x, y)    

	def pow(self, x, y):
		import gmpy2
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
		x = os.urandom(n)
		
		X = field.pow(g, x)
		#print 'Alice:\tx = %s, X = %s' % (x, X)
		#print 'Alice:\tx = %s, X = %s' % (field.arrayToInt(x), field.arrayToInt(X))
		self._socket.send(X)

		Y = self._socket.recv(n)
		K0, K1 = field.pow(Y, x), field.pow(field.div(Y, X), x)

		pad = otp.OTP()
		C0, C1 = pad.encrypt(message0, n, K0)[0], pad.encrypt(message1, n, K1)[0]

		#print 'Alice:\tY = %s, K0 = %s, K1 = %s, C0 = %s, C1 = %s' % tuple([field.arrayToInt(i) for i in (Y, K0, K1, C0, C1)])
		self._socket.send(C0)
		self._socket.send(C1)

	def recv(self, b, g, field, n):
		
		import os
		import otp
		X = self._socket.recv(n)
		#print 'Bob:\tX = %s' % X
		y = os.urandom(n)
		gy = field.pow(g, y)
		if b:
			Y = field.mult(X, gy)
		else:
			Y = gy

		#print 'Bob:\tX = %s, y = %s, gy = %s, Y = %s' % tuple([field.arrayToInt(i) for i in (X, y, gy, Y)])
		self._socket.send(Y)
		Kb = field.pow(X, y)

		C0 = self._socket.recv(n)
		C1 = self._socket.recv(n)
		if b:
			Cb = C1
		else:
			Cb = C0

		pad = otp.OTP()
		#print 'Bob:\tKb = %s, C0 = %s, C1 = %s' % tuple([field.arrayToInt(i) for i in (Kb, C0, C1)])
		return pad.decrypt(Cb, n, Kb)[0]