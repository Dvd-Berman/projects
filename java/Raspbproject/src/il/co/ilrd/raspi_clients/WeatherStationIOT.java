package il.co.ilrd.raspi_clients;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.PriorityBlockingQueue;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

public class WeatherStationIOT {
	private static final String COMPANY_NAME = "Amazing_inc";
	private static final String LOW_BATTERY = " LOW_BATTERY_";
	private static final String ERROR = " ERROR_"; 
	private static final double RTT_ALPHA = 0.2;
	private final String serialNumber;
	private final String ip;
	private final int port;
	private HumiditySensor humiditysensor = new HumiditySensor();
	private TempSensor tempSensor = new TempSensor();
	private Battery battery = new Battery(20);
	private DatagramSocket socket = new DatagramSocket();
	private PriorityBlockingQueue<IOTUpdateMessage> messagesQueue = new PriorityBlockingQueue<>();
	private Map<String, IOTUpdateMessage> messageMap = new ConcurrentHashMap<>();
	private Semaphore waitTillSendTimeSem = new Semaphore(0);
	private long globalRTT = 3000;
	
	public static void main(String[] args) throws IOException {
		//WeatherStationIOT weatherStation = new WeatherStationIOT(args[0],args[1],Integer.parseInt(args[2]));
		WeatherStationIOT weatherStation = new WeatherStationIOT("WSBeta", "10.1.0.102", 55555);
		weatherStation.start();
	}
	
	private void start() {
		new Thread(new MessageReceiver()).start();
		new Thread(new MessageSender()).start();
		new Thread(new UpdateRunnable()).start();
		new Thread(new WarningRunnable()).start();
		new Thread(new ErrorRunnable()).start();
	}
	
	public WeatherStationIOT(String serialNum, String ip, int port) throws IOException {
		this.serialNumber = serialNum;
		this.ip = ip;
		this.port = port;
	}
		
	private class MessageSender implements Runnable {
		private static final int SEND_INTERVAL = 1000;
		
		@Override
		public void run() {
			try {
				while(true) {
					Thread.sleep(SEND_INTERVAL);
					sendMessage();
			
				}
			} catch (IOException | InterruptedException e) {
			System.out.println(e.getMessage());
			}				
		}
	
		private void sendMessage() throws IOException, InterruptedException {		
			IOTUpdateMessage iotUpdateMessage;
			long remainingTimeTillSend = Long.MAX_VALUE;
			do {
				iotUpdateMessage = messagesQueue.peek();
				if (null != iotUpdateMessage) {
					remainingTimeTillSend = iotUpdateMessage.getTimeToSend() - System.currentTimeMillis();
				} 				
			} while (waitTillSendTimeSem.tryAcquire(remainingTimeTillSend, TimeUnit.MILLISECONDS));
			iotUpdateMessage = messagesQueue.poll();
			if(iotUpdateMessage != null) {
				if (0 < iotUpdateMessage.getRetransmissionCounter()) {
					iotUpdateMessage.decreaseCounter();
					try {
				        byte[] buf = iotUpdateMessage.getMessage().getBytes();
						InetAddress address = InetAddress.getByName(ip);
				        DatagramPacket packet = new DatagramPacket(buf, 
				        			buf.length, address, port);
						iotUpdateMessage.setTimeSent(System.currentTimeMillis());
				        socket.send(packet);
					} catch (IOException e) {
						System.out.println("sender excp " + e.getMessage());
					}
					turnOnSendLed();
					iotUpdateMessage.updateTimeToSend(globalRTT);
					messagesQueue.add(iotUpdateMessage);
				}					
			}

		}
	}
	
/******************************************************************************/
	
	private class MessageReceiver implements Runnable {
		private static final int ARR_SIZE = 4096; 
		@Override
		public void run() {
			try {
				while (true) {
					byte[] receivedbuf = new byte[ARR_SIZE];
					InetAddress address = InetAddress.getByName(ip);
					DatagramPacket packet = new DatagramPacket(receivedbuf, receivedbuf.length, address, port);
					socket.receive(packet);
					turnOnReceiveLed();
					String responseString = new String(receivedbuf); 
					String recievedDescription = getDescription(responseString);					
					IOTUpdateMessage recievedMessage = messageMap.get(recievedDescription);
					updateCurrentRTT(recievedMessage);
					messagesQueue.remove(recievedMessage);
					System.out.println(recievedDescription);							
				}
			} catch (IOException e) {
				System.out.println(e.getMessage());
			} 
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
	
	private class UpdateRunnable implements Runnable {
		private static final long UPDATE_INTERVAL = 5000;
		private long updateID = 0;
		private static final int TIMES_OF_RETRANSMISSION = 1;
		private static final String TEMP = " The temp is ";
		private static final String HUMIDITY = " and the humidity is ";

		@Override
		public void run() {
		
			while(true) {
				try {
					Thread.sleep(UPDATE_INTERVAL);
					updateWeatherStats(tempSensor.getTemp(), humiditysensor.getHumidistLevel());
				} catch (InterruptedException e) {
						System.out.println(e.getMessage());
				}				
			}
		}

		private void updateWeatherStats(double temp, double humidistLevel) {
			String idString = String.valueOf(++updateID);
			String description = idString + TEMP + temp + HUMIDITY + humidistLevel;
			String iotUpdate = IOTMessageCreator.createIOTUpdateMessage(serialNumber, 
							COMPANY_NAME, description);
			IOTUpdateMessage iotUpdateMessage = new IOTUpdateMessage(null, iotUpdate, 
													TIMES_OF_RETRANSMISSION,
													globalRTT);
			messageMap.put(description ,iotUpdateMessage);
			messagesQueue.put(iotUpdateMessage);
			waitTillSendTimeSem.release();
		}
	}
	
/******************************************************************************/
	
	private class WarningRunnable implements Runnable {
		private long idCounter = 0;
		private static final int WARNING_INTERVAL = 3000; 
		private static final int TIMES_OF_RETRANSMISSION = 3;

		@Override
		public void run() {
			while(true) {
				try {
					Thread.sleep(WARNING_INTERVAL);
					sendWarningIfNeeded();
				} catch (IOException | InterruptedException e) {
					System.out.println(e.getMessage());
				}				
			}
		}
		
		private void sendWarningIfNeeded() throws IOException {
			if (battery.isLow()) {
				String idString = String.valueOf(++idCounter);
				String description = idString + LOW_BATTERY;
				String iotUpdate = IOTMessageCreator.createIOTUpdateMessage(serialNumber, 
						COMPANY_NAME, description);
				IOTUpdateMessage iotUpdateMessage = new IOTUpdateMessage(null, iotUpdate, 
						TIMES_OF_RETRANSMISSION, globalRTT);
				messageMap.put(description, iotUpdateMessage);
				messagesQueue.put(iotUpdateMessage);
				waitTillSendTimeSem.release();
			}
		}
	}
	
/******************************************************************************/
	
	private class ErrorRunnable implements Runnable {
		private long idCounter = 0;
		private static final int ERROR_INTERVAL = 2000; 
		private static final int TIMES_OF_RETRANSMISSION = 3;

		@Override
		public void run() {
			while(true) {
				try {
					Thread.sleep(ERROR_INTERVAL);
					sendErrorIfNeeded();
				} catch (IOException | InterruptedException e) {
				System.out.println(e.getMessage());
			}				
		}
	}
	
		private void sendErrorIfNeeded() throws IOException {
			if (tempSensor.getTemp() > 70) {
				String idString = String.valueOf(++idCounter);
				String description =  idString + ERROR;
				String iotUpdate = IOTMessageCreator.createIOTUpdateMessage(serialNumber, 
						COMPANY_NAME, description);
				IOTUpdateMessage iotUpdateMessage = new IOTUpdateMessage(null, iotUpdate, 
												TIMES_OF_RETRANSMISSION, globalRTT);
				messageMap.put(description, iotUpdateMessage);
				messagesQueue.put(iotUpdateMessage);
				waitTillSendTimeSem.release();
			}
		}
	}
	
/******************************************************************************/
	
	private void updateCurrentRTT(IOTUpdateMessage recievedMessage) {
		if(null != recievedMessage) {
			long recievedTime = System.currentTimeMillis();
			long newRTT = 3 * (recievedTime - recievedMessage.getTimeSent());
			globalRTT = RaspiUtil.updateRTT(globalRTT, newRTT);
			System.out.println("RTT is now : " + globalRTT);			
		}
	}
	
	private void turnOnSendLed() throws IOException {
		RaspiUtil.turnOnSendLed();
	}

	private void turnOnReceiveLed() throws IOException {
		RaspiUtil.turnOnReceiveLed();
	}	
}
	

