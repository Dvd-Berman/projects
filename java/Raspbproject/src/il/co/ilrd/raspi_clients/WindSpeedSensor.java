package il.co.ilrd.raspi_clients;

import java.util.Random;

public class WindSpeedSensor {
	private Random rand = new Random();
	
	public double getWindSpeed() {
		return rand.nextDouble() * 120;
	}
}
