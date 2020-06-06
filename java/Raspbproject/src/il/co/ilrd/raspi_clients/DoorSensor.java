package il.co.ilrd.raspi_clients;

import java.util.Random;

public class DoorSensor {
	private Random rand = new Random();
	public int getTimeOpen() {
		return rand.nextInt(80);
	}
}
