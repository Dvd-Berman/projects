package il.co.ilrd.chatserver;

import java.io.IOException;
import il.co.ilrd.chatserver.Server.WrongUseOfSelector;

class TestChat {
	public static void main(String[] args) throws IOException, WrongUseOfSelector {
		Server chatServer = new Server();
		chatServer.addTcpConnection(ProtocolPort.CHAT_PROTOCOL_PORT.getPort());
		new Thread(chatServer).start();
	}
}
