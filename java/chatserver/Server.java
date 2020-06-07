package il.co.ilrd.chatserver;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.Channel;
import java.nio.channels.DatagramChannel;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import il.co.ilrd.pingpong.handlers.Message;
import il.co.ilrd.pingpong.handlers.PingPongMessage;
import il.co.ilrd.pingpong.handlers.ProtocolType;
import il.co.ilrd.pingpong.handlers.ServerMessage;

public class Server implements Runnable {
	private ConnectionHandler connectionHandler;
	private MessageHandler messageHandler;

	Server() throws IOException{
		connectionHandler = new ConnectionHandler();
		messageHandler = new MessageHandler();}
	
	@Override
	public void run() {
		try {
			connectionHandler.startConnections();
			connectionHandler.run();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (WrongUseOfSelector e) {
			e.printStackTrace();
		}
		
	}

	public void stopServer() throws IOException {
		connectionHandler.stopConnections();
	}


	public void addBroadcastConnection(int portNumber)
			throws WrongUseOfSelector, IOException {
		if(true == connectionHandler.isRunning) {
			throw new WrongUseOfSelector("Can't add connections after start");
		}
		connectionHandler.addConnection(new BroadcastConnection(portNumber));
	}

	public void addTcpConnection(int portNumber) 
			throws IOException, WrongUseOfSelector {
		if(true == connectionHandler.isRunning) {
			throw new WrongUseOfSelector("Can't add connections after start");
		}
		connectionHandler.addConnection(new TcpConnection(portNumber));
	}

	public void addUdpConnection(int portNumber) 
			throws WrongUseOfSelector, IOException {
		if(true == connectionHandler.isRunning) {
			throw new WrongUseOfSelector("Can't add connections after start");
		}
		connectionHandler.addConnection(new UdpConnection(portNumber));
	}	

/******************************************************************************/
/*------------------------- Connection Interface------------------------------*/
/******************************************************************************/

	private interface Connection {

		void sendMessage(ClientInfoWrapper clientInfoWrapper, 
				ByteBuffer message) throws IOException;

		public void InitConnection() throws IOException;

		public Channel getChannel();
		
		public int getPort();
		
		public void answerClient(Channel client, 
				ByteBuffer messageBuffer) throws IOException, 
		ClassNotFoundException, WrongUseOfSelector;
	}

/******************************************************************************/
/*------------------------TCP Connection--------------------------------------*/
/******************************************************************************/

	private class TcpConnection implements Connection {
		private final int PORT;
		private ServerSocketChannel tcpServerChannel; 

		TcpConnection(int port) throws IOException{
			this.PORT = port;
			tcpServerChannel = ServerSocketChannel.open();
		}
		@Override
		public void sendMessage(ClientInfoWrapper clientInfoWrapper,
				ByteBuffer messageBuffer) throws IOException {
			while(messageBuffer.hasRemaining()) {
				clientInfoWrapper.getTcpClientChannel().write(messageBuffer);
				
			}
			messageBuffer.clear();
		}
		@Override
		public void InitConnection() throws IOException {
			tcpServerChannel.configureBlocking(false);
			tcpServerChannel.bind
			(new InetSocketAddress(InetAddress.getLocalHost(), PORT));
			tcpServerChannel.register
			(connectionHandler.selector, SelectionKey.OP_ACCEPT);
		}
		@Override
		public Channel getChannel() {
			return tcpServerChannel;
		}
		@Override
		public void answerClient(Channel clientChannel, 
				ByteBuffer messageBuffer) throws IOException,
		ClassNotFoundException, WrongUseOfSelector {
			if(-1 == ((SocketChannel)clientChannel).read(messageBuffer)) {
				connectionHandler.connectionMap.remove(clientChannel);
				connectionHandler.connectionList.remove(this);
				clientChannel.close();
				System.out.println("client is closed");
			}
			else {
				messageHandler.handleMessage(messageBuffer, 
						new ClientInfoWrapper(this, (SocketChannel) clientChannel));
			}		 
		}
		@Override
		public int getPort() {
			return PORT;
		}
	}

/******************************************************************************/
/*------------------------UDP Connection--------------------------------------*/
/******************************************************************************/

	private class UdpConnection implements Connection {
		private final int PORT;
		private  DatagramChannel udpServerChannel;

		UdpConnection(int port) throws IOException{
			this.PORT = port;
			udpServerChannel = DatagramChannel.open();
		}

		@Override
		public void InitConnection() throws IOException {
			udpServerChannel.configureBlocking(false);
			udpServerChannel.socket().bind
			(new InetSocketAddress(InetAddress.getLocalHost(),PORT));
			udpServerChannel.register
			(connectionHandler.selector, SelectionKey.OP_READ);
		}

		@Override
		public Channel  getChannel() {
			return udpServerChannel;
		}

		@Override
		public void answerClient(Channel serverChannel, 
				ByteBuffer messageBuffer) throws IOException, 
		ClassNotFoundException, WrongUseOfSelector {
			SocketAddress clientAddress = 
					((DatagramChannel) serverChannel).receive(messageBuffer);		
			messageHandler.handleMessage
			(messageBuffer, new ClientInfoWrapper(this, clientAddress));		
		}

		@Override
		public void sendMessage
		(ClientInfoWrapper clientInfoWrapper, ByteBuffer messageBuffer)
				throws IOException {
			messageBuffer.flip();
			((DatagramChannel) this.getChannel()).send
			(messageBuffer, clientInfoWrapper.getUdpClientAddress());
			messageBuffer.clear();	
		}

		@Override
		public int getPort() {
			
			return PORT;
		}
	}

/******************************************************************************/
/*------------------------BroadCast Connection--------------------------------*/
/******************************************************************************/

	private class BroadcastConnection implements Connection {
		private final int PORT;
		private DatagramChannel broadcastServerChannelListener;
		private DatagramChannel ServerChannelSender; 

		BroadcastConnection(int port) throws IOException{
			this.PORT = port;
		}

		@Override
		public void InitConnection() throws IOException {
			broadcastServerChannelListener = DatagramChannel.open();
			broadcastServerChannelListener.configureBlocking(false);
			broadcastServerChannelListener.socket().bind(
					new InetSocketAddress
					(InetAddress.getByName("127.255.255.255"), PORT));
			broadcastServerChannelListener.
			register(connectionHandler.selector, SelectionKey.OP_READ);	
			ServerChannelSender = DatagramChannel.open();
			ServerChannelSender.configureBlocking(false);
			ServerChannelSender.socket().bind
			(new InetSocketAddress(InetAddress.getLocalHost(), PORT));
		}

		@Override
		public Channel getChannel() {
			return broadcastServerChannelListener;
		}

		public DatagramChannel getSenderChannel() {
			return ServerChannelSender;
		}

		@Override
		public void answerClient(Channel clientChannel,
				ByteBuffer messageBuffer) 
						throws ClassNotFoundException, IOException, WrongUseOfSelector {
			SocketAddress clientAddress = 
					((DatagramChannel)clientChannel).receive(messageBuffer);		
			messageHandler.handleMessage
			(messageBuffer, new ClientInfoWrapper(this, clientAddress));						
		}

		@Override
		public void sendMessage
		(ClientInfoWrapper clientInfoWrapper, ByteBuffer messageBuffer) 
				throws IOException {
			BroadcastConnection broadcastConnection = 
					(BroadcastConnection) clientInfoWrapper.getConnection();
			messageBuffer.flip();
			(this.getSenderChannel()).send
			(messageBuffer, clientInfoWrapper.getUdpClientAddress());
			messageBuffer.clear();				
		}

		@Override
		public int getPort() {
			
			return PORT;
		}
	}

/******************************************************************************/
/*------------------------ConnectionHandler Connection------------------------*/
/******************************************************************************/

	private class ConnectionHandler implements Runnable {
		private Selector selector;
		private List<Connection> connectionList;
		private Map<Channel, Connection> connectionMap;
		private ByteBuffer messageBuffer;
		private static final int BYTE_CAPACITY = 2048;
		private boolean isRunning = false;
		private final long selectTimeout = 5000;

		ConnectionHandler() throws IOException{
			connectionList  = new ArrayList<>();
			connectionMap = new HashMap<>();
			messageBuffer = ByteBuffer.allocate(BYTE_CAPACITY);
		}

		private void startConnections() throws IOException, WrongUseOfSelector {
			selector = Selector.open();
			if(connectionList.isEmpty()) {
				throw new WrongUseOfSelector("No Connections Present");
			}
			for(Connection connectionRunner : connectionList) {
				connectionRunner.InitConnection();
				connectionMap.put(connectionRunner.getChannel(), connectionRunner);
				System.out.println("I'm Here");
			}
		}

		private void stopConnections() throws IOException {
			isRunning = false;
			closeResource();
		}

		private void addConnection(Connection connection) {
			connectionList.add(connection);
		}
		

		private void removeConnection(Connection connection) throws IOException {
			connection.getChannel().close();
			connectionList.remove(connection);
		}

		@Override
		public void run() {
			isRunning = true;
			try {
				while(true) {
					if(0 == selector.select(selectTimeout)) {
						System.out.println("server running");
						continue;
					}
					Set<SelectionKey> selectedKeys = selector.selectedKeys();
					Iterator<SelectionKey> iter = selectedKeys.iterator();
					while(iter.hasNext()) {
						SelectionKey key = iter.next();
						if(key.isAcceptable()) {
							System.out.println("Accetable");
							registerClientSoket(key);
						}
						if(key.isReadable()) {	
							System.out.println("Readable");
							Channel clientChannel =  key.channel();
							connectionMap.get(clientChannel).answerClient
							(clientChannel, messageBuffer);						
						}
					}
					iter.remove();
				}

			} catch(Exception e) {
				if(isRunning) {
					throw new RuntimeException(e);
				}
			}
		}

		private void registerClientSoket(SelectionKey key) throws IOException {
			ServerSocketChannel serverChannel = 
					(ServerSocketChannel)key.channel();
			SocketChannel client = serverChannel.accept();
			client.configureBlocking(false);
			connectionMap.put(client, connectionMap.get(serverChannel));
			client.register(selector, SelectionKey.OP_READ);
		}
		
		private void closeResource() {			
			try {
				Set<SelectionKey> selectedKeys = selector.keys();
				
				for (SelectionKey Key : selectedKeys) {
					Key.channel().close();
				}
				
				selector.close();
			} catch (IOException e) {
				System.out.println("close resource" + e);
			}
		}
	}
	
/******************************************************************************/
/*------------------------Protocol--------------------------------------------*/
/******************************************************************************/

	private interface Protocol{
		public void handleMessage
		(ClientInfoWrapper clientInfoWrapper, Message<?, ?> message,
				ByteBuffer messageBuffer) 
						throws IOException, WrongUseOfSelector;
	}

/******************************************************************************/
/*------------------------PingPongProtocol------------------------------------*/
/******************************************************************************/

	private class PingPongProtocol implements Protocol{
		private static final String PING = "ping"; 
		private static final String PONG = "pong";
		private final ServerMessage pingMessage = 
				new ServerMessage
				(ProtocolType.PINGPONG, new PingPongMessage("ping", null));
		private final ServerMessage pongMessage = 
				new ServerMessage
				(ProtocolType.PINGPONG, new PingPongMessage("pong", null));

		private void SendMessageToClient(ByteBuffer messageBuffer, 
				ClientInfoWrapper clientInfoWrapper) throws IOException {
			clientInfoWrapper.getConnection().sendMessage
			(clientInfoWrapper, messageBuffer);							
		}


		private void FillBufferWithReturnMessage(Message<?, ?> message,
				ByteBuffer messageBuffer) throws IOException {
			String inputmessage = (String) message.getKey();
			if(inputmessage.equals(PING)) {
				messageBuffer.put(ObjectConvertUtil.toByteArray(pongMessage));
			} else if(inputmessage.equals(PONG)) {
				messageBuffer.put(ObjectConvertUtil.toByteArray(pingMessage));
			}		
		}


		@Override
		public void handleMessage(ClientInfoWrapper clientInfoWrapper, 
				Message<?, ?> message, ByteBuffer messageBuffer)
						throws IOException {
			messageBuffer.clear();
			FillBufferWithReturnMessage(message, messageBuffer);
			messageBuffer.flip();
			SendMessageToClient(messageBuffer, clientInfoWrapper);
		}

	}
	
/******************************************************************************/
/*------------------------ChatProtocol----------------------------------------*/
/******************************************************************************/	
	
	private class ChatProtocol implements Protocol{
		private Map<SocketChannel, String> registeredChatClients;
		private Map<ChatProtocolKeys,
			TriFunction<ClientInfoWrapper, ChatServerMessage, ByteBuffer>> clientRequestHandlerMap;
		private final ChatServerMessage nameInUse = new ChatServerMessage(
				ChatProtocolKeys.REGISTRATION_REFUSE, "Sorry This name exists in the chat");
		private final ChatServerMessage channelInUse = new ChatServerMessage(
				ChatProtocolKeys.REGISTRATION_REFUSE, "Sorry this channel is connected already");
		private final ChatServerMessage notRegisteredMessage = new ChatServerMessage(
				ChatProtocolKeys.ERROR_MESSAGE, "OOPS, gotta register first");
		private final ChatServerMessage wrongKeyMessage = new ChatServerMessage(ChatProtocolKeys.ERROR_MESSAGE, "Sorry wrong ChatKey");
		
		private ChatProtocol(){
			registeredChatClients = new HashMap<>();
			clientRequestHandlerMap = new HashMap<>();
			addHandlersToMap();
		}

		private void addHandlersToMap() {
			 clientRequestHandlerMap.put(ChatProtocolKeys.REGISTRATION_REQUEST, new registerChatClientHandler());
			 clientRequestHandlerMap.put(ChatProtocolKeys.MESSAGE, new ChatMessageHandler());
			 clientRequestHandlerMap.put(ChatProtocolKeys.REMOVE_REQUEST, new RemoveClientHandler());
			 clientRequestHandlerMap.put(ChatProtocolKeys.BROADCAST_MESSAGE, new WrongKeyHandler());
			 clientRequestHandlerMap.put(ChatProtocolKeys.ERROR_MESSAGE, new WrongKeyHandler());
			 clientRequestHandlerMap.put(ChatProtocolKeys.NEW_CLIENT_REGISTRATION, new WrongKeyHandler());
			 clientRequestHandlerMap.put(ChatProtocolKeys.REGISTRATION_ACK, new WrongKeyHandler());
			 clientRequestHandlerMap.put(ChatProtocolKeys.REGISTRATION_REFUSE, new WrongKeyHandler());
		}

		@Override
		public void handleMessage(ClientInfoWrapper clientInfoWrapper,
				Message<?, ?> chatMessage, ByteBuffer messageBuffer)
				throws IOException, WrongUseOfSelector {
			
			if(!(clientInfoWrapper.getConnection() instanceof TcpConnection) ||
				clientInfoWrapper.getConnection().getPort() !=
						ProtocolPort.CHAT_PROTOCOL_PORT.getPort()) {
				throw new WrongUseOfSelector("Wrong Port Or Connection");
			}
			
			ChatProtocolKeys clientRequest = 
					(ChatProtocolKeys) chatMessage.getKey();
			clientRequestHandlerMap.get(clientRequest).apply(clientInfoWrapper,
					(ChatServerMessage)chatMessage, messageBuffer);
		}
		
		private class registerChatClientHandler implements 
		TriFunction<ClientInfoWrapper, ChatServerMessage , ByteBuffer> {
			@Override
			public void apply(ClientInfoWrapper clientInfoWrapper, 
					ChatServerMessage chatMessage, ByteBuffer messageBuffer)
															throws IOException {
				if(registeredChatClients.containsValue(chatMessage.getData())) {
					chatMessage = nameInUse;
				}
				else if(registeredChatClients.containsKey
						(clientInfoWrapper.getTcpClientChannel())){
					chatMessage = channelInUse;
				}
				
				else {
					registeredChatClients.put
					(clientInfoWrapper.getTcpClientChannel(),
									chatMessage.getData());
					ChatServerMessage broadcastMessage = new ChatServerMessage(
							ChatProtocolKeys.NEW_CLIENT_REGISTRATION, 
										chatMessage.getData() + " joined" );
					messageBuffer.clear();
					fillBufferWithReturnMessage(messageBuffer, broadcastMessage);
					messageBuffer.flip();
					sendMessageToOtherClients(messageBuffer,clientInfoWrapper);
					chatMessage = 
							new ChatServerMessage(ChatProtocolKeys.REGISTRATION_ACK, 
												"Welcome" + chatMessage.getData());
				}
				messageBuffer.clear();
				fillBufferWithReturnMessage(messageBuffer, chatMessage);
				messageBuffer.flip();
				sendMessageToClient(messageBuffer, clientInfoWrapper);
			}
		}
		
		private class ChatMessageHandler implements TriFunction<ClientInfoWrapper, ChatServerMessage , ByteBuffer> {
			@Override
			public void apply(ClientInfoWrapper clientInfoWrapper, 
					ChatServerMessage chatServerMessage, ByteBuffer messageBuffer)
							throws IOException {
				if(isRegisteredHandler(chatServerMessage, messageBuffer, clientInfoWrapper)){
					chatServerMessage =  new ChatServerMessage(ChatProtocolKeys.BROADCAST_MESSAGE, chatServerMessage.getData());						
					messageBuffer.clear();
					fillBufferWithReturnMessage(messageBuffer, chatServerMessage);
					messageBuffer.flip();
					sendMessageToOtherClients(messageBuffer, clientInfoWrapper);
				}
				else {
					messageBuffer.clear();
					fillBufferWithReturnMessage(messageBuffer, notRegisteredMessage);
					messageBuffer.flip();
					sendMessageToClient(messageBuffer, clientInfoWrapper);
				}
			}
		}
		
		private class RemoveClientHandler implements TriFunction<ClientInfoWrapper, ChatServerMessage , ByteBuffer> {

			@Override
			public void apply(ClientInfoWrapper clientInfoWrapper, 
					ChatServerMessage chatServerMessage, ByteBuffer messageBuffer)
									throws IOException {
				if(isRegisteredHandler(chatServerMessage, messageBuffer, clientInfoWrapper)){
					connectionHandler.removeConnection(clientInfoWrapper.getConnection());
					chatServerMessage =
							new ChatServerMessage(ChatProtocolKeys.BROADCAST_MESSAGE, 
											chatServerMessage.getData() + " left");
					messageBuffer.clear();
					fillBufferWithReturnMessage(messageBuffer, chatServerMessage);
					messageBuffer.flip();
					sendMessageToOtherClients(messageBuffer, clientInfoWrapper);
				}
				else {
					messageBuffer.clear();
					fillBufferWithReturnMessage(messageBuffer, notRegisteredMessage);
					messageBuffer.flip();
					sendMessageToClient(messageBuffer, clientInfoWrapper);
				}
			}
		}
		private class WrongKeyHandler implements TriFunction<ClientInfoWrapper, ChatServerMessage , ByteBuffer> {

			@Override
			public void apply(ClientInfoWrapper clientInfoWrapper, ChatServerMessage chatMessage, ByteBuffer messageBuffer)
					throws IOException {
				messageBuffer.clear();
				fillBufferWithReturnMessage(messageBuffer, wrongKeyMessage);
				messageBuffer.flip();
				sendMessageToClient(messageBuffer, clientInfoWrapper);
			}	
		}
		
		private boolean isRegisteredHandler(ChatServerMessage message, ByteBuffer messageBuffer, 
				ClientInfoWrapper clientInfoWrapper) throws IOException {
			if(!registeredChatClients.containsKey(clientInfoWrapper.getTcpClientChannel())) { 
				
				return false;
			}
			return true;
		}
		
/******************************************************************************/
		
		private void sendMessageToOtherClients(ByteBuffer messageBuffer,
				ClientInfoWrapper clientInfoWrapper) throws IOException {
			SocketChannel currClientChannel =  clientInfoWrapper.getTcpClientChannel();
			Connection mutualConnection = clientInfoWrapper.getConnection();
			
			for (SocketChannel channelRunner : registeredChatClients.keySet()) {
				if(!channelRunner.equals(currClientChannel)) {
					mutualConnection.sendMessage(new ClientInfoWrapper(mutualConnection,
							channelRunner), messageBuffer);
				}
			}
		}
		private void sendMessageToClient(ByteBuffer messageBuffer, 
				ClientInfoWrapper clientInfoWrapper) throws IOException {
			clientInfoWrapper.getConnection().sendMessage
			(clientInfoWrapper, messageBuffer);							
		}
		
		private void fillBufferWithReturnMessage(ByteBuffer messageBuffer,
				ChatServerMessage chatMessage) throws IOException {
			ServerMessage returnMessage = new ServerMessage(ProtocolType.CHAT_SERVER, chatMessage);
			messageBuffer.put(ObjectConvertUtil.toByteArray(returnMessage));
		}
	}

/******************************************************************************/
/*------------------------MessageHandler--------------------------------------*/
/******************************************************************************/

	private class MessageHandler {
		private Map<ProtocolType, Protocol> protocolMap;

		private MessageHandler(){
			protocolMap = new HashMap<>();
			addProtocol(ProtocolType.PINGPONG, new PingPongProtocol());
			addProtocol(ProtocolType.CHAT_SERVER, new ChatProtocol());
		}

		@SuppressWarnings("unlikely-arg-type")
		private void handleMessage(ByteBuffer messageBuffer,
				ClientInfoWrapper clientInfoWrapper) 
						throws IOException, ClassNotFoundException, WrongUseOfSelector {
			messageBuffer.clear();
			byte[] byteArr = messageBuffer.array();
			@SuppressWarnings("unchecked")
			Message<ProtocolType, Message<?, ?>> message = 
					(Message<ProtocolType, Message<?, ?>>) ObjectConvertUtil.toObject(byteArr); 	
			protocolMap.get(message.getKey()).handleMessage
			(clientInfoWrapper, message.getData(), messageBuffer);
		}


		private void addProtocol(ProtocolType pingPongProtocol, Protocol protocol) {
			protocolMap.put(pingPongProtocol, protocol);
		}

		private void removeProtocol(Protocol protocol) {
			protocolMap.remove(protocol);
		}
	}

/******************************************************************************/	

	private class ClientInfoWrapper {
		private SocketChannel tcpClientChannel;
		private SocketAddress udpClientAddress;
		private Connection connection;

		public ClientInfoWrapper
		(Connection connection, SocketChannel tcpClientChannel) {
			this.tcpClientChannel = tcpClientChannel;
			this.connection = connection;
		}

		public ClientInfoWrapper
		(Connection connection, SocketAddress udpClientAddress) {
			this.udpClientAddress = udpClientAddress;
			this.connection = connection;
		}

		private SocketChannel getTcpClientChannel() {
			return tcpClientChannel;
		}

		private SocketAddress getUdpClientAddress() {
			return udpClientAddress;
		}

		private Connection getConnection() {
			
			return connection;
		}
	}
	
/******************************************************************************/
	
	private interface TriFunction<T, U, W>{
		void apply(T firstArg, U secondArg, W thirdArg) throws IOException;
	}

/******************************************************************************/

	public class WrongUseOfSelector extends Exception { 
		private static final long serialVersionUID = 1L;

		public WrongUseOfSelector(String errorMessage) {
			super(errorMessage);
		}
	}
}
