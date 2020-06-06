package il.co.ilrd.raspi_clients;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.PriorityBlockingQueue;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

public class FridgeIOT {
	private static final String COMPANY_NAME = "Amazing_inc";
	private static final String OPEN_DOOR = "DOOR_IS_OPEN_";
	private static final String ERROR = "ERROR_"; 
	private final String serialNum;
	private boolean isRunning = true;
	private final String ip;
	private final int port;
	private DoorSensor doorSensor = new DoorSensor();
	private TempSensor tempSensor = new TempSensor();
	private Socket socket;
	private PriorityBlockingQueue<IOTUpdateMessage> messagesQueue = new PriorityBlockingQueue<>();
	private Map<String, IOTUpdateMessage> IOTUpdateMesMap = new ConcurrentHashMap<>();
	private Semaphore pauseSendingMessagesSem = new Semaphore(0);
	private long globalRTT = 3000;
	
	public static void main(String[] args) throws IOException, InterruptedException {
		FridgeIOT fridgeIOT = new FridgeIOT("FridgeGama","10.1.0.102",8080);
		//FridgeIOT fridgeIOT = new FridgeIOT(args[0],args[1],Integer.parseInt(args[2]));
		fridgeIOT.start();
	}
	
	private void start() {
		new Thread(new MessageReceiver()).start();
		new Thread(new MessageSender()).start();
		new Thread(new UpdateRunnable()).start();
		new Thread(new DoorSensorExaminor()).start();
		new Thread(new TempExaminor()).start();
	}

	public FridgeIOT(String serialNum, String ip, int port) throws IOException, InterruptedException {
		this.serialNum = serialNum;
		this.ip = ip;
		this.port = port;
		tryToConnectTillSuccess();
	}
		
	private class MessageSender implements Runnable {
		private static final int SEND_INTERVAL = 1000;
		@Override
		public void run() {
			try(
				OutputStream outputStream = socket.getOutputStream();
			){
			while(isRunning) {
					Thread.sleep(SEND_INTERVAL);
					sendMessages(outputStream);
			
				}
			} catch (IOException | InterruptedException e) {
				System.out.println(e.getMessage());
			}				
		}

		private void sendMessages(OutputStream outputStream) throws InterruptedException, IOException {		
			IOTUpdateMessage iotUpdateMessage;
			long remainingTimeTillSend = Long.MAX_VALUE;
			do {
				iotUpdateMessage = messagesQueue.peek();
				if (null != iotUpdateMessage) {
					remainingTimeTillSend = iotUpdateMessage.getTimeToSend() - System.currentTimeMillis();
				} 				
			} while (pauseSendingMessagesSem.tryAcquire(remainingTimeTillSend, TimeUnit.MILLISECONDS));
			iotUpdateMessage = messagesQueue.poll();
			if(iotUpdateMessage != null) {
				if (0 < iotUpdateMessage.getRetransmissionCounter()) {
					iotUpdateMessage.decreaseCounter();
					try {
						iotUpdateMessage.setTimeSent(System.currentTimeMillis());
						outputStream.write(iotUpdateMessage.getMessage().getBytes());
						outputStream.flush();
					} catch (IOException e) {
						System.out.println("sender excp " + e.getMessage());
						e.printStackTrace();
					}
					TurnOnSendLed();
					iotUpdateMessage.updateTimeToSend(globalRTT);
					messagesQueue.add(iotUpdateMessage);
				}					
			}

		}
	}

	private class MessageReceiver implements Runnable {
		private JSONParser jsonParser = new JSONParser();
		private static final int ARR_SIZE = 4096; 
		
		@Override
		public void run() {
			try (
					InputStream inputStream = socket.getInputStream();
			) {
				while (true) {
					try {
						byte[] byteArr = new byte[ARR_SIZE];
						if(-1 == inputStream.read(byteArr)) {
							Thread.sleep(1000);
							continue;
						}
						TurnOnReceiveLed();
						String responseMessage = new String(byteArr);
						HttpParser httpPareser = new HttpParser(responseMessage);
						System.out.println(httpPareser.getBody().getBodyString());
						JSONObject jsonObject = (JSONObject) jsonParser.parse(httpPareser.getBody().getBodyString().trim());						
						String messageKey = (String) jsonObject.get("commandType");
						if (null != messageKey) {
							IOTUpdateMessage iotMessage = IOTUpdateMesMap.get(messageKey);
							updateCurrentRTT(iotMessage);
							messagesQueue.remove(iotMessage);
							IOTUpdateMesMap.remove(messageKey);
						}							
					} catch (ParseException | InterruptedException e) {
						System.out.println(e.getMessage());
					}
				}
			} catch (IOException e) {
				e.printStackTrace();
			} 
		}
	}

	private class UpdateRunnable implements Runnable {
		private Map<String, String> headersMap = new HashMap<>();
		private static final int UPDATE_INTERVAL = 5000; 
		private static final int TIMES_OF_RETRANSMISSION = 1;

		
		@Override
		public void run() {
			headersMap.put("Content-Type", "application/json");
			while(isRunning) {
				try {
					Thread.sleep(UPDATE_INTERVAL);
					String description = "the temp is " + String.valueOf(tempSensor.getTemp());
					headersMap.put("Content-Length", new Integer(getJsonMessage(description).length()).toString());
					String httpMessage = HttpBuilder.createHttpRequestMessage(HttpMethod.POST, 
									HttpVersion.HTTP_1_1, 
									"/", 
									headersMap, 
									getJsonMessage(description));
					IOTUpdateMessage iotUpdateMessage = new IOTUpdateMessage(null, httpMessage, 
							TIMES_OF_RETRANSMISSION, globalRTT);
					messagesQueue.put(iotUpdateMessage);
					pauseSendingMessagesSem.release();
				} catch (InterruptedException e) {
					System.out.println(e.getMessage());
				}				
			}
		}
	}
	
	private class DoorSensorExaminor implements Runnable {
		private Map<String, String> headersMap = new HashMap<>();
		private long idCounter = 0;
		private static final int WARNING_INTERVAL = 3000; 
		private static final int TIMES_OF_RETRANSMISSION = 3;
		

		@Override
		public void run() {
			headersMap.put("Content-Type", "application/json");
			while(isRunning) {
				try {
					Thread.sleep(WARNING_INTERVAL);
					sendWarningIfNeeded();
				} catch (IOException | InterruptedException e) {
					System.out.println(e.getMessage());
				}				
			}
		}
		
		private void sendWarningIfNeeded() throws IOException {
			if (doorSensor.getTimeOpen() > 60) {
				String description = OPEN_DOOR + String.valueOf(idCounter++);
				String httpMessage = HttpBuilder.createHttpRequestMessage(HttpMethod.POST, 
						HttpVersion.HTTP_1_1, 
						"/", 
						headersMap, 
						getJsonMessage(description));
				headersMap.put("Content-Length", new Integer(getJsonMessage(description).length()).toString());
				IOTUpdateMessage iotUpdateMessage = new IOTUpdateMessage(null, httpMessage, 
														TIMES_OF_RETRANSMISSION,globalRTT);
				messagesQueue.put(iotUpdateMessage);
				IOTUpdateMesMap.put(description, iotUpdateMessage);
				pauseSendingMessagesSem.release();
			}
		}
	}
	
	private class TempExaminor implements Runnable {
		private Map<String, String> headersMap = new HashMap<>();
		private long idCounter = 0;
		private static final int ERROR_INTERVAL = 2000; 
		private static final int TIMES_OF_RETRANSMISSION = 3;

		@Override
		public void run() {
			headersMap.put("Content-Type", "application/json");
			while(isRunning) {
				try {
					Thread.sleep(ERROR_INTERVAL);
					sendErrorIfNeeded();
				} catch (IOException | InterruptedException e) {
					System.out.println(e.getMessage());
				}				
			}
		}
		
		private void sendErrorIfNeeded() throws IOException {
			if(tempSensor.getTemp() > 80) {
				String description = ERROR + String.valueOf(idCounter++);
				String httpMessage = HttpBuilder.createHttpRequestMessage(HttpMethod.POST, 
						HttpVersion.HTTP_1_1, 
						"/", 
						headersMap, 
						getJsonMessage(description));
				headersMap.put("Content-Length", new Integer(getJsonMessage(description).length()).toString());
				IOTUpdateMessage iotUpdateMessage = new IOTUpdateMessage(null, httpMessage, 
															TIMES_OF_RETRANSMISSION,globalRTT);
				messagesQueue.put(iotUpdateMessage);
				IOTUpdateMesMap.put(description, iotUpdateMessage);
				pauseSendingMessagesSem.release();
			}
		}
	}
			
	private String getJsonMessage(String description) {
		return "{"
				+ "\"CommandKey\": \"IOT_UPDATE\",\n" + 
				"	\"Data\": {\n" + 
				"		\"dbName\": \"" + COMPANY_NAME + "\",\n" + 
				"		\"rawData\": \"'" + serialNum + "'|'" + description + "'\"" + 
				"		}\n" + 
				"}";
	}
	
	private synchronized void tryToConnectTillSuccess() throws InterruptedException {
		try {
			socket = new Socket(InetAddress.getByName(ip), port);
		} catch (IOException e) {
			System.out.println(e.getMessage());
		}
	}
	private void updateCurrentRTT(IOTUpdateMessage recievedMessage) {
		if(null != recievedMessage) {
			long recievedTime = System.currentTimeMillis();
			long newRTT = 3 * (recievedTime - recievedMessage.getTimeSent());
			globalRTT = RaspiUtil.updateRTT(globalRTT, newRTT);
			System.out.println("RTT is now : " + globalRTT);			
		}
	}
	private void TurnOnSendLed() throws IOException {
		RaspiUtil.turnOnSendLed();
	}

	private void TurnOnReceiveLed() throws IOException {
		RaspiUtil.turnOnReceiveLed();	
	}	
}
