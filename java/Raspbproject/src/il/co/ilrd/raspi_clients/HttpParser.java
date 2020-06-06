package il.co.ilrd.raspi_clients;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class HttpParser {
	private StartLineHttpParser startLine;
	private HeaderHttpParser header = null;
	private BodyHttpParser body = null;
	private final static  String CRLF = "\\r?\\n";
	private final static String EMPTY_LINE = "\\r\\n\\r?\\n";
	private final int STARTLINE_INDEX = 0;
	private final int HEADER_AND_BODY_INDEX = 1;
	private final int HEADER_INDEX = 0;
	private final int BODY_INDEX = 1;

	public HttpParser(String message)  {
		String [] startLineAndRest = message.split(CRLF,2);
		this.startLine = new StartLineHttpParser(startLineAndRest[STARTLINE_INDEX]);
		checkAndInitParserFields(startLineAndRest);
	}
	
	private void checkAndInitParserFields(String[] startLineAndRest) {
		if(bodyOrHeaderExist(startLineAndRest)) {
			String[] headersAndBody = startLineAndRest[HEADER_AND_BODY_INDEX].split(EMPTY_LINE);
			if(headerExist(headersAndBody)) {
				this.header = new HeaderHttpParser(headersAndBody[HEADER_INDEX]);
			}
			if(bodyExist(headersAndBody)) {
				this.body = new BodyHttpParser(headersAndBody[BODY_INDEX]);
			}
		}
	}
	
	private boolean bodyOrHeaderExist(String[] startLineAndRest) {
		return !startLineAndRest[HEADER_AND_BODY_INDEX].equals(CRLF);
	}

	private boolean bodyExist(String[] headersAndBody) {
		
		return (2 == headersAndBody.length) && 
				(!headersAndBody[BODY_INDEX].trim().isEmpty());
	}

	private boolean headerExist(String[] headersAndBody) {
		return 1 < headersAndBody.length;
	}

	public StartLineHttpParser getStartLine() {
		return startLine;
	}

	public HeaderHttpParser getHeader() {
		return header;
	}

	public BodyHttpParser getBody() {
		return body;
	}

	boolean isRequest() {
		return startLine.isRequest();
	}
	
	boolean isReply() {
		return startLine.isReply();
	}
	
/******************************************************************************/
/*------------------------StartLineHttpParser---------------------------------*/
	
	public class StartLineHttpParser {
		private HttpVersion version;
		private HttpMethod method;
		private HttpStatusCode status;
		private String url;
		private boolean isResponse;
		private final int METHOD_INDEX = 0;
		private final int URL_INDEX = 1;
		private final int VERSION_REQUEST_INDEX = 2;
		private final int VERSION_RESPONSE_INDEX = 0;
		private final int STATUS_CODE_INDEX = 1;
		private final static String SPACE = " ";
		
/******************************************************************************/
/*------------------------StartLineHttpParser---------------------------------*/		
		
		public StartLineHttpParser(String startLineString)
				{
			String[] startlineParts = startLineString.split(SPACE, 3);
			if(checkIfPartTwoIsNumeric(startlineParts[STATUS_CODE_INDEX])) {
				this.isResponse = true;
				initResponseValue(startlineParts);	
			}
			else {
				this.isResponse = false;
				initRequestValues(startlineParts);
			}
		}

		public HttpMethod getHttpMethod() {

			return method;
		}

		public String getURL() {

			return url;
		}

		public HttpStatusCode getStatus() {

			return status;
		}

		public boolean isRequest() {

			return false == isResponse;
		}

		public boolean isReply() {

			return isResponse;
		}

		public HttpVersion getHttpVersion() {

			return version;
		}

		private void initRequestValues(String[] startlineParts) 
				 {
			this.method = HttpMethod.valueOf(startlineParts[METHOD_INDEX]);
			this.url = startlineParts[URL_INDEX];
			this.version = getCurrHttpVersion(startlineParts[VERSION_REQUEST_INDEX]);
		}

		private HttpVersion getCurrHttpVersion(String httpVersion) {
			for (HttpVersion versionIter : HttpVersion.values()) {
				if (versionIter.getVersionAsString().equals(httpVersion)) {
					return versionIter;
				}					
			}
			return null;
		}

		private void initResponseValue(String[] startlineParts) {
			this.version = getCurrHttpVersion(startlineParts[VERSION_RESPONSE_INDEX]);
			this.status = getStatusField(startlineParts[STATUS_CODE_INDEX].trim());
		}

		private HttpStatusCode getStatusField(String statusCode) {
			for(HttpStatusCode statusRunner : HttpStatusCode.values()) {
				if(statusCode.equals(statusRunner.asText())) {

					return statusRunner;
				}
			}

			return null;
		}

		private boolean checkIfPartTwoIsNumeric(String secondPartOfStartLine) {
			try{
				Integer.parseInt(secondPartOfStartLine);
			}
			catch(NumberFormatException nfe) {

				return false;
			}

			return true;
		}
	}
	
/******************************************************************************/
/*------------------------HeaderHttpParser------------------------------------*/	
	
	public class HeaderHttpParser {
		Map<String, String> headerMap = new HashMap<>();
		private final static String COLON = ":";
		private final int HEADER_KEY_INDEX = 0;
		private final int HEADER_VALUE_INDEX = 1;
		
		public HeaderHttpParser(String headers) {
			String[] headersParts = headers.split(CRLF);
			fillHeaderMap(headersParts);
		}
	
		private void fillHeaderMap(String[] headersParts) {
			String[] headerkeyAndValue;
			for(String header : headersParts) {
				headerkeyAndValue = header.split(COLON, 2);
				headerMap.put(headerkeyAndValue[HEADER_KEY_INDEX], 
									headerkeyAndValue[HEADER_VALUE_INDEX]);
			}
		}

		public String getHeader(String header) {
			
			return headerMap.get(header);
		}
	
		public Map<String, String> getAllHeaders() {
			
			return headerMap;
		}
	}
	
/******************************************************************************/
/*------------------------BodyHttpParser------------------------------------*/		
	
	public class BodyHttpParser {
		String body;
		
		public BodyHttpParser(String body) {
			this.body = body;
		}
	
		public String getBodyString() {
			return body;
		}
	}
}