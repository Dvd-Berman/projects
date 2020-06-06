package michaeljordan;


import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;


public class clients {
	private static String MESSAGE = "{\"CommandKey\":\"COMPANY_REGISTRATION\",\n" + 
			"  \"Data\" : {\"dbName\":\"tadiran\",\n" + 
			"  \"sqlCommand\":\n" + 
			"\n" + 
			"\"CREATE TABLE PaymentHistory (\n" + 
			"	payment_history_id int AUTO_INCREMENT PRIMARY KEY,\n" + 
			"	payment_id int NOT NULL,\n" + 
			"    payment_date date,\n" + 
			"    amount int NOT NULL,\n" + 
			"    approved boolean NOT NULL\n" + 
			")\"\n" + 
			"}}"; 
	public static void main(String[] args) throws IOException, InterruptedException {
		String inetAddress = "10.1.0.102";
		ByteBuffer buffer = ByteBuffer.allocate(4096);
			/**************************tcpclient***************************/
			try (
					SocketChannel clientSocket = SocketChannel.open(
										new InetSocketAddress(inetAddress, 50000));	
			)
			{
				buffer.put(MESSAGE.getBytes());
				buffer.flip();
				clientSocket.write(buffer);
				ByteBuffer byteBuffer = ByteBuffer.allocate(2048);//ByteBuffer.wrap(bodyString.getBytes());//Charset.forName("ASCII").encode(bodyString);
				clientSocket.read(byteBuffer);
				String responseString = new String(byteBuffer.array());
				System.out.println(responseString);
			} 
			
			/**********************udpclient*******************************/
			System.out.println("\nnudp");
			try (DatagramSocket socket = new DatagramSocket()) {
			        byte[] buf = MESSAGE.getBytes();
			        InetAddress address = InetAddress.getByName(inetAddress);
			        DatagramPacket packet = new DatagramPacket(buf, 
			        			buf.length, address, 
			        			55555);
			        socket.send(packet);
			        byte[] receivedbuf = new byte[2048];
	 		        packet = new DatagramPacket(receivedbuf, receivedbuf.length);
			        socket.receive(packet);
			        System.out.println(new String(receivedbuf));
			}		
		}
	} 



