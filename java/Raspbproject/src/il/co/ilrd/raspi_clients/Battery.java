package il.co.ilrd.raspi_clients;

public class Battery {
	private  double batteryPower;
	private final double dangerZone = 5;

	Battery(double initialValue){
		this.batteryPower = initialValue;
	}
	
	public boolean isLow() {
		return --batteryPower < dangerZone;
	}
}
