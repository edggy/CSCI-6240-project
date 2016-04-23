class Gate:
	def __init__(self, func):
		'''
		@param func - a function that this gate evaluates
		'''
		self._func = func
		self._cache = {}
		self.parents = set([])
		self.children = set([])

	def __call__(self, *args):
		if args not in self._cache:
			self._cache[args] = self._func(*args)
		return self._cache[args]
	
class TableGate(Gate):
	'''
	A gate that takes a dict of valid inputs as tuples and their corrisponding output
	'''
	def __init__(self, table):
		'''
		@param func - a function that this gate evaluates
		'''
		self._table = table
		self.parents = set([])
		self.children = set([])
		
	def __call__(self, *args):
		if args not in self._table:
			return None
		return self._table[args]	
	
class GarbledGate(TableGate):
	def __init__(self, table):
		'''
		@param func - a function that this gate evaluates
		'''
		TableGate.__init__(self, table)
		self._cache = {}

	def __call__(self, *args):
		if args not in self._cache:
			self._cache[args] = self.evalGarbGate(*args)
		return self._cache[args]
	
	def evalGarbGate(self, *args):
		import otp
		pad = otp.OTP()
		key = None
		for arg in args:
			print '%r' % arg
			if key is None:
				key = arg
			else:
				key = pad.decrypt(key, n, arg)[0]
		outputs = {}
		print '%r' % key
		for childID in self._table:
			for output in self._table[childID]:
				value = pad.decrypt(output[0], n, key)[0]
				print 'output[0] = %r\t key = %r\t value = %r' % (output[0], key, value)
				if pad.verify(value, output[1][0], n, output[1][1])[0]:
					outputs[childID] = str(value)
					break
			if childID not in outputs:
				raise Exception
			
		return outputs	
	
	

class Edge:
	def __init__(self, parent = None, child = None, value = None):
		self.parent = parent
		self.child = child
		self.value = value

class Circuit:
	def __init__(self, gates, edges):
		'''
		@param gates - A dict of IDs to gates
		@param edges - A dict of IDs to edge
		'''
		self._edges = edges
		self._gates = gates
		for edgeID in self._edges:
			edge = self._edges[edgeID]
			if edge.parent is not None:
				self._gates[edge.parent].children.add(edgeID)
			if edge.child is not None:
				self._gates[edge.child].parents.add(edgeID)
		


	def evaluate(self, inputs, outputs):
		'''
		@param inputs - A map of ID to their initial states
		@param outputs - A set of ID of outputs
		'''
		from collections import deque
		
		result = {}
		
		queue = deque()
		for edgeID in inputs:
			edgeValue = inputs[edgeID]
			self._edges[edgeID].value = edgeValue
			queue.appendleft(edgeID)
			
		while len(queue) > 0:
			edgeID = queue.pop()
			edge = self._edges[edgeID]
			if edgeID in outputs:
				result[edgeID] = edge.value
			
			if edge.child is not None:
				vals = []
				child = self._gates[edge.child]
				for parentEdgeID in child.parents:
					parent = self._edges[parentEdgeID]
					vals.append(parent.value)
				if None not in vals:
					value = child(*vals)
					for childEdgeID in child.children:
						childEdge = self._edges[childEdgeID]
						childEdge.value = value
						queue.appendleft(childEdgeID)
		
		return result			
					
		

	def garble(self, n):
		'''
		@param n - The size of the encrypted keys
		'''
		import os
		import otp
		import copy
		from collections import deque
		
		pad = otp.OTP()
		
		garbledGates = {}
		for edgeID in self._edges:
			zero = os.urandom(n)
			one = os.urandom(n)
			# Assign two random values to each edge, one for off, the other for on
			self._edges[edgeID].value = (zero, one)
		
		for gateID in self._gates:
			gate = self._gates[gateID]
			table = {}
			
			inputQueue = deque([{}])
			
			for edgeID in gate.parents:
				parentEdge = self._edges[edgeID]
				tmpQueue = deque([])
				while len(inputQueue) > 0:
					data = inputQueue.pop()
					dataCopy = copy.copy(data)
					data[edgeID] = (0, parentEdge.value[0])
					dataCopy[edgeID] = (1, parentEdge.value[1])
					tmpQueue.appendleft(data)
					tmpQueue.appendleft(dataCopy)
				inputQueue = tmpQueue
			
			while len(inputQueue) > 0:
				inputs = inputQueue.pop()
				# e.g. {'edge1':(1, 326752), 'edge2':(0:354), ...}
				args = []
				newArgs = []
				argMap = []
				for edgeID in inputs:
					args.append(inputs[edgeID][0])
					newArgs.append(inputs[edgeID][1])
					argMap.append((edgeID, inputs[edgeID][0]))
				output = gate(*args)
				argMap = tuple(argMap)
				key = None
				for arg in newArgs:
					if key is None:
						key = arg
					else:
						key = pad.encrypt(key, n, arg)[0]		
				print 'inputs = %s:\tkey = %r' % (inputs, key)
				table[argMap] = {}
				for childID in gate.children:
					childEdge = self._edges[childID]
					curValue = childEdge.value[output]
					curValue = pad.encrypt(curValue, n, key)[0]
					print 'childEdge.value[output] = %r\t key = %r\t curValue = %r' % (childEdge.value[output], key, curValue)
					MACkey = os.urandom(n*2)
					mac = pad.mac(childEdge.value[output], n, MACkey)[0]
					table[argMap][childID] = (str(curValue), (str(mac), MACkey))
			
			print table
			# scrambledTable is a map from childIDs to a set of (encrypted values, MAC)
			scrambledTable	= {}
			for argMap in table:
				#setVals = set([])
				for childID in table[argMap]:
					if childID not in scrambledTable:
						scrambledTable[childID] = set([])
						
					encVal = table[argMap][childID]
					#setVals.add(encVal)
					scrambledTable[childID].add(encVal)


			# garbledGates is a map from the gateID to the scrambledTable
			garbledGates[gateID] = scrambledTable
		
		return (garbledGates, self._edges)
	
	def evaluateGarbled(self, inputs, outputs, n):
		'''
		@param inputs - A map of ID to their initial states
		@param outputs - A set of ID of outputs
		@param n - The size of the encrypted keys
		'''
		import otp
		from collections import deque
			
		done = set()	
		result = {}
		
		queue = deque()
		
		for edgeID in inputs:
			edgeValue = inputs[edgeID]
			print 'Setting input value %s\t%s' % (edgeID, edgeValue)
			self._edges[edgeID].value = edgeValue
			queue.appendleft(edgeID)
			
		while len(queue) > 0:
			edgeID = queue.pop()
			edge = self._edges[edgeID]
			print 'Evauating edge: %s' % edgeID
			if edgeID in outputs:
				result[edgeID] = edge.value
			
			if edge.child is not None:
				print 'I am not a sink'
				vals = []
				child = self._gates[edge.child]
				print 'Trying to evaulate gate: %s' % edge.child
				if edge.child in done:
					continue
				for parentEdgeID in child.parents:
					parent = self._edges[parentEdgeID]
					print 'Getting edge value: %s\t%s' % (parentEdgeID, parent.value)
					vals.append(parent.value)
				if None not in vals:
					print 'All parents have a value'
					value = child(*vals)
					done.add(edge.child)
					for childEdgeID in child.children:
						childEdge = self._edges[childEdgeID]
						print 'Setting edge value %s\t%s' % (childEdgeID, value[childEdgeID])
						childEdge.value = value[childEdgeID]
						queue.appendleft(childEdgeID)
		
		return result		
		
		

if __name__ == '__main__':
	andGate = Gate(lambda x, y: x * y)
	orGate = Gate(lambda x, y: (x + y + 1)/2)
	notGate = Gate(lambda x: 1 - x)
	
	andTableGate = TableGate({(1,1):1, (1,0):0, (0,1):0, (0,0):0})
	orTableGate = TableGate({(1,1):1, (1,0):1, (0,1):1, (0,0):0})
	notTableGate = TableGate({(0,):1, (1,):0})
	
	gates = {}
	edges = {}
	
	gates['and1'] = andTableGate
	#gates['and2'] = andGate
	#gates['and3'] = andGate
	gates['or1'] = orTableGate
	#gates['or2'] = orGate
	#gates['or3'] = orGate
	gates['not1'] = notTableGate
	#gates['not2'] = notGate
	#gates['not3'] = notGate
	
	
	
	edges['edge1'] = Edge(None, 'and1')
	edges['edge2'] = Edge(None, 'and1')
	# 1 & 2
	edges['edge3'] = Edge('and1', 'not1')
	# ~(1 & 2)
	edges['edge4'] = Edge('and1', 'or1')
	edges['edge5'] = Edge('not1', 'or1')
	# (1 & 2) | ~(1 & 2)
	edges['edge6'] = Edge('or1', None)
	
	edgeNames = [i for i in edges]
	
	circ = Circuit(gates, edges)
	
	print circ.evaluate({'edge1':1, 'edge2':1}, edgeNames)
	print circ.evaluate({'edge1':1, 'edge2':0}, edgeNames)
	print circ.evaluate({'edge1':0, 'edge2':1}, edgeNames)
	print circ.evaluate({'edge1':0, 'edge2':0}, edgeNames)
	
	n = 16	
	
	garbGatesData, garbEdges = circ.garble(n)
	
	garbGates = {}
	for gateID in garbGatesData:
		# scrambledTable is a map from childIDs to a set of (encrypted values, MAC)
		scrambledTable = garbGatesData[gateID]
		#def evalGarbGate(table, *args):
			#import otp
			#pad = otp.OTP()
			#key = None
			#for arg in args:
				#print '%r' % arg
				#if key is None:
					#key = arg
				#else:
					#key = pad.decrypt(key, n, arg)[0]
			#outputs = {}
			#print '%r' % key
			#for childID in table:
				#for output in table[childID]:
					#value = pad.decrypt(output[0], n, key)[0]
					#print 'output[0] = %r\t key = %r\t value = %r' % (output[0], key, value)
					#if pad.verify(value, output[1][0], n, output[1][1])[0]:
						#outputs[childID] = value
						#break
				#if childID not in outputs:
					#raise Exception
				
			#return outputs
			
				
		garbGates[gateID] = GarbledGate(scrambledTable)
	
	print 'and1: ', garbGates['and1'](garbEdges['edge1'].value[1], garbEdges['edge2'].value[1])
	garbCirc = Circuit(garbGates, edges)
	
	print [(i, garbEdges[i].value) for i in garbEdges]
	
	value = garbCirc.evaluateGarbled({'edge1':garbEdges['edge1'].value[1], 'edge2':garbEdges['edge2'].value[1]}, edgeNames, n)
	print value == garbEdges['edge6'].value[0], value == garbEdges['edge6'].value[1], value
	print garbCirc.evaluateGarbled({'edge1':garbEdges['edge1'].value[1], 'edge2':garbEdges['edge2'].value[0]}, edgeNames, n)
	print garbCirc.evaluateGarbled({'edge1':garbEdges['edge1'].value[0], 'edge2':garbEdges['edge2'].value[1]}, edgeNames, n)
	print garbCirc.evaluateGarbled({'edge1':garbEdges['edge1'].value[0], 'edge2':garbEdges['edge2'].value[0]}, edgeNames, n)	