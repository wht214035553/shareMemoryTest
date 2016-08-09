all:
	gcc readShareMemory.c -o readShareMemory -g
	gcc writeShareMemory.c -o writeShareMemory -g
clean:
	-rm readShareMemory writeShareMemory
