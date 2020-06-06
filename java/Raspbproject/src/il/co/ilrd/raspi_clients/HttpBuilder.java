package il.co.ilrd.raspi_clients;

import java.util.Map;
import java.util.Objects;

public class HttpBuilder {
	private static final String SPACE = " ";
	private static final String EmptyString = "";
	private static final String CRLF = "\r\n";
	private static final String EMPTY_LINE = "\r\n";
	
	public static String createHttpRequestMessage(HttpMethod method, 
			HttpVersion version, String url, Map<String, String> header, String body){
		checkforValidInputForRequestMessage(method, version, url);
		String startLine = 
				StartLineHttpBuilder.createStartLineRequest(method,version, url);		
		String headerString = getHeaderString(header);
		String bodyString = getBodyString(body);
	
		return startLine + headerString + EMPTY_LINE + bodyString;
	}
	
	public static String createHttpResponseMessage(HttpVersion version,
			HttpStatusCode code, Map<String, String> header, String body){
		checkforValidInputForResponseMessage(version, code);
		
		String startLine = StartLineHttpBuilder.createStartLineResponse(version, code);		
		String headerString = getHeaderString(header);
		String bodyString = getBodyString(body);

		return startLine + headerString + EMPTY_LINE + bodyString;
	}
	
/******************************************************************************/
/*------------------------StartLineHttpBuilder--------------------------------*/		
	
	public static class StartLineHttpBuilder {
		static String createStartLineRequest(HttpMethod method,
				HttpVersion version, String url) {
			
			return method.getMethodAsString() + SPACE + url + SPACE + 
					version.getVersionAsString() + CRLF;
		}
		
		static String createStartLineResponse(HttpVersion version, HttpStatusCode code) {

			return version.getVersionAsString() + SPACE + code.asText() + 
										SPACE + code.getDescription() + CRLF;
		}
	}
	
/******************************************************************************/
/*------------------------HeaderHttpBuilder-----------------------------------*/		
	
	public static class HeaderHttpBuilder {
		private static final String COLON = ":";
		
		static String createHeader(Map<String, String> header) {
			String headerString = EmptyString;
			
			for (String hederIter : header.keySet()) {
				headerString += hederIter + COLON + header.get(hederIter) + CRLF;
			}
			return headerString;
		}
	}
	
/******************************************************************************/
/*------------------------BodyHttpBuilder-------------------------------------*/			
	
	public static class BodyHttpBuilder {
		static String createBody(String body) {
			
			return body;
		}
	}

/*------------------------private methods-------------------------------------*/				
	
	private static void checkforValidInputForResponseMessage(HttpVersion version,
			HttpStatusCode code) {
		Objects.requireNonNull(version);
		Objects.requireNonNull(code);
	}
	
	private static void checkforValidInputForRequestMessage(HttpMethod method, 
			HttpVersion version, String url) {
		Objects.requireNonNull(method, "method");
		Objects.requireNonNull(version, "version");
		Objects.requireNonNull(url, "url");
	}

	private static String getBodyString(String body) {
		if (null != body) {
			return BodyHttpBuilder.createBody(body);
		}
		return EmptyString;
	}

	private static String getHeaderString(Map<String, String> header) {
		if (null != header) {
			return HeaderHttpBuilder.createHeader(header);
		}
		return EmptyString;
	}
}