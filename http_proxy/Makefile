all: client proxy

client: clean client.c
	gcc -o client client.c
	#./client 127.0.0.1 7777 www.daum.net
	
proxy: clean proxy.c
	gcc -o proxy proxy.c
	# ./proxy 127.0.0.1 7777
	
clean:
	rm -rf client *.o
	rm -rf proxy *.o
	rm -rf ./cache/
	
	
	
