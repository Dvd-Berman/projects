package il.co.ilrd.raspi_clients;


public class IOTUpdateMessage implements Comparable<IOTUpdateMessage> {
	private static final String ID_VALUE_DELIMITER = "$";
	private String id;
	private String value;
	private int retransmissionCounter;
	private long timeToSend;
	private long timeSent;
	private IOTUpdateMessage myID;
	
	public IOTUpdateMessage(String id, String value, int retransmissionCounter, long sendingInterval) {
		this.id = id;
		this.value = value;
		this.retransmissionCounter = retransmissionCounter;
		timeToSend = System.currentTimeMillis() + sendingInterval;
	}
	
	public void setTimeSent(long timeSent) {
		this.timeSent = timeSent;
	}
	public long getTimeSent() {
		return timeSent;
	}
	
	public long getTimeToSend() {
		return timeToSend;
	}
	
	public String getID() {
		return id;
	}
	
	public void decreaseCounter() {
		--retransmissionCounter;
	}
	
	public int getRetransmissionCounter() {
		return retransmissionCounter;
	}
	
	public String getMessage() {
		return  value;		
	}
	
	public void updateTimeToSend(long currentRTT) {
		timeToSend = System.currentTimeMillis() + currentRTT;
	}

	@Override
	public int compareTo(IOTUpdateMessage other) {
		return (int) (this.timeToSend - other.timeToSend);
	}
}
