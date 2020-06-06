package il.co.ilrd.gatewayserver;

import java.util.HashMap;
import java.util.Map;
import java.util.function.Function;

public class CMDFactory<T, K, D> {
	private  Map<K, Function<D, ? extends T>> map = new HashMap<>();
	
	private CMDFactory(){}
	
	private static class FactoryHolder{
		private static final CMDFactory<?, ?, ?> factory = new CMDFactory<>();
	}
	
	@SuppressWarnings("unchecked")
	public static <T, K, D> CMDFactory<T, K, D> getFactory(){
		return (CMDFactory<T, K, D>) FactoryHolder.factory;
	}
	
	public void add(K key, Function<D, ? extends T> func) {
		
		map.put(key, func);
	}

	public T create(K key, D data) {
		return map.get(key).apply(data);
	}
	
	public T create(K key) {
		return create(key, null);	
	}
}