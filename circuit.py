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
					
		

	def garble(self):
		pass

if __name__ == '__main__':
	andGate = Gate(lambda x, y: x * y)
	orGate = Gate(lambda x, y: (x + y + 1)/2)
	notGate = Gate(lambda x: 1 - x)
	gates = {}
	edges = {}
	
	gates['and1'] = andGate
	gates['and2'] = andGate
	gates['and3'] = andGate
	gates['or1'] = orGate
	gates['or2'] = orGate
	gates['or3'] = orGate
	gates['not1'] = notGate
	gates['not2'] = notGate
	gates['not3'] = notGate
	
	edges[1] = Edge(None, 'and1')
	edges[2] = Edge(None, 'and1')
	# 1 & 2
	edges[3] = Edge('and1', 'not1')
	# ~(1 & 2)
	edges[4] = Edge('and1', 'or1')
	edges[5] = Edge('not1', 'or1')
	# (1 & 2) | ~(1 & 2)
	edges[6] = Edge('or1', None)
	
	circ = Circuit(gates, edges)
	
	print circ.evaluate({1:1, 2:1}, [1,2,3,4,5,6])
	print circ.evaluate({1:1, 2:0}, [1,2,3,4,5,6])
	print circ.evaluate({1:0, 2:1}, [1,2,3,4,5,6])
	print circ.evaluate({1:0, 2:0}, [1,2,3,4,5,6])
	