package il.co.ilrd.raspi_clients;

public enum HttpVersion {
	HTTP_1_0("HTTP/1.0"),
    HTTP_1_1("HTTP/1.1");
	
	private String version;

	HttpVersion(String version) {
	    this.version = version;
	}

	public String getVersionAsString() {
	    return version;
	}
}