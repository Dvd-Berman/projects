package il.co.ilrd.chatserver;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;

import il.co.ilrd.databasemanagementserver.ObjectCovertUtil;
import il.co.ilrd.pingpong.handlers.Message;
import il.co.ilrd.pingpong.handlers.ProtocolType;
import il.co.ilrd.pingpong.handlers.ServerMessage;

class ChatClient2 {
	private ByteBuffer buffer = ByteBuffer.allocate(2048); 
	private ServerMessage message;
	private SocketChannel socket;
	private boolean toContinue = true;
	private boolean keepListening = true;
	private static final int PORT_NUM = ProtocolPort.CHAT_PROTOCOL_PORT.getPort();
	
	public static void main(String[] args) throws UnknownHostException, ClassNotFoundException, IOException {
		ChatClient2 user = new ChatClient2();
		user.runCheckInputThread();
	}
	
	private void registerToChat(String userName) throws UnknownHostException, IOException, ClassNotFoundException {
		socket = SocketChannel.open(new InetSocketAddress(InetAddress.getLocalHost(), PORT_NUM));
		message = new ServerMessage(ProtocolType.CHAT_SERVER, 
				new ChatServerMessage(ChatProtocolKeys.REGISTRATION_REQUEST, " " + userName));
		buffer.put(ObjectCovertUtil.toByteArray(message));
		keepListening = true;
		runPrintMessages();
		buffer.clear();
		socket.write(buffer);
		buffer.clear();
	}
	
	private void removerFromChat(String userName) throws UnknownHostException, IOException, ClassNotFoundException {
		keepListening = false;
		message = new ServerMessage(ProtocolType.CHAT_SERVER, 
				new ChatServerMessage(ChatProtocolKeys.REMOVE_REQUEST, userName));
		buffer.put(ObjectCovertUtil.toByteArray(message));
		buffer.clear();
		socket.write(buffer);
		buffer.clear();
	}
	
	private void sendMessage(String messageString) throws UnknownHostException, IOException, ClassNotFoundException {
		message = new ServerMessage(ProtocolType.CHAT_SERVER, 
				new ChatServerMessage(ChatProtocolKeys.MESSAGE, messageString));
		buffer.put(ObjectCovertUtil.toByteArray(message));
		buffer.clear();
		socket.write(buffer);
		buffer.clear();
	}
	
	private void runCheckInputThread() {
		
		Runnable stopRunnable = new Runnable() {
			public void run() {
				String dataString = "initString";
				String userName = null;
				
				try(BufferedReader input = new BufferedReader(new InputStreamReader(System.in))) {
					while(toContinue) {
						dataString = input.readLine(); 
						switch (dataString) {
						case "join":
							System.out.println("please enter the the user name:");
							userName = input.readLine();
							registerToChat(userName);
							break;
						case "remove":
							removerFromChat(userName);
							break;
						default:
							if(dataString.equals("exit")) {
								removerFromChat(userName);
								toContinue = false;
								break;
							}
							sendMessage(userName + ": " + dataString);
							break;
						}
					}
					
				} catch (IOException e){
					System.out.println("input thread: " + e);
				} catch (ClassNotFoundException e) {
					System.out.println("input thread: " + e);
				}
			}
		};
		new Thread(stopRunnable).start();
	}
	
	private void runPrintMessages() throws IOException, ClassNotFoundException {
		Runnable messagePrinting = new Runnable() {
			
			@Override
			public void run() {
				try {
					while(keepListening) {
						socket.read(buffer);
						ServerMessage retMessage = (ServerMessage) ObjectCovertUtil.toObject(buffer.array());
						System.out.println(retMessage.getData().toString());
						buffer.clear();			
					}									
				} catch (Exception e) {
					System.out.println("exception");
					if(keepListening) {
						throw new RuntimeException(e);
					}
				}
			}
		};
		new Thread(messagePrinting).start();
	}
}

