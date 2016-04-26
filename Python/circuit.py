class BinaryGate:
	'''
	A binary gate has exactly two inputs and one output
	'''

	def __init__(self, name, table):
		'''
		@param name - The unique identifier for this gate
		@param table - A dict of input tuples of length 2 to output truth values
		@param parents - A tuple of length 2 containing the incoming edges
		@param child - The outgoing edge
		'''
		self.name = name
		self.table = table
		self.parents = set([])
		self.child = None

	def __hash__(self):
		return hash(self.name)

	def __str__(self):
		return self.name

	def __repr__(self):
		return self.name
		#ret = self.name
		#for row in self.table:
			#ret += '\n%s\t%s\t%s' % (row[0], row[1], self.table[row])
		#return ret + '\n' + str(self.parents) + '\n' + str(self.child)

	def eval(self, input1, input2):
		return self.table[(input1, input2)]

	def __call__(self, *args):
		if len(args) == 1:
			return self.eval(args[0], args[0])		
		if len(args) == 2:
			return self.eval(args[0], args[1])

	def garble(self, n):
		'''
		@param n - the size of the key in bytes
		@return - a garbled gate of this gate
		'''
		encrypedTable = []
		for args in self.table:
			# args is the current tuple of arguments

			# encArgs is a list of the encrypted values
			encArgs = []
			for i, arg in enumerate(args):
				# i is the argument number, arg is the argument value

				# get the ith parent's arg value
				encArgs.append(self.parents[i].value[arg])

			# key is the encryption of all inputs
			key = None
			for arg in encArgs:
				if key is None:
					key = arg
				else:
					key = pad.encrypt(key, n, arg)[0]

			# outVal is the value of the output
			outVal = self.table[args]     

			# The value is the value of the output edge
			value = self.child.value[outVal]

			# The encrypted value is the encryption of the value
			encVal = str(pad.encrypt(value, n, key)[0])

			## Generate a random MAC key
			#MACkey = os.urandom(n*2)

			## Compute the MAC
			#mac = str(pad.mac(value, n, MACkey)[0])

			# Add this edge to the encrypedTable
			encrypedTable.append(encVal)

		return GarbledBinaryGate(self.name, encrypedTable, self.parents, self.child)          

class GarbledBinaryGate(BinaryGate):
	pass

class Edge:
	'''
	An edge connecting the output of one gate to the input of another
	'''
	def __init__(self, name, parent = None, child = None, value = None):
		'''
		@param name - The name of the edge
		@param child - The name of the gate that this feeds into
		@param parent - The name of the gate that this came from
		@param value - The value of this edge
		'''
		self.name = name
		self.parent = parent
		self.child = child
		self.value = value

	def __str__(self):
		return '%s:\t%s\t%s\t%s' % (self.name, self.parent, self.child, self.value)

	def __repr__(self):
		return '%s:\t%s\t%s\t%s' % (self.name, self.parent, self.child, self.value)
	
	def __hash__(self):
		return hash(self.name)

	def garble(self, n):
		# Randomly assign values to represent each state
		return GarbledEdge(self.name, self.child, self.parent, {0:os.urandom(n), 1:os.urandom(n)})

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
		self.gates = gates
		self.edges = edges
		
		lastEdge = None
		for edgeID in self.edges:
			edge = self.edges[edgeID]
			
			if edge.parent is not None:
				self.gates[edge.parent].child = edgeID
			if edge.child is not None:
				print self.gates[edge.child].parents
				self.gates[edge.child].parents.add(edgeID)
			if lastEdge:
				print lastEdge is edge
			lastEdge = edge
				
	def evaluate(self, inputs, outputs):
		'''
		@param inputs - A map of ID to their initial states
		@param outputs - A set of ID of outputs
		'''
		from collections import deque
		result = {}
	
		queue = deque()
		
		# set the input edges to their input values
		for edgeID in inputs:
			edgeValue = inputs[edgeID]
			self.edges[edgeID].value = edgeValue
			queue.appendleft(edgeID)
			
		while len(queue) > 0:
			edgeID = queue.pop()
			edge = self.edges[edgeID]
			
			if edgeID in outputs:
				# If the edge is in outputs, add it to results
				result[edgeID] = edge.value
		
			if edge.child is not None:
				# If we have a child
				vals = []
				child = self.gates[edge.child]
				
				# Gather each of the gate's parent's values
				for parentEdgeID in child.parents:
					parent = self.edges[parentEdgeID]
					vals.append(parent.value)
					
				# Check that each of the gate's parent has a value
				if None not in vals:
					# Evaluate the gate with the arguments
					value = child(*vals)
					edgeChild = self.edges[child.child]
					edgeChild.value = value
					queue.appendleft(child.child)
		
		return result		
			
			
	
class GarbledCircuit(Circuit):
	pass


if __name__ == '__main__':
	ifTable = {(1,1):1,(1,0):0, (0,1):1, (0,0):1}
	nifTable = {(1,1):0,(1,0):1, (0,1):0, (0,0):0}
	iffTable = {(1,1):1,(1,0):0, (0,1):0, (0,0):1}
	not1Table = {(1,1):0,(1,0):0, (0,1):1, (0,0):1}
	andTable = {(1,1):1,(1,0):0, (0,1):0, (0,0):0}
	orTable = {(1,1):1,(1,0):1, (0,1):1, (0,0):0}
	#nif = and(a,not(b)) = 0, 1, 0, 0
	n = 4
	
	def makeGTpart(n):
		gates = {}
		edges = {}		
		
		
		needsAnding = []
		
		#needsAnding.append('a%02d%02d' % (n,n))
		
		notGate = BinaryGate('not b%02d%02d' % (n,n), not1Table)
		edges['b%02d%02d' % (n,n)] = Edge('b%02d%02d' % (n,n), None, 'not b%02d%02d' % (n,n))
		gates[notGate.name] = notGate
		
		andGate = BinaryGate('and %s %s' % ('a%02d%02d' % (n,n),notGate), andTable)
		gates[andGate.name] = andGate
		edges['a%02d%02d' % (n,n)] = Edge('a%02d%02d' % (n,n), None, andGate.name)
		edges[notGate.name] = Edge(notGate.name, notGate.name, andGate.name)
		needsAnding.append(andGate.name)
		
		for i in range(n):
			iffGate = BinaryGate('iff a%02d%02d b%02d%02d' % (i, n, i, n), iffTable)
			edges['a%02d%02d' % (i,n)] = Edge('a%02d%02d' % (i,n), None, 'iff a%02d%02d b%02d%02d' % (i, n, i, n))
			edges['b%02d%02d' % (i,n)] = Edge('b%02d%02d' % (i,n), None, 'iff a%02d%02d b%02d%02d' % (i, n, i, n))
			needsAnding.append(iffGate.name)
			gates[iffGate.name] = iffGate		
			# TODO
		
		#if n == 0:
			#edges['a%02d%02d' % (n,n)] = Edge('a00,00', 'a00,00', 'and: not: b00,00 a00,00')
			#needsAnding.append(edges['a00,00'])
			
		lastAnd = None
		while len(needsAnding) > 0:
			nextGate = needsAnding.pop()
			if lastAnd is None:
				lastAnd = nextGate
			else:
				andGate = BinaryGate('and %s %s' % (lastAnd,nextGate), andTable)
				edges[lastAnd] = Edge(lastAnd, lastAnd, andGate.name)
				edges[nextGate] = Edge(nextGate, nextGate, andGate.name)
				gates[andGate.name] = andGate
				lastAnd = andGate.name
		
		return (gates, edges, lastAnd)
	
	gates, edges, lastOutput = makeGTpart(0)
	for i in range(1, n):
		curGates, curEdges, output = makeGTpart(i)
		for g in curGates:
			gates[g] = curGates[g]
		#print curEdges
		for e in curEdges:
			edges[e] = curEdges[e]
		
		orGate = BinaryGate('or %s %s' % (lastOutput,output), orTable)
		edges[lastOutput] = Edge(lastOutput, lastOutput, orGate.name)
		edges[output] = Edge(output, output, orGate.name)
		gates[orGate.name] = orGate
		lastOutput = orGate.name
	edges['output'] = Edge('output', lastOutput)
			
	
	
	c = Circuit(gates, edges)
	inputs = {}
	for i in range(n):
		a = {0:0, 1:1, 2:1, 3:0}
		b = {0:0, 1:1, 2:0, 3:0}
		for j in range(n):
			if i <= j:
				inputs['a%02d%02d' % (i,j)] = a[i]
				inputs['b%02d%02d' % (i,j)] = b[i]
	print c.evaluate(inputs, ('output',))