package il.co.ilrd.raspi_clients;

import java.util.Random;

public class TempSensor {
	private Random randTemp = new Random();
	public double getTemp() {
		return Math.round(randTemp.nextDouble() * 100);
	}
}
