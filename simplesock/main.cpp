#include "SockStream.h"
#include "SockConnector.h"
#include "SockDgram.h"
#include "SockAcceptor.h"
#include "ThreadSvc.h"
#include "InetAddr.h"
#include "UnixAddr.h"
#include "LogMsg.h"
#include "SharedMem.h"
#include "LapTimer.h"
#include "Poll.h"
//#include "Mutex.h"

void testshm()
{
	//=== shm 생성자측 사용법
	SharedMem shmCreator;
	shmCreator.create("myshm.dat", 1024);  // create 1Kb shm
	printf("key=%d, shmid=%d\n", shmCreator.get_key(), shmCreator.get_id());
	if (shmCreator.open() < 0)
		printf("failed\n");
	else
		printf("shm addr=%X\n", shmCreator.get_addr());
	strcpy((char*)shmCreator.get_addr(), "This shm created");
	shmCreator.close();

	// create() 호출하더라도 이미 shm 이 존재하면, 그냥 attach 된다.
	SharedMem shmC2;
	shmC2.create("myshm.dat", 1024);  // create 1Kb shm
	printf("key=%d, shmid=%d\n", shmC2.get_key(), shmC2.get_id());
	if (shmC2.open() < 0)
		printf("failed\n");
	else
		printf("shm addr=%X\n", shmC2.get_addr());
	strcpy((char*)shmC2.get_addr(), "This shm2 created");
	shmC2.close();

	//=== shm 사용자측 사용법
	SharedMem shmReader;
	shmReader.open("myshm.dat", false);  // read only mode == false
	printf("key=%d, shmid=%d\n", shmReader.get_key(), shmReader.get_id());
	printf("shm addr=%X\n", shmReader.get_addr());
	printf("CONT: %s\n", shmReader.get_addr());
	if (shmReader.get_addr()) strcpy((char*)shmReader.get_addr(), "This shm read by shmReader");
	shmReader.close();

	SharedMem shmR2;
	shmR2.open("myshm.dat", false);  // read only mode == false. read only mode 에서 write 하면 프로그램 오류 발생함. 
	printf("key=%d, shmid=%d\n", shmR2.get_key(), shmR2.get_id());
	printf("shm addr=%X\n", shmR2.get_addr());
	printf("CONT: %s\n", shmR2.get_addr());
	if (shmR2.get_addr())  strcpy((char*)shmR2.get_addr(), "This shm written by shmR2");
}

/// 
void test_addr()
{
	UnixAddr a("/tmp/aaaa.sock");
	UnixAddr b(a);

	a.print("== a ==");
	b.print("== b ==");
	
	InetAddr c("127.0.0.1", 10010);
	InetAddr d(c);

	
	c.print("== c ==");
	d.print("== d ==");
	
	InetAddr local_addr("127.0.0.1", 0);
	local_addr.print("== local_addr ==");
}

class StreamServer: public ThreadSvc
{
	int svc()
	{
		InetAddr server_addr("127.0.0.1", 30000);
		SockAcceptor server;		
		TimeValue timeout(20*1000); // 20초 timeout
		server_addr.print("Server");
		int res = server.open(server_addr);
		printf("Server: server.open()=%d, sockfd=%d\n", res, server.get_handle());

		printf("Server: waiting a new client for %d secs\n", timeout.sec());

		unsigned char buffer[128];
		SockStream client;
		InetAddr client_addr;
		res = server.accept(client, &client_addr, &timeout);
		if (res < 0) printf("accept failed !!!\n");
		else if (res == 0) printf("accept timeout  !\n");
		else {
			client_addr.print("accepted New Client");
			res = client.recv(buffer,  sizeof(buffer)-1);
			SLEEP(1);	

			printf("server recv %d bytes\n", res);
			if (res > 0) {
				buffer[res] = 0;
				printf("server recv data; %s\n", buffer);
			}			
			res = client.send(buffer, strlen((const char*)buffer));
			printf("server sent %d bytes\n", res);
		}

		SLEEP(1*1000);
		printf("server closing now..\n");
		client.close();
		server.close();
		return 0;
	} // svc()
};

/// test TCP
void test_stream()
{
	// run TCP server
	StreamServer server;
	server.activate();  

	/// run TCP client	
	SockStream stream1;
	stream1.open(AF_INET);
	
	SLEEP(100);
	SockConnector conn;
	InetAddr svrAddr("127.0.0.1", 30000);	
	TimeValue conn_time(3000); // 3초 timeout

	printf("connecting...\n");
	LapTimer lap;
	lap.start();
	int res = conn.connect(stream1, svrAddr, conn_time);
	//int res = conn.connect(stream1, svrAddr);
	lap.mark();
	printf("msec=%d for connect()\n", lap.msec());
	if (res < 0) {
		printf("connect failed !!\n");
		return;
	}
	printf("connected\n");
	
	SLEEP(100);
	unsigned char buffer[128] = "Hello !!";

	int send_Res = stream1.send((const char*)buffer, strlen((const char*)buffer));
	printf("client sent %d bytes\n", send_Res);
	LOG_PRN((LM_DEBUG, "SEND: %d bytes", send_Res));
	
	printf("client waiting to read...3sec\n");
	unsigned char recvbuff[1024*2] = {0};
	TimeValue timeout(4*1000);
	int recved_Leng = stream1.recv_n((char*)recvbuff, sizeof(recvbuff), timeout);
	printf("client recv %d bytes\n", recved_Leng);

	LOG_PRN((LM_DEBUG, "RECV: %d bytes", recved_Leng));
	LOG_PRN((LM_DEBUG, "RECV: %s", recvbuff));
	HEX_DUMP((LM_DEBUG, (const char*)recvbuff, recved_Leng));

	recved_Leng = stream1.recv_n((char*)recvbuff, sizeof(recvbuff), timeout);
	printf("client recv %d bytes\n", recved_Leng);
}


#define	LOADBUFFSIZE	(1024*1024)
#define LOAD_UNIT	(20)
#define LOAD_MAXCNT	(LOAD_UNIT*10)

class StreamServer2: public ThreadSvc
{
	int svc()
	{
		InetAddr server_addr("127.0.0.1", 30000);
		SockAcceptor server;		
		TimeValue timeout(20*1000); // 20초 timeout
		TimeValue recvtime(2*1000); // 20초 timeout
		//server_addr.print("Server");
		int res = server.open(server_addr);
		printf("Server: server.open()=%d, sockfd=%d\n", res, server.get_handle());

		//unsigned char buffer[LOADBUFFSIZE+1];
		//unsigned char sendbuffer[LOADBUFFSIZE+1];
		unsigned char *buffer=0;
		unsigned char *sendbuffer=0;
		buffer = (unsigned char *)calloc(LOADBUFFSIZE+1, 1);
		sendbuffer = (unsigned char *)calloc(LOADBUFFSIZE+1, 1);
		memset(sendbuffer, LOADBUFFSIZE, 'B');
		
		SockStream client;
		InetAddr client_addr;
		res = server.accept(client, &client_addr, &timeout);
		if (res < 0) printf("accept failed !!!\n");
		else if (res == 0) printf("accept timeout  !\n");
		else {
			client_addr.print("accepted New Client");

			int cnt = 0;
			while(1) {
				res = client.recv_n(buffer,  LOADBUFFSIZE, recvtime);
				if (res > 0) {
					cnt++;
					if (cnt % LOAD_UNIT == 0) printf("Server cnt[%d]\n", cnt);
					
					buffer[res] = 0;
					//printf("server recv data; %s\n", buffer);
					res = client.send_n(sendbuffer, LOADBUFFSIZE, recvtime);
				}
				else break;
			}			
			
			//printf("server sent %d bytes\n", res);
		}

		SLEEP(1*1000);
		printf("server closing now..\n");
		client.close();
		server.close();
		return 0;
	} // svc()
};



/// test TCP
void test_stream_load()
{
	// run TCP server
	StreamServer2 server;
	server.activate();  

	/// run TCP client	
	SockStream stream1;
	stream1.open(AF_INET);
	
	SLEEP(100);
	SockConnector conn;
	InetAddr svrAddr("127.0.0.1", 30000);	
	TimeValue conn_time(3000); // 3초 timeout
	

	printf("connecting...\n");
	
	LapTimer lap;
	lap.start();
	int res = conn.connect(stream1, svrAddr, conn_time);
	//int res = conn.connect(stream1, svrAddr);
	lap.mark();
	printf("msec=%d for connect()\n", lap.msec());
	if (res < 0) {
		printf("connect failed !!\n");
		return;
	}
	printf("connected\n");
	
	SLEEP(100);

	//unsigned char buffer[LOADBUFFSIZE+1];
	//unsigned char sendbuffer[LOADBUFFSIZE+1];
	unsigned char *buffer=0;
	unsigned char *sendbuffer=0;
	buffer = (unsigned char *)calloc(LOADBUFFSIZE+1, 1);
	sendbuffer = (unsigned char *)calloc(LOADBUFFSIZE+1, 1);

	memset(sendbuffer, LOADBUFFSIZE, 'A');

	TimeValue send_time(2000); // 2초 timeout
	TimeValue recv_time(4*1000);
	int cnt=0;
	LapTimer loadlap;
	loadlap.start();

	int loopcnt = 0;
	int sum_laptime=0;
	while (cnt < LOAD_MAXCNT) {				
		int send_Res = stream1.send_n((const char*)sendbuffer, LOADBUFFSIZE, send_time);
		int recved_Leng = stream1.recv_n((char*)buffer, LOADBUFFSIZE, recv_time);
		cnt++;
		if (cnt % LOAD_UNIT == 0) {
			loopcnt++;
			loadlap.mark();
			sum_laptime += loadlap.msec();
			printf("Client cnt[%d]: %d msec. %d bytes\n", cnt, loadlap.msec(), send_Res);
			loadlap.start();
		}				
	}
	printf("closing client...\n");
	printf("Avg laptime: %0.2f\n", (float)sum_laptime / (LOAD_MAXCNT/LOAD_UNIT));
	stream1.close();

}

class UnixStreamServer: public ThreadSvc
{
	int svc()
	{
		UnixAddr server_addr("/tmp/server.sock");
		SockAcceptor server;		
		TimeValue timeout(20*1000); // 20초 timeout
		server_addr.print("Server");
		int res = server.open(server_addr);
		if (res < 0) {
			printf("ERROR: %s\n", server.error_msg());
		}
		printf("Server: server.open()=%d, sockfd=%d\n", res, server.get_handle());

		printf("Server: waiting a new client for %d secs\n", timeout.sec());

		unsigned char buffer[128];
		SockStream client;
		UnixAddr client_addr;
		res = server.accept(client, &client_addr, &timeout);
		if (res < 0) printf("accept failed !!!\n");
		else if (res == 0) printf("accept timeout  !\n");
		else {
			client_addr.print("accepted New Client");
			res = client.recv(buffer,  sizeof(buffer)-1);
			SLEEP(1);	

			printf("server recv %d bytes\n", res);
			if (res > 0) {
				buffer[res] = 0;
				printf("server recv data; %s\n", buffer);
			}			
			res = client.send(buffer, strlen((const char*)buffer));
			printf("server sent %d bytes\n", res);
		}

		SLEEP(3);
		printf("server closing now..\n");
		client.close();
		server.close();
		return 0;
	} // svc()
};



/// test TCP
void test_unix_stream()
{
	// run TCP server
	UnixStreamServer server;
	server.activate();  

	/// run TCP client	
	SockStream stream1;
	//stream1.open(AF_UNIX);
	UnixAddr cliAddr("/tmp/client.sock");	
	//stream1.bind(cliAddr);	
	stream1.open(cliAddr);
	
	SLEEP(100);
	SockConnector conn;
	UnixAddr svrAddr("/tmp/server.sock");	
	TimeValue conn_time(3000); // 3초 timeout

	printf("connecting...\n");
	LapTimer lap;
	lap.start();
	int res = conn.connect(stream1, svrAddr, conn_time);
	//int res = conn.connect(stream1, svrAddr);
	lap.mark();
	printf("msec=%d for connect()\n", lap.msec());
	if (res < 0) {
		printf("connect failed !!\n");
		return;
	}
	printf("connected\n");
	
	SLEEP(100);
	unsigned char buffer[128] = "Hello !!";

	int send_Res = stream1.send((const char*)buffer, strlen((const char*)buffer));
	printf("client sent %d bytes\n", send_Res);
	LOG_PRN((LM_DEBUG, "SEND: %d bytes", send_Res));
	
	printf("client waiting to read...3sec\n");
	unsigned char recvbuff[1024*2] = {0};
	TimeValue timeout(4*1000);
	int recved_Leng = stream1.recv_n((char*)recvbuff, sizeof(recvbuff), timeout);
	printf("client recv %d bytes\n", recved_Leng);

	LOG_PRN((LM_DEBUG, "RECV: %d bytes", recved_Leng));
	LOG_PRN((LM_DEBUG, "RECV: %s", recvbuff));
	HEX_DUMP((LM_DEBUG, (const char*)recvbuff, recved_Leng));

	recved_Leng = stream1.recv_n((char*)recvbuff, sizeof(recvbuff), timeout);
	printf("client recv %d bytes\n", recved_Leng);
}


class DgramServer: public ThreadSvc
{
	int svc()
	{
		InetAddr server_addr("127.0.0.1", 20000);

		
		InetAddr client_addr;
		SockDgram dgram;
		TimeValue timeout(2000); // 2초 timeout
		unsigned char buffer[128];

		server_addr.print("Server");
		int res = dgram.open(server_addr);
		//dgram.print("Dgram Server");
		printf("Server: dgram.open()=%d, sockfd=%d\n", res, dgram.get_handle());
		
		int cnt = 0;
		while(cnt < 3) {			
			//int res = dgram.recv(buffer, sizeof(buffer)-1, client_addr);
			int res = dgram.recv_n(buffer, sizeof(buffer)-1, &client_addr, timeout);			
			if (res < 0) {
				printf("Server: error in recv() -- %s\n", dgram.error_msg());
				break;
			}
			if (res > 0) {
				client_addr.print("Client");
				printf("server recv %d bytes\n", res);
				buffer[res] = 0;
				printf("server recv data; %s\n", buffer);
			}
			else	{
				printf("server timeout\n");
			}
			cnt++;
		}  // while
		char *msg = "I am going HOME ^^";
		dgram.send(msg, strlen(msg), client_addr);
		return 0;
	} // svc()
};

class DgramClient: public ThreadSvc
{
	int svc()
	{
		InetAddr server_addr("127.0.0.1", 20000);
		SockDgram dgram;
		TimeValue timeout(2000); // 2초 timeout
		unsigned char buffer[128] = "Hello UDP !!";
		int res = dgram.open(AF_INET);
		printf("Client: dgram.open()=%d, sockfd=%d\n", res, dgram.get_handle());
		//dgram.print("Dgram Client");
		
		while(1) {
			int res = dgram.send(buffer, strlen((const char*)buffer), server_addr);
			printf("client sent %d bytes\n", res);
			if (res < 0) {
				printf("Client: error in send() -- %s\n", dgram.error_msg());
				break;
			}

			char recvbuff[128]={0};
			InetAddr cliaddr;
			res = dgram.recv_n(recvbuff, 32, &cliaddr, timeout); 
			if (res > 0) {
				recvbuff[res] = 0;
				printf("client recv -- %s\n", recvbuff);
				break;
			}
			//SLEEP(2000);
		} // while
		return 0;
	} // svc()
};


/// test UDP
void test_dgram()
{
	DgramServer dserver;
	DgramClient dclient;
	dserver.activate();
	SLEEP(500);
	dclient.activate();
	
	dclient.wait();	
}

#define UNIX_UDP_ADDR	"/tmp/UDS.sock"
class UnixDgramServer: public ThreadSvc
{
	int svc()
	{
		UnixAddr server_addr(UNIX_UDP_ADDR);
		UnixAddr client_addr;
		SockDgram dgram;
		TimeValue timeout(2000); // 2초 timeout
		unsigned char buffer[128];

		server_addr.print("Server");
		int res = dgram.open(server_addr);
		//dgram.print("Dgram Server");
		printf("Server: dgram.open()=%d, sockfd=%d\n", res, dgram.get_handle());
		
		int cnt = 0;
		while(cnt < 3) {			
			//int res = dgram.recv(buffer, sizeof(buffer)-1, client_addr);
			int res = dgram.recv_n(buffer, sizeof(buffer)-1, &client_addr, timeout);			
			if (res < 0) {
				printf("Server: error in recv() -- %s\n", dgram.error_msg());
				break;
			}
			if (res > 0) {
				client_addr.print("Client");
				printf("server recv %d bytes\n", res);
				buffer[res] = 0;
				printf("server recv data; %s\n", buffer);
			}
			else	{
				printf("server timeout\n");
			}
			cnt++;
		}  // while
		char *msg = "I am going HOME ^^";
		dgram.send(msg, strlen(msg), client_addr);
		return 0;
	} // svc()
};

class UnixDgramClient: public ThreadSvc
{
	int svc()
	{
		UnixAddr server_addr(UNIX_UDP_ADDR);
		SockDgram dgram;
		TimeValue timeout(2000); // 2초 timeout
		unsigned char buffer[128] = "Hello UDP !!";
		UnixAddr myaddr("/tmp/UDCaaaaaaaaaaaaaaa.sock");
		myaddr.print("Client Myaddr");
		//int res = dgram.open(AF_UNIX);
		int res = dgram.open(myaddr);
		printf("Client: dgram.open()=%d, sockfd=%d\n", res, dgram.get_handle());
		//dgram.print("Dgram Client");
		
		while(1) {
			int res = dgram.send(buffer, strlen((const char*)buffer), server_addr);
			printf("client sent %d bytes\n", res);
			if (res < 0) {
				printf("Client: error in send() -- %s\n", dgram.error_msg());
				break;
			}

			char recvbuff[128]={0};
			UnixAddr cliaddr;
			res = dgram.recv_n(recvbuff, 32, &cliaddr, timeout); 
			if (res > 0) {
				cliaddr.print("cli recv");
				recvbuff[res] = 0;
				printf("client recv -- %s\n", recvbuff);
				break;
			}
			//SLEEP(2000);
		} // while
		return 0;
	} // svc()
};


/// test UDP Unix Addr
void test_Unixdgram()
{
	UnixDgramServer dserver;
	UnixDgramClient dclient;
	dserver.activate();
	SLEEP(500);
	dclient.activate();
	
	dclient.wait();	
}
/*
void test_mutex()
{
	Mutex m(INTER_PROC);
	m.lock();
	m.unlock();
}
*/

int main(int argc, char* argv[])
{
	LOG_OPEN("test");
	LOG_PRN((LM_DEBUG, "==== Starts ===="));	
	
	Sock sock;
	SockStream stream;
	SockDgram dgram;
	SockAcceptor acceptor;
	Poll poller;
	poller.add(sock, POLLIN);
	poller.add(stream, POLLIN);
	poller.get_sock(0);
	#ifdef _WIN32
	printf("Sock[%X]  Stream[%X]  Dgram[%X]  Acceptor[%X]\n", sock.get_handle(), stream.get_handle(), dgram.get_handle(), acceptor.get_handle());
	#endif

	sock.open(SOCK_STREAM, AF_INET);
	InetAddr addr("100.10.10.1", 100);
	
	//sock.bind(addr); -- bind()는 protected. Acceptor나 SockDgram 내부에서만 호출.

	//test_mutex();

	//testshm();

	//test_stream();

	test_stream_load();

	//test_dgram();

	//test_unix_stream();

	//test_Unixdgram();

	printf("\n------ Done. Enter any char !\n");
	getchar();
}


