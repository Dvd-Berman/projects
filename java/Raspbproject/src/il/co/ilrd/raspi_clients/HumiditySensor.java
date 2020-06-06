package il.co.ilrd.raspi_clients;

import java.util.Random;

public class HumiditySensor {
	private Random rand = new Random();
	
	public double getHumidistLevel() {
		return Math.round(rand.nextDouble() * 100);
	}
}
