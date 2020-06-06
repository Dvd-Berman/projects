package il.co.ilrd.gatewayserver;

import java.io.IOException;

import il.co.ilrd.gatewayserver.GatewayServer.WrongUseOfServer;


public class TestGateWayServer {
	public static void main(String[] args) throws IOException, WrongUseOfServer {
		GatewayServer gatewayServer = new GatewayServer("/home/david/Downloads/jarfolder/");
		gatewayServer.addHttpServer(ServerPort.HTTP_SERVER_PORT);
		gatewayServer.addTcpServer(ServerPort.TCP_SERVER_PORT);
		gatewayServer.addUdpServer(ServerPort.UDP_SERVER_PORT);
		
		new Thread(gatewayServer).start();
	}
}
