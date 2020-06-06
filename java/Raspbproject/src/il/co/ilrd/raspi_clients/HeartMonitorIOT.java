package il.co.ilrd.raspi_clients;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.util.Random;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.PriorityBlockingQueue;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

public class HeartMonitorIOT {
	private static HeartRateSensor heartRateSensor;
	public static ConcurrentHashMap<String, IOTUpdateMessage> messageMap =
			new ConcurrentHashMap<>();
	private PriorityBlockingQueue<IOTUpdateMessage> messagesQueue = new PriorityBlockingQueue<>();
	public static ConcurrentHashMap<String, Semaphore> semaphoreMap = 
			new ConcurrentHashMap<>();
	private Semaphore pauseSendingMessagesSem = new Semaphore(0);
	private static final String COMPANY_NAME = "Amazing_inc";
	private static final int BYTE_CAPACITY = 4096;
	private static final String HEART_RATE = " Heart rate is  ";
	private static final String BATTERY_ERROR = " Error Battery is low";
	private static final String HR_WARNING = " Warning Heart Rate dangerous ";
	private Battery battery = new Battery(100);
	private boolean isRunning = true;
	private final String serialNumber;
	private final int port;
	private final String ip;
	private Socket clientSocket;
	private long globalRTT = 2000;

	
	HeartMonitorIOT(String serialNumber, int port, String ip){
		this.serialNumber = serialNumber;
		this.port = port;
		this.ip = ip;
		heartRateSensor = new HeartRateSensor();
		initConnection();
	}

	private void initConnection() {
		try {
			clientSocket = new Socket(InetAddress.getByName(ip), port);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public static void main(String[] args) {
		HeartMonitorIOT heartMonitorIOT = new HeartMonitorIOT("HRAlpha", 50000, "10.1.0.102");
		//HeartMonitorIOT heartMonitorIOT = new HeartMonitorIOT(args[0], Integer.parseInt(args[1]), args[2]);
		new Thread(heartMonitorIOT.new HRUpdates()).start();
		new Thread(heartMonitorIOT.new RecieveMessage()).start();
		new Thread(heartMonitorIOT.new SendMessage()).start();
		new Thread(heartMonitorIOT.new HRWarning()).start();
		new Thread(heartMonitorIOT.new DeviceError()).start();
	}

	public void turnOnSendLight() {
		RaspiUtil.turnOnSendLed();
	}
	
	public void turnOnReceiveLight() {
		RaspiUtil.turnOnReceiveLed();
	}
	
/******************************************************************************/
/*----------------------SendMessage Class-------------------------------------*/

	private class SendMessage implements Runnable {
		private static final long MAX_WAIT = Long.MAX_VALUE;

		@Override
		public void run() {
			try(OutputStream outputStream = clientSocket.getOutputStream()){
				while(isRunning) {
					sendMessage(outputStream);
				}
			}
			catch (IOException | InterruptedException e) {
				e.printStackTrace();
			} 
		}

		private void sendMessage(OutputStream outputStream) throws InterruptedException, IOException {		
			IOTUpdateMessage iotUpdateMessage;
			long remainingTimeTillSend = MAX_WAIT;
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
					iotUpdateMessage.setTimeSent(System.currentTimeMillis());
					outputStream.write(iotUpdateMessage.getMessage().getBytes());
					outputStream.flush();
					turnOnSendLight();
					iotUpdateMessage.updateTimeToSend(globalRTT);
					messagesQueue.add(iotUpdateMessage);
				}					
			}
		}
	}

/******************************************************************************/
/*----------------------RecieveMessage Class----------------------------------*/	
	
	private class RecieveMessage implements Runnable {

		@Override
		public void run() {
			try(InputStream inputStream = clientSocket.getInputStream()) {
				while(isRunning) {
					byte[] byteArr = new byte[BYTE_CAPACITY];
					if(-1 !=  inputStream.read(byteArr)) {
						String responseString = new String(byteArr);
						String recievedDescription = getDescription(responseString);
						if(null != recievedDescription) {
							IOTUpdateMessage recievedMessage = messageMap.get(recievedDescription);
							updateCurrentRTT(recievedMessage);
							messagesQueue.remove(recievedMessage);
							turnOnReceiveLight();
							System.out.println(recievedDescription);							
						}
					}
				}
			}
			catch (IOException e) {
				e.printStackTrace();
			}
		}

		private void updateCurrentRTT(IOTUpdateMessage recievedMessage) {
			long recievedTime = System.currentTimeMillis();
			long newRTT = 3 * (recievedTime - recievedMessage.getTimeSent());
			globalRTT = RaspiUtil.updateRTT(globalRTT, newRTT);
			System.out.println("RTT is now : " + globalRTT);
		}

		private String getDescription(String responseString) {
			JSONParser jsonParser = new JSONParser();
			try {
				JSONObject responseJSON = (JSONObject) jsonParser.parse(responseString.trim());
				return (String) responseJSON.getOrDefault("commandType", null);
			} catch (ParseException e) {
				e.printStackTrace();
			}
			return null;
		}
	}
	
/******************************************************************************/
/*----------------------HRUpdates Class---------------------------------------*/		
	
	private class HRUpdates implements Runnable {
		private long updateID = 0;
		private static final long SLEEP_TIME = 5000;
		private static final int SENDING_COUNTER = 1;

		@Override
		public void run() {
			while(isRunning) {
				try {
					Thread.sleep(SLEEP_TIME);
					updateHR(heartRateSensor.getHeartRate());
				}
				catch (InterruptedException | IOException e) {
					System.out.println(e.getMessage());
				}
			}
		}
		private void updateHR(int heartRate) throws IOException {
			String idString = String.valueOf(++updateID);
			String description = idString + HEART_RATE + heartRate;
			String iotUpdate = IOTMessageCreator.createIOTUpdateMessage(serialNumber, 
							COMPANY_NAME, description);
			IOTUpdateMessage iotUpdateMessage =
					new IOTUpdateMessage(null, iotUpdate, SENDING_COUNTER, globalRTT);
			messageMap.put(description ,iotUpdateMessage);
			messagesQueue.put(iotUpdateMessage);
			pauseSendingMessagesSem.release();
		} 
	}

/******************************************************************************/
/*----------------------HRWarning Class---------------------------------------*/		
		
		private class HRWarning implements Runnable {
			private long warningID = 0;
			private static final long SLEEP_TIME = 2000;
			private static final int SENDING_COUNTER = 3;
			private static final int HIGH_HR_THRESHOLD = 130;
			private static final int LOW_HR_THRESHOLD = 30;

			@Override
			public void run() {
				while(isRunning) {
					try {
						Thread.sleep(SLEEP_TIME);
						checkForExtremeHR(heartRateSensor.getHeartRate());
					} catch (InterruptedException | IOException e) {
						System.out.println(e.getMessage());
					}				
				}
			}
			
			private void checkForExtremeHR(int heartRate) throws IOException {
				if(heartRate <= LOW_HR_THRESHOLD || heartRate >= HIGH_HR_THRESHOLD) {
					String idString = String.valueOf(++warningID);
					String description = idString + HR_WARNING + heartRate;
					String iotWarning = IOTMessageCreator.createIOTUpdateMessage(serialNumber, 
							COMPANY_NAME, description);
					IOTUpdateMessage iotUpdateMessage =
							new IOTUpdateMessage(null, iotWarning, SENDING_COUNTER, globalRTT);
					messageMap.put(description ,iotUpdateMessage);
					messagesQueue.put(iotUpdateMessage);
					pauseSendingMessagesSem.release();
				}
			}
		}
		
/******************************************************************************/
/*----------------------DeviceError Class-------------------------------------*/		
				
		private class DeviceError implements Runnable {
			private long errorID = 0;
			private static final long SLEEP_TIME = 2000;
			private static final int SENDING_COUNTER = 3;

			@Override
			public void run() {
				while(isRunning) {
					try {
						Thread.sleep(SLEEP_TIME);
						checkForError();
					} catch (InterruptedException | IOException e) {
						System.out.println(e.getMessage());
					}				
				}
			}

			private void checkForError() throws IOException {	
					if(battery.isLow()) {
						String idString = String.valueOf(++errorID);
						String description =  idString + BATTERY_ERROR;
						String iotError = IOTMessageCreator.createIOTUpdateMessage(serialNumber, 
								COMPANY_NAME,  description);
						IOTUpdateMessage iotUpdateMessage =
								new IOTUpdateMessage(null, iotError,
											SENDING_COUNTER, globalRTT);
						messageMap.put(description ,iotUpdateMessage);
						messagesQueue.put(iotUpdateMessage);
						pauseSendingMessagesSem.release();
				}
			}
		}

/******************************************************************************/
/*---------------------------HeartSensor--------------------------------------*/
	
	private class HeartRateSensor {
		private Random rand = new Random();
		
		private  int getHeartRate() {
			return rand.nextInt(150);
		}
	}
}
