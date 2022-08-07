from source import Options,Generator

if __name__ == '__main__':
	try:
		options = Options.IdGeneratorOptions(workerId=23,seqBitLength=10)
		options.BaseTime = 1231111111
		idgen = Generator.DefaultIdGenerator()
		idgen.SetIdGernerator(options)

		uid = idgen.NextId()

		print(uid)
		print(options.__dict__)
		
	except ValueError  as e:
		print(e)


