package il.co.ilrd.gatewayserver;
import static java.nio.file.StandardWatchEventKinds.ENTRY_CREATE;
import static java.nio.file.StandardWatchEventKinds.ENTRY_DELETE;
import static java.nio.file.StandardWatchEventKinds.ENTRY_MODIFY;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.Channel;
import java.nio.channels.DatagramChannel;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.nio.charset.Charset;
import java.nio.file.ClosedWatchServiceException;
import java.nio.file.FileSystems;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.WatchEvent;
import java.nio.file.WatchKey;
import java.nio.file.WatchService;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;
import java.util.concurrent.SynchronousQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;
import com.sun.net.httpserver.Headers;
import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;
import il.co.ilrd.http_message.HttpBuilder;
import il.co.ilrd.http_message.HttpParser;
import il.co.ilrd.http_message.HttpStatusCode;
import il.co.ilrd.http_message.HttpVersion;
import il.co.ilrd.jarloader.JarLoader;
import il.co.ilrd.observer.Callback;
import il.co.ilrd.observer.Dispatcher;


public class GatewayServer implements Runnable {
	private ThreadPoolExecutor threadPool;
	private CMDFactory<FactoryCommand, String, Void> cmdFactory = CMDFactory.getFactory();
	private IOTTaskCreator iotTaskCreator = new IOTTaskCreator();
	private ConnectionHandler connectionHandler = new ConnectionHandler();
	private Map<String, DatabaseManagement> companyDbManagementMap = new HashMap<>();
	private MessageHandler messageHandler = new MessageHandler();
	private FactoryCommandLoader factoryCommandLoader = new FactoryCommandLoader();
	private final static int DEAFULT_NUM_THREADS = Runtime.getRuntime().availableProcessors();
	private static final int BUFFER_CAPACITY = 4096;
	private JarMonitor jarMonitor = null;
	private static final String URL = "jdbc:mysql://localhost/";
	private static final String USER_NAME = "root";
	private static final String PASSWORD = "haymonlimon";
	private static final String JAR_SUFFIX = ".jar";


	@Override
	public void run() {
		this.start();
	}

	public GatewayServer(int numOfThreads, String dirPath) throws IOException {
		threadPool = new ThreadPoolExecutor(numOfThreads, Integer.MAX_VALUE, 1, 
				TimeUnit.DAYS, new SynchronousQueue<Runnable>());
		jarMonitor = new JarMonitor(dirPath);
		factoryCommandLoader.addCMDFromJarDir(dirPath);
	}

	public GatewayServer(String dirPath) throws IOException {
		this(DEAFULT_NUM_THREADS, dirPath);
	}

	public void addHttpServer(ServerPort serverPort) throws WrongUseOfServer, IOException {
		checkIfStarted();
		int portNum = serverPort.getPort();
		//connectionHandler.addHighLevelHttpServer(portNum);
		connectionHandler.addConnection(new LowLevelHttpServer(portNum));
	}

	public void addTcpServer(ServerPort serverPort) throws WrongUseOfServer {
		checkIfStarted();
		int portNum = serverPort.getPort();
		connectionHandler.addConnection(new TcpServer(portNum));
	}

	public void addUdpServer(ServerPort serverPort) throws WrongUseOfServer {
		checkIfStarted();
		int portNum = serverPort.getPort();
		connectionHandler.addConnection(new UdpServer(portNum));
	}

	public void start() {
		try {
			connectionHandler.startConnections();
			connectionHandler.run();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (WrongUseOfServer e) {
			e.printStackTrace();
		}
	}

	public void stop() throws IOException {
		connectionHandler.stopConnections();
	}

	public void setNumOfThreads(int numOfThread) {
		threadPool.setCorePoolSize(numOfThread);
	}

/******************************************************************************/
/*------------------------ConnectionHandler Connection------------------------*/
/******************************************************************************/

	private class ConnectionHandler implements Runnable {
		private Selector selector;
		private List<ServerConnection> connectionList = new ArrayList<>();
		private Map<Channel, ServerConnection> connectionMap = new HashMap<>();
		private Map<SocketChannel, ClientInfoWrapper> socketClientInfo = new HashMap<>();
		private boolean isRunning = false;
		private final long selectTimeout = 5000;
		private HighLevelHttpServer highLevelHttpServer = null;

		private void addHighLevelHttpServer(int portNumber) throws IOException {
			highLevelHttpServer = new HighLevelHttpServer(portNumber);
			connectionList.add(highLevelHttpServer);
		}

		private void startConnections() throws IOException, WrongUseOfServer {
			if(noConnections()) {
				throw new WrongUseOfServer("No Connections Present");
			}
			startJarMonitor();
			selector = Selector.open();

			for(ServerConnection connectionRunner : connectionList) {
				connectionRunner.initServerConnection();
				connectionMap.put(connectionRunner.getChannel(), connectionRunner);
			}
			if(null != highLevelHttpServer) {
				highLevelHttpServer.start();
			}
		}

		private void startJarMonitor() {
			Callback<String> callback = 
					new Callback<String>((param)->factoryCommandLoader.addToFactory(param), null);
			jarMonitor.register(callback);
			new Thread(jarMonitor).start();
		}

		private boolean noConnections() {
			return connectionList.isEmpty();
		}

		private void stopConnections() throws IOException {
			isRunning = false;
			closeResource();
		}

		private void addConnection(ServerConnection connection) {
			connectionList.add(connection);
		}


		private void removeConnection(ServerConnection connection) throws IOException {
			connection.getChannel().close();
			connectionList.remove(connection);
		}

		@Override
		public void run() {
			isRunning = true;
			try {
				while(isRunning) {
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
							connectionMap.get(clientChannel).handleRequestMessage(socketClientInfo.get(clientChannel));
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
			ServerConnection connection = connectionMap.get(serverChannel);
			SocketChannel client = serverChannel.accept();
			client.configureBlocking(false);
			connectionMap.put(client,connection);
			client.register(selector, SelectionKey.OP_READ);
			socketClientInfo.put(client, new ClientInfoWrapper(connection, client));
		}

		private void closeResource() {			
			try {
				Set<SelectionKey> selectedKeys = selector.keys();
				for (SelectionKey Key : selectedKeys) {
					Key.channel().close();
				}

				if(null != highLevelHttpServer) {
					highLevelHttpServer.stopServer();
				}
				threadPool.shutdown();
				selector.close();
			} catch (IOException e) {
				System.out.println("close resource" + e);
			}
		}
	}

/******************************************************************************/
/*------------------------MessageHandler--------------------------------------*/
/******************************************************************************/

	private class MessageHandler {
		private static final String JSON_SYNTAX_ERROR = 
				"{ \"errorMessage\" : \"json syntax error\" }"; 
		private static final String COMMAND_KEY_ERROR = 
				"{ \"errorMessage\" : \"commandKey not found\" }"; 
		private static final String JSON_FIELD_NAME_ERROR = 
				"{ \"errorMessage\" : \"json wrong field name\" }"; 

		private void handleMessage(String bodyJsonString,
				ClientInfoWrapper clientInfoWrapper) throws IOException {
			Runnable runnable;
			try {
				runnable = iotTaskCreator.createRunnableTask(bodyJsonString, clientInfoWrapper);
				threadPool.submit(runnable);			
			} catch (ParseException e) {
				clientInfoWrapper.getConnection().
				sendErrorResponseMessage(JSON_SYNTAX_ERROR, clientInfoWrapper);
			} catch (IllegalArgumentException | ClassCastException e) {
				clientInfoWrapper.getConnection().
				sendErrorResponseMessage(COMMAND_KEY_ERROR, clientInfoWrapper);
			} catch (NullPointerException e) {
				clientInfoWrapper.getConnection().
				sendErrorResponseMessage(JSON_FIELD_NAME_ERROR, clientInfoWrapper);
			}
		}
	}

/******************************************************************************/
/*------------------------interface ServerConnection -------------------------*/

	private interface ServerConnection {
		public void initServerConnection() throws IOException;
		public void handleRequestMessage(ClientInfoWrapper clientInfoWrapper) throws IOException;
		public void sendResponseMessage(String jsonString,
				ClientInfoWrapper clientInfoWrapper) throws IOException;
		public void sendErrorResponseMessage(String jsonString,
				ClientInfoWrapper clientInfoWrapper) throws IOException;
		public Channel getChannel();
	}


/******************************************************************************/
/*------------------------ class HighLevelHttpServer--------------------------*/			

	class HighLevelHttpServer implements ServerConnection {
		private HttpServer httpServer;
		private final int portNumber;

		HighLevelHttpServer(int portNumber) throws IOException {
			this.portNumber = portNumber;
		}

		private void start() {
			httpServer.start();
		}

		private void stopServer (){
			httpServer.stop(0);
		}

		private class DefaultHandler implements HttpHandler {
			@Override
			public void handle(HttpExchange httpExchange) throws IOException {			
				try {
					handleRequestMessage(new ClientInfoWrapper(HighLevelHttpServer.this, httpExchange));
				} catch (IOException e) {
					e.printStackTrace();
				}				
			}
		}

		@Override
		public void initServerConnection() throws IOException {
			httpServer = HttpServer.create(new InetSocketAddress(portNumber), 0);
			httpServer.createContext("/", new DefaultHandler());
			httpServer.setExecutor(null);
		}

		@Override
		public void handleRequestMessage(ClientInfoWrapper clientInfo) throws IOException {
			String bodyJsonString = getBodyJsonString(clientInfo.getHttpExchange());
			messageHandler.handleMessage(bodyJsonString, clientInfo);
		}

		@Override
		public void sendResponseMessage(String jsonString,
				ClientInfoWrapper clientInfo) throws IOException {
			sendResponse(clientInfo.getHttpExchange(), HttpStatusCode.OK, jsonString);
		}

		@Override
		public void sendErrorResponseMessage(String jsonString,
				ClientInfoWrapper clientInfo) throws IOException {
			sendResponse(clientInfo.getHttpExchange(), 
					HttpStatusCode.BAD_REQUEST, jsonString);
		}

		private String getBodyJsonString(HttpExchange httpExchange) throws IOException {
			byte[] byteArray = new byte[BUFFER_CAPACITY];
			try(
					InputStream inputStream = httpExchange.getRequestBody();
					) {
				inputStream.read(byteArray);
				return new String(byteArray).trim();
			}
		}

		private void sendResponse(HttpExchange httpExchange, HttpStatusCode statusCode, String responseBody)
				throws IOException {
			fillHeadersWithStandardHeaders(httpExchange);
			int responselength = responseBody.length();
			httpExchange.sendResponseHeaders(statusCode.getCode(), responselength);
			try (OutputStream outputStream = httpExchange.getResponseBody();) {
				outputStream.write(responseBody.getBytes());
			}
		}

		@Override
		public Channel getChannel() {
			return null;
		}
	}

/******************************************************************************/
/*------------------------ class LowLevelHttpServer---------------------------*/	

	private class LowLevelHttpServer implements ServerConnection {
		private final int PORT;
		private ServerSocketChannel tcpServerChannel;
		private Map<String, String> headersMap = new HashMap<>();
		private ByteBuffer byteBuffer = ByteBuffer.allocate(BUFFER_CAPACITY);

		LowLevelHttpServer(int port) throws IOException {
			this.PORT = port;
			tcpServerChannel = ServerSocketChannel.open();
		}

		@Override
		public void initServerConnection() throws IOException {
			tcpServerChannel = ServerSocketChannel.open();
			tcpServerChannel.configureBlocking(false);
			tcpServerChannel.bind(new InetSocketAddress(PORT));
			tcpServerChannel.register(connectionHandler.selector, 
					SelectionKey.OP_ACCEPT);			
		}

		@Override
		public void handleRequestMessage(ClientInfoWrapper clientInfo) throws IOException {
			String httpString = getMessageString(byteBuffer, clientInfo);
			if (null != httpString) {
				HttpParser httpParser = new HttpParser(httpString);
				messageHandler.handleMessage(httpParser.getBody().getBodyString(), clientInfo);				
			}
		}

		@Override
		public void sendResponseMessage(String jsonString, ClientInfoWrapper clientInfo) throws IOException {
			sendResponse(HttpStatusCode.OK, jsonString, clientInfo);
		}

		@Override
		public void sendErrorResponseMessage(String jsonString, ClientInfoWrapper clientInfo) throws IOException {
			sendResponse(HttpStatusCode.BAD_REQUEST, jsonString, clientInfo);
		}

		@Override
		public Channel getChannel() {
			return tcpServerChannel;
		}

		private void fillHeafersMapWithStandardHeaders(int responseLength) {
			Integer resBodySize = responseLength;
			headersMap.put("Content-Length", resBodySize.toString());
			headersMap.put("Content-Type", "application/json");
		}

		private void sendResponse(HttpStatusCode statusCode, 
				String responseBody, ClientInfoWrapper clientInfoWrapper) throws IOException {
			ByteBuffer responseByteBuffer = getResponseByteBuffer(statusCode, responseBody);
			while (responseByteBuffer.hasRemaining()) {
				clientInfoWrapper.getTcpClientChannel().write(responseByteBuffer);
			}
		}

		private ByteBuffer getResponseByteBuffer(HttpStatusCode statusCode, String responseBody) {
			fillHeafersMapWithStandardHeaders(responseBody.length());
			String responseMessage = 
					HttpBuilder.createHttpResponseMessage(HttpVersion.HTTP_1_1,
							statusCode, headersMap, responseBody);
			return ByteBuffer.wrap(responseMessage.getBytes());
		}
	}

/******************************************************************************/
/*------------------------ class TcpServer------------------------------------*/				

	private class TcpServer implements ServerConnection {
		private ByteBuffer byteBuffer = ByteBuffer.allocate(BUFFER_CAPACITY);
		private ServerSocketChannel tcpServerChannel;
		private final int port; 

		public TcpServer(int port) {
			this.port = port;
		}

		@Override
		public void initServerConnection() throws IOException {
			tcpServerChannel = ServerSocketChannel.open();
			tcpServerChannel.configureBlocking(false);
			tcpServerChannel.bind(new InetSocketAddress(port));
			tcpServerChannel.register(connectionHandler.selector, SelectionKey.OP_ACCEPT);
		}

		@Override
		public void handleRequestMessage(ClientInfoWrapper clientInfo) throws IOException {
			String bodyString = getMessageString(byteBuffer, clientInfo);
			if(null != bodyString) {
				messageHandler.handleMessage(bodyString, clientInfo);				
			}
		}

		@Override
		public void sendResponseMessage(String jsonString,
				ClientInfoWrapper clientInfo) throws IOException {
			ByteBuffer byteBuffer = Charset.forName("ASCII").encode(jsonString);
			while (byteBuffer.hasRemaining()) {
				clientInfo.getTcpClientChannel().write(byteBuffer);
			}
		}

		@Override
		public void sendErrorResponseMessage(String jsonString, 
				ClientInfoWrapper clientInfo) throws IOException {
			sendResponseMessage(jsonString, clientInfo);
		}

		@Override
		public Channel getChannel() {
			return tcpServerChannel;
		}
	}

/******************************************************************************/
/*------------------------ class UdpServer------------------------------------*/			

	private class UdpServer implements ServerConnection {
		private ByteBuffer byteBuffer = ByteBuffer.allocate(BUFFER_CAPACITY);
		private DatagramChannel udpServerChannel;
		private final int port;

		public UdpServer(int port) {
			this.port = port;
		}

		@Override
		public void initServerConnection() throws IOException {
			udpServerChannel = DatagramChannel.open();
			udpServerChannel.configureBlocking(false);
			udpServerChannel.socket().bind(new InetSocketAddress(port));
			udpServerChannel.register(connectionHandler.selector, SelectionKey.OP_READ);			
		}

		@Override
		public void handleRequestMessage(ClientInfoWrapper clientInfo) throws IOException {
			byteBuffer.clear();
			SocketAddress socketAddress = udpServerChannel.receive(byteBuffer);
			if (null == socketAddress) {
				System.out.println("null");
			}
			byteBuffer.flip();
			String bodyString = 
					new String(Charset.forName("ASCII").decode(byteBuffer).array());
			messageHandler.handleMessage(bodyString, new ClientInfoWrapper(this, socketAddress));
		}

		@Override
		public void sendResponseMessage(String jsonString, ClientInfoWrapper clientInfo) throws IOException {
			ByteBuffer byteBuffer = Charset.forName("ASCII").encode(jsonString);			
			udpServerChannel.send(byteBuffer, clientInfo.getUdpClientAddress());
		}

		@Override
		public void sendErrorResponseMessage(String jsonString,
				ClientInfoWrapper clientInfo) throws IOException {
			sendResponseMessage(jsonString, clientInfo);
		}

		@Override
		public Channel getChannel() {
			return udpServerChannel;
		}
	}

/******************************************************************************/
/*------------------------ClientInfoWrapper-----------------------------------*/
/******************************************************************************/

	private class ClientInfoWrapper {
		private SocketChannel tcpClientChannel;
		private SocketAddress udpClientAddress;
		private ServerConnection connection;
		private HttpExchange httpExchange;


		public ClientInfoWrapper
		(ServerConnection connection, SocketChannel tcpClientChannel) {
			this.tcpClientChannel = (SocketChannel) tcpClientChannel;
			this.connection = connection;
		}

		public ClientInfoWrapper
		(ServerConnection connection, SocketAddress udpClientAddress) {
			this.udpClientAddress = udpClientAddress;
			this.connection = connection;
		}

		public ClientInfoWrapper(ServerConnection connection, 
				HttpExchange httpExchange) {
			this.httpExchange = httpExchange;
			this.connection = connection;
		}

		private SocketChannel getTcpClientChannel() {

			return tcpClientChannel;
		}

		private SocketAddress getUdpClientAddress() {

			return udpClientAddress;
		}

		private HttpExchange getHttpExchange() {

			return httpExchange;
		}

		private ServerConnection getConnection() {

			return connection;
		}
	}

/******************************************************************************/
/*------------------------private classes-------------------------------------*/
/******************************************************************************/

	private class FactoryCommandLoader{
		private static final String FACTORY_CMD_INTERFACE = "FactoryCommandModifier";
		private Map<String, Double> versionsMap = new HashMap<>(); 
		
		private void addCMDFromJarDir(String jarDirPath){
			File dirFile = new File(jarDirPath);
			for(File fileIter : dirFile.listFiles()) {
				if(fileIter.getName().endsWith(JAR_SUFFIX)) {
					addToFactory(fileIter.getAbsolutePath());
				}
			}
		}

		private List<Class<?>> load(String jarFilePath)
								throws ClassNotFoundException, IOException {

			return JarLoader.load(FACTORY_CMD_INTERFACE, jarFilePath);
		}
		
		private void addToFactory(String jarFilePath) {
			
			try {
				List<Class<?>> classList = factoryCommandLoader.load(jarFilePath);
				for(Class<?> currClass : classList) {
					FactoryCommandModifier currFCM = (FactoryCommandModifier) currClass.getConstructor().newInstance();
					String className = currClass.getSimpleName();
					double currVersion = currFCM.getVersion();
					
					if(isNewClass(className) || isNewVersion(className, currVersion)) {
						System.out.println("Adding this class" + className);
						versionsMap.put(className, currVersion);
						currFCM.addToFactory();
					}
				}
			} catch (ClassNotFoundException |
					IOException | IllegalAccessException
					| IllegalArgumentException | InvocationTargetException 
					| NoSuchMethodException | SecurityException 
					| InstantiationException e) {
				e.printStackTrace();
			}
		}

		private boolean isNewClass(String className) {
			return !versionsMap.containsKey(className);
		}
		private boolean isNewVersion(String className, double currVersion) {
			return versionsMap.get(className) < currVersion;
		}
	}
	
/******************************************************************************/
	
	private class IOTTaskCreator {
		private JSONParser parser = new JSONParser();
		private static final String COMMAND_KEY = "CommandKey";
		private static final String DATA = "Data";


		private Runnable createRunnableTask(String jsonString, 
				ClientInfoWrapper clientInfo) throws ParseException {
			JSONObject jsonObject = (JSONObject) parser.parse(jsonString);

			return new IOTRunnable((String)jsonObject.get(COMMAND_KEY),
					jsonObject.get(DATA), clientInfo);
		}
	}

	private class IOTRunnable implements Runnable {
		private final String commandKey;
		private final Object data;
		private final ClientInfoWrapper clientInfo;
		private static final String ERROR_MESSAGE = "errorMessage";
		private static final String DATA_BASE_NAME = "dbName";


		public IOTRunnable(String commandKey, Object data, ClientInfoWrapper clientInfo) {
			Objects.requireNonNull(commandKey);
			Objects.requireNonNull(data);
			this.commandKey = commandKey;
			System.out.println("This is the commandKey " + commandKey);
			this.data = data;
			System.out.println("This is the data: " + data);
			this.clientInfo = clientInfo;
		}

		@Override
		public void run() {
			try {
				FactoryCommand factoryCommand = cmdFactory.create(commandKey);
				DatabaseManagementInterface dbManagement = getDatabaseManagement(data, DATA_BASE_NAME);
				String responseMessage  = factoryCommand.run(data, dbManagement);
				System.out.println("This is the response message: " + responseMessage);
				if(null != responseMessage) {
					clientInfo.getConnection().sendResponseMessage(responseMessage, clientInfo);
				}
			} 
			catch (SQLException e) {
				try {
					e.printStackTrace();/////////////////////////////////// remove after corrections
					clientInfo.getConnection().sendErrorResponseMessage(
							getJsonString(ERROR_MESSAGE, e.getMessage()), 
							clientInfo);
				} catch (IOException e1) {
					e1.printStackTrace();
				}
			} catch (IOException e2) {
				e2.printStackTrace();
			} 
		}
	}

/******************************************************************************/
/*------------------------JarMonitor class------------------------------------*/
/******************************************************************************/	

	private class JarMonitor implements DirMonitor, Runnable {
		private Dispatcher<String> dispatcher = new Dispatcher<>();
		private WatchService watcher = FileSystems.getDefault().newWatchService();
		private String directoryToWatch = null;
		private boolean continueRun = true;

		public JarMonitor(String dirPath) throws IOException  {
			Objects.requireNonNull(dirPath);
			File dir = new File(dirPath);
			checkIfDirIsValid(dir);
			directoryToWatch = dirPath;
			Paths.get(directoryToWatch).register(watcher, ENTRY_MODIFY, ENTRY_CREATE);
		}

		@Override
		public void register(Callback<String> callback) {
			dispatcher.register(Objects.requireNonNull(callback));
		}

		@Override
		public void unregister(Callback<String> callback) {
			dispatcher.unregister(Objects.requireNonNull(callback));
		}

		@Override
		public void stopUpdate() throws IOException {
			continueRun = false;
			watcher.close();
		}

		@Override
		public void run() {
			while (continueRun) {
				WatchKey key = null;
				try {
					key = watcher.take();
				}
				catch (InterruptedException e) {
					System.out.println("OOPS got interrupted");
				}
				catch (ClosedWatchServiceException e) {
					System.out.println("Stopped updating");
				}

				for (WatchEvent<?> event : key.pollEvents()) {

					final Path changedJar = (Path) event.context();
					if (changedJar.getFileName().toString().endsWith(JAR_SUFFIX)) {
						System.out.println("This is the jarFile " + changedJar);
						updateAll(directoryToWatch.toString() + changedJar);

						break;
					}
				}

				checkIfKeyIsValid(key);
			}
		}

/*-----------------------JarMonitor Auxiliary methods-------------------------*/	

		private void checkIfKeyIsValid(WatchKey key) {
			boolean valid = key.reset();
			if (!valid) {
				continueRun = false;
			}
		}

		private void checkIfDirIsValid(File dir) throws FileNotFoundException {
			if(!dir.isDirectory()) {
				throw new FileNotFoundException("Path isn't a Directory");
			}
		}

		private void updateAll(String pathString) {
			dispatcher.updateAll(pathString);
		}
	}

/******************************************************************************/
/*------------------------DatabaseManagement class----------------------------*/
/******************************************************************************/	


	private class DatabaseManagement implements DatabaseManagementInterface {
		private final String databaseUrl;
		private final String userName;
		private final String password;
		private static final String SSL_OFF = "?useSSL=false";
		private static final String RAWDATA_DELIMETER = "\\|";
		private static final String COMMAND_DELIMETER = ";";
		private static final String RAWDATA_TABLE_NAME = "IOTEvent ";

		public DatabaseManagement(String url, String userName, String password, 
				String databaseName) throws SQLException {
			this.userName = userName;
			this.password = password;	
			this.databaseUrl = url + databaseName + SSL_OFF;
			createDatabase(databaseName, url + SSL_OFF);
			System.out.println(databaseName);
		}

		@Override
		public void createIOTEvent(String rawData) throws SQLException { 
			String[] values = rawData.split(RAWDATA_DELIMETER);
			String sqlCommand = " INSERT INTO " + RAWDATA_TABLE_NAME + " (serial_number, description)  VALUES (";
			int length = values.length;
			for (int i = 0; i < length; ++i) {
				if (i + 1 == length) {
					sqlCommand +=  values[i] + ")"; 
				} else {
					sqlCommand +=  values[i] + ", "; 
				}
			}    
			executeSQLCommand(sqlCommand);
		}

		private void createDatabase(String databaseName, String url) throws SQLException {
			try(
					java.sql.Connection conn = DriverManager.getConnection(url, userName, password);
					Statement statement = conn.createStatement();
					){
				statement.execute(" CREATE DATABASE IF NOT EXISTS " + databaseName);
				System.out.println("I'm creating a data base!!!!!!!!!!!!\n\n\n\n\n\n");
			} 
		}


		private void executeSQLCommand(String sqlCommands) throws SQLException {
			try(
					java.sql.Connection conn =
					DriverManager.getConnection(databaseUrl, userName, password);
					Statement statement = conn.createStatement();
					){
				String[] sqlCommandsArr = sqlCommands.split(COMMAND_DELIMETER);
				conn.setAutoCommit(false);	
				for (String command : sqlCommandsArr) {
					System.out.println(command);
					statement.addBatch(command.trim());			
				}
				statement.executeBatch();
				conn.commit();
			} 	
		}

		@Override
		public void createTable(String sqlCommand) throws SQLException {
			executeSQLCommand(sqlCommand);

		}

		@Override
		public void createRow(String sqlCommand) throws SQLException {
			executeSQLCommand(sqlCommand);
		}
	}

/******************************************************************************/
/*------------------------Auxiliary functions---------------------------------*/
/******************************************************************************/	

	private String getJsonString(String fieldName, String fieldValue) {
		return "{ \"" + fieldName + "\": " + fieldValue +  " }";
	}

	private DatabaseManagement getDatabaseManagement(Object data, 
			String dbNameKey) throws SQLException {
		JSONObject jsonObject = (JSONObject) data;
		String dbNameValue = (String) getJSONFieldValue(dbNameKey,jsonObject);
		createDBIfNotExist(dbNameValue);

		return companyDbManagementMap.get(dbNameValue);
	}

	private void createDBIfNotExist(String dbName) throws SQLException {
		if(!companyDbManagementMap.containsKey(dbName)) {
			companyDbManagementMap.put(dbName, 
					new DatabaseManagement(URL, USER_NAME, PASSWORD, dbName));
		}			
	}

	private Object getJSONFieldValue(String jsonFieldName, JSONObject jsonObject) 
			throws SQLException {
		Object fieldValue = jsonObject.get(jsonFieldName); 
		if(null == fieldValue) {
			throw new SQLException("wrong field name in json");
		}

		return fieldValue;
	}

	private void checkIfStarted() throws WrongUseOfServer {
		if(true == connectionHandler.isRunning) {
			throw new WrongUseOfServer("Can't add connections after start");
		}
	}

	public class WrongUseOfServer extends Exception {
		private static final long serialVersionUID = 1L;

		private WrongUseOfServer(String message) {
			super(message);
		}
	}

	private void fillHeadersWithStandardHeaders(HttpExchange httpExchange) {
		Headers headers = httpExchange.getResponseHeaders();
		headers.add("Content-Type", "application/json");
	}

	private String getMessageString(ByteBuffer byteBuffer, ClientInfoWrapper clientInfo) 
			throws IOException {
		String messageString = null;
		byteBuffer.clear();
		SocketChannel socketChannel = (SocketChannel) clientInfo.getTcpClientChannel();
		if (-1 == socketChannel.read(byteBuffer)) {
			socketChannel.close();
			System.out.println("client is closed");
		} else {
			byteBuffer.flip();
			messageString = new String(Charset.forName("ASCII").decode(byteBuffer).array());
			System.out.println("This is the messageString:" + messageString+ "\n\n\n\n\n");
		}
		return messageString;
	}
}