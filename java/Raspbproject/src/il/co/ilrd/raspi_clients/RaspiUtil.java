package il.co.ilrd.raspi_clients;

import java.io.IOException;

public class RaspiUtil {
	private static final double ALPHA = 0.2;
	public static void turnOnSendLed() {
		try {
			Runtime.getRuntime().exec("python SendLED.py");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public static void turnOnReceiveLed() {
		try {
			Runtime.getRuntime().exec("python ReceiveLED.py");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public static long updateRTT(long oldRTT, long currentRTT) {
		return (long) (ALPHA * oldRTT + (1 - ALPHA) * currentRTT);
	}
}
