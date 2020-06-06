package michaeljordan;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.PriorityBlockingQueue;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;


public class YoniHeartMonitorIOT  {
	private static final String COMPANY_NAME = "Tadiran3";
	private static final String LOW_BATTERY = "low battery";
	private static final String ERROR = "error error..."; 
	private static final String IOT_UPDATE = "IOT_UPDATE";
	private final String serialNum;
	private final String ip;
	private final int port;
	private PriorityBlockingQueue<IOTUpdateMessage> messagesQueue = new PriorityBlockingQueue<>();
	private Map<String, IOTUpdateMessage> IOTUpdateMesMap = new ConcurrentHashMap<>();
	private HeartrateSensor heartrateSensor = new HeartrateSensor();
	private Battery battery = new Battery(22);
	private Socket socket;
	private Semaphore waitTillSendTimeSem = new Semaphore(0);
	
	public static void main(String[] args) throws InterruptedException {
//		HeartMonitorIOT heartMonitorIOT = new HeartMonitorIOT(Integer.parseInt(args[0]),
//												args[1],
//												Integer.parseInt(args[3]));
		YoniHeartMonitorIOT heartMonitorIOT = new YoniHeartMonitorIOT("00001", "127.0.0.1", 50000);
		heartMonitorIOT.start();
	}
	
	private void start() {
		new Thread(new MessageReceiver()).start();
		new Thread(new MessageSender()).start();
		new Thread(new UpdateRunnable()).start();
		new Thread(new WarningRunnable()).start();
		new Thread(new ErrorRunnable()).start();
	}

	public YoniHeartMonitorIOT(String serialNum, String ip, int port) throws InterruptedException {
		this.serialNum = serialNum;
		this.ip = ip;
		this.port = port;
		tryToConnectTillSuccess();
	}
			
	private class MessageSender implements Runnable {
		
		@Override
		public void run() {
			try(
					OutputStream outputStream = socket.getOutputStream();
			){
				while(true) {
					Thread.sleep(1000);
					sendMessage(outputStream);
			
				}
			} catch (IOException | InterruptedException e) {
				// TODO Auto-generated catch block
				System.out.println(e.getMessage());
			}				
		}

		private void sendMessage(OutputStream outputStream) throws InterruptedException, IOException {		
			IOTUpdateMessage iotUpdateMessage;
			long remainingTimeTillSend = 10000000;
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
							outputStream.write(getJsonMessage(iotUpdateMessage.getMessage()).getBytes());
							outputStream.flush();
						} catch (IOException e) {
							System.out.println("sender excp " + e.getMessage());
							e.printStackTrace();
						}
						iotUpdateMessage.updateTimeToSend();
						messagesQueue.add(iotUpdateMessage);
					}					
				}
			}
	}

	private class MessageReceiver implements Runnable {
		private JSONParser jsonParser = new JSONParser();
		@Override
		public void run() {
			try (
				InputStream inputStream = socket.getInputStream();
			) {
				while (true) {
					byte[] byteArr = new byte[4096];
					try {
					if(-1 ==  inputStream.read(byteArr)) {
						System.out.println("server is closed");
						continue;
					}
					String responseMessage = new String(byteArr);
					System.out.println(responseMessage);
						JSONObject jsonObject = (JSONObject) jsonParser.parse(responseMessage.trim());						
						String messageKey = (String) jsonObject.get("commandType");
						if (null != messageKey) {
							messagesQueue.remove(IOTUpdateMesMap.get(messageKey));
							IOTUpdateMesMap.remove(messageKey);
						}
					} catch (ParseException  e) {
						System.out.println(e.getMessage());
					}
				}
			} catch (IOException e) {
				System.out.println("receiver excp " + e.getMessage());
				e.printStackTrace();
			} 
		}
	}

	private class UpdateRunnable implements Runnable {

		@Override
		public void run() {
			while(true) {
				try {
					Thread.sleep(5000);
//					System.out.println(updateSocketChannel);
					String description = String.valueOf(heartrateSensor.getHeartRate())+ " heart beats";
//					System.out.println(string);
					IOTUpdateMessage iotUpdateMessage = new IOTUpdateMessage(null, description, 1, 2000);
					messagesQueue.put(iotUpdateMessage);
					waitTillSendTimeSem.release();
//					IOTUpdateMesMap.put(description)
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					System.out.println(e.getMessage());
				}				
			}
		}
	}
	
	private class WarningRunnable implements Runnable {
		
		@Override
		public void run() {
			while(true) {
				try {
					Thread.sleep(1000);
					sendWarningIfNeeded();
				} catch (IOException | InterruptedException e) {
					System.out.println(e.getMessage());
				}				
			}
		}
		
		private void sendWarningIfNeeded() throws IOException {
			if (battery.isLow()) {
				IOTUpdateMessage iotUpdateMessage = new IOTUpdateMessage(null, LOW_BATTERY, 3, 1000);
				messagesQueue.put(iotUpdateMessage);
				IOTUpdateMesMap.put(LOW_BATTERY, iotUpdateMessage);
				waitTillSendTimeSem.release();
				try {
					Thread.sleep(10000000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}
	
	private class ErrorRunnable implements Runnable {
		
		@Override
		public void run() {
			while(true) {
				
				try {
					Thread.sleep(1000);
					sendErrorIfNeeded();
				} catch (IOException | InterruptedException e) {
					System.out.println(e.getMessage());
				}				
			}
		}
		
		private void sendErrorIfNeeded() throws IOException {
			if(heartrateSensor.getHeartRate() < 0) {
				IOTUpdateMessage iotUpdateMessage = new IOTUpdateMessage(null, ERROR, 3, 1000);
				messagesQueue.put(iotUpdateMessage);
				IOTUpdateMesMap.put(ERROR, iotUpdateMessage);
				waitTillSendTimeSem.release();
				try {
					Thread.sleep(10000000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
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
	
	private void tryToConnectTillSuccess() throws InterruptedException {
		try {
			socket = new Socket(InetAddress.getByName(ip), port);
			System.out.println("succeed to connect");
		} catch (IOException e) {
			System.out.println(e.getMessage());
			Thread.sleep(3000);
			tryToConnectTillSuccess();
		}
	}
}
