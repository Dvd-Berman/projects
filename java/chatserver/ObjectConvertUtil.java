package il.co.ilrd.chatserver;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

public class ObjectConvertUtil {
	public static byte[] toByteArray(Object obj) throws IOException {
		byte[] bytes = null;
        try(ByteArrayOutputStream bos = new ByteArrayOutputStream();
        	ObjectOutputStream oos = new ObjectOutputStream(bos)){
            oos.writeObject(obj);
            bytes = bos.toByteArray();
        }
        
        return bytes;
    }
	
	public static Object toObject(byte[] bytes) throws ClassNotFoundException, IOException {
		Object obj = null;
        try(ByteArrayInputStream bis = new ByteArrayInputStream(bytes);
        	ObjectInputStream ois = new ObjectInputStream(bis);) {
            obj = ois.readObject();
        }

        return obj;
	}
}
