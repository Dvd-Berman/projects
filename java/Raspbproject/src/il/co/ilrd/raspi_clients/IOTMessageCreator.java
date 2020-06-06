package il.co.ilrd.raspi_clients;

import org.json.simple.JSONObject;

public class IOTMessageCreator {
	private static final String IOTCommandKey = "IOT_UPDATE";
	private static final String COMMAND_KEY = "CommandKey";
	private static final String DATA  = "Data";
	private static final String DBNAME = "dbName";
	private static final String RAW_DATA = "rawData";
	
	
	public static String createIOTUpdateMessage(String serialNumber,
											String dbName, String description){
		JSONObject jsonMessage =  new JSONObject();
		JSONObject innerJson = new JSONObject();
		innerJson.put(DBNAME, dbName);
		innerJson.put(RAW_DATA, getRawData(serialNumber, description));
		jsonMessage.put(COMMAND_KEY, IOTCommandKey);
		jsonMessage.put(DATA, innerJson);
		
		return jsonMessage.toString();
	}

	private static Object getRawData(String serialNumber, String description) {
		return '\'' + serialNumber + '\'' + "|" +  '\'' + description + '\'';
	}
}

