class Gate:
	def __init__(self, name, table, parents = (), children = set()):
		'''
		@param name - The name for this gate
		@param table - A dict of all the valid inputs to their outputs
		@param parents - A tuple of all of the input edges
		@param child - A set of all of the output edges
		'''
		self.name = name
		self._table = table
		self._parents = parents
		self._children = children		
	
	def __str__(self):
		ret = str(self.name)
		for row in self._table:
			rowStr = ''
			for i in row:
				rowStr += str(i) + '\t'
			rowStr += self._table[row]
			ret += '\n' + rowStr
		
		return ret
	
	def __repr__(self):
		return '%s:\t%s\t%s\t%s' % (self.name, repr(self._table), repr(self._parents), repr(self._children)) 
	
	def __call__(self, *args):
		if args not in self._table:
			return None
		return self._table[args]
	
	def setChildren(self, *args):
		output = self(*args)
		for child in self._children:
			child.value = output
	
	def garble(self, n):
		'''
		@param n - the size of the key in bytes
		@return - a garbled gate of this gate
		'''
		
		# a dict from output edge names to a set of (encryped value, (MAC, key)) to be used to get the result dict<edge,set<tup<enc,tup<mac,key>>>>
		encrypedTable = {}
		
		for args in self._table:
			# args is the current tuple of arguments
			
			# encArgs is a list of the encrypted values
			encArgs = []
			for i, arg in enumerate(args):
				# i is the argument number, arg is the argument value
				
				# get the ith parent's arg value
				encArgs.append(self._parents[i].value[arg])
			
			# key is the encryption of all inputs
			key = None
			for arg in encArgs:
				if key is None:
					key = arg
				else:
					key = pad.encrypt(key, n, arg)[0]
			
			# outVal is the value of the output
			outVal = self._table[args]
			
			# Compute the table for each child edge seperately
			for child in self._children:
				
				# The value is the value of the output edge
				value = child.value[outVal]
				
				# The encrypted value is the encryption of the value
				encVal = str(pad.encrypt(value, n, key)[0])
				
				# Generate a random MAC key
				MACkey = os.urandom(n*2)
				
				# Compute the MAC
				mac = str(pad.mac(value, n, MACkey)[0])
				
				# Add this edge to the encrypedTable
				encrypedTable[child.name] = (encVal, (MAC, MACkey))
			
		return GarbledGate(encrypedTable, n)
	
class GarbledGate(Gate):
	def __init__(self, encrypedTable, n):
		'''
		@param table - a dict from output edge names to a set of (encryped value, (MAC, key)) to be used to get the result dict<edge,set<tup<enc,tup<mac,key>>>>
		@param n - the size of the key in bytes
		'''
		Gate.__init__(self, encrypedTable)
		self.n = n
		
	def __call__(self, *args):
		import otp
		pad = otp.OTP()
		
		# key is the encryption of all inputs
		key = None
		for arg in reversed(args):
			if key is None:
				key = arg
			else:
				key = pad.decrypt(key, self.n, arg)[0]
		
		# ret is a dict from child edge names to their values
		ret = {}
		
		for child in self._children:
			result = None
			# Try each encrypted value
			for encVal, MACdata in self._table[child.name]:
				# Get the value of the decryption
				value = pad.decrypt(encVal, self.n, key)[0]
				
				# Use the MACdata to check if the decryption is valid
				if pad.verify(value, MACdata[0], self.n, MACdata[1])[0]:
					# If there is a hit, no need to continue
					result = str(value)
					break
		
			# If we got no result something went wrong...
			if not result:
				raise Exception
			
			ret[child.name] = result
		
		return result
	
	def setChildren(self, *args):
		output = self(*args)
		for child in self._children:
			child.value = output[child.name]
		

class Edge:
	'''
	An edge connecting the output of one gate to the input of another
	'''
	def __init__(self, name, child = None, parent = None, value = None):
		'''
		@param name - The name of the edge
		@param child - The gate that this feeds into
		@param parent - The gate that this came from
		@param value - The value of this edge
		'''
		self.name = name
		self._parent = parent
		self._child = child
		self._value = value
		
	def __str__(self):
		return '%s:\t%s\t%s\t%s' % (self.name, self._parent.name, self._child.name, self._value)
	
	def __repr__(self):
		return '%s:\t%s\t%s\t%s' % (self.name, self._parent.name, self._child.name, self._value)
	
	def garble(self, n):
		return GarbledEdge(self.name, self._child, self._parent, {0:os.urandom(n), 1:os.urandom(n)})

class GarbledEdge(Edge):
	'''
	An edge with garbled values
	'''
	pass

class Circuit:
	def __init__(self, gates, edges):
		'''
		@param gates - A dict of IDs to gates
		@param edges - A dict of IDs to edge
		'''        