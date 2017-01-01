all:
	gcc -o hgfastbranch hgfastbranch.c
	cp -f hgfastbranch ~/scripts

clean:
	rm -rf hgfastbranch
