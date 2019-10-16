app:obj/time_tcp_server.o obj/time_tcp_client.o obj/msg.o obj/echo_tcp_server_process.o obj/echo_tcp_server_thread.o  obj/echo_tcp_client.o
	gcc obj/time_tcp_server.o -o bin/time_tcp_server
	gcc obj/time_tcp_client.o -o bin/time_tcp_client
	gcc -Iinclude obj/echo_tcp_client.o obj/msg.o -o bin/echo_tcp_client
	gcc -Iinclude obj/echo_tcp_server_process.o obj/msg.o -o bin/echo_tcp_server_process
	gcc -Iinclude obj/echo_tcp_server_thread.o obj/msg.o -o bin/echo_tcp_server_thread

obj/time_tcp_server.o:src/time_tcp_server.c
	gcc  -Iinclude -c $< -o $@

obj/time_tcp_client.o:src/time_tcp_client.c
	gcc  -Iinclude -c $< -o $@

obj/msg.o:src/msg.c
	gcc  -Iinclude -c $< -o $@

obj/echo_tcp_server_process.o:src/echo_tcp_server_process.c
	gcc  -Iinclude -c $< -o $@

obj/echo_tcp_server_thread.o:src/echo_tcp_server_thread.c
	gcc  -Iinclude -c $< -o $@

obj/echo_tcp_client.o:src/echo_tcp_client.c
	gcc  -Iinclude -c $< -o $@

clean:
	rm bin/*
	rm obj/*
