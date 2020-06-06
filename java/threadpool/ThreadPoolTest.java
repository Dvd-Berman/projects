package il.co.ilrd.threadpool;

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import il.co.ilrd.threadpool.ThreadPool.TaskPriority;

class ThreadPoolTest {
	public static void main(String[] args){
		ThreadPool tPool = new ThreadPool(0);
		//Future<Void> future1 = tPool.submitTask(new CoronaCallable(), TaskPriority.MIN);
		//Future<Integer> future2  = tPool.submitTask(new ZoomCallable(), TaskPriority.MAX);
		//Future<String> future3  = tPool.submitTask(new AdMatayCallable());
		//Future<Integer> future4  = tPool.submitTask(new ZoomCallable(), TaskPriority.MAX);
		
//		future3.cancel(false);
//		System.out.println(future3.isCancelled());
//		
//		future1.cancel(true);
//		System.out.println(future1.isCancelled());
		
//		tPool.setNumberOfThread(0);
//		tPool.submitTask(new ZoomCallable(), TaskPriority.MIN);
		
		tPool.setNumberOfThread(1);
		tPool.pause();
		tPool.submitTask(new ZoomCallable());
		tPool.resume();
		//tPool.shutdown();
		tPool.submitTask(new AdMatayCallable());
		tPool.execute(()-> System.out.println("Runnable task"));	
//		tPool.shutdown();
//		System.out.println(tPool.awaitTermination(6, TimeUnit.SECONDS));
		Future<?> exceptionFuture = tPool.submitTask(new ExceptionTask(), TaskPriority.MAX);
		try {
			exceptionFuture.get(4, TimeUnit.SECONDS);
		} catch (Exception e) {
			System.out.println(e.getCause());
		}
		tPool.submitTask(new CoronaCallable(), TaskPriority.MIN);
	}
}

class ExceptionTask implements Runnable{

	@Override
	public void run() {
		throw new NullPointerException();
	}
}


class CoronaCallable implements Callable<Void> {

	@Override
	public Void call() throws Exception {
		Thread.sleep(4000);
		System.out.println("Corona for all");
		return null;
	}
}

class ZoomCallable implements Callable<Integer> {

	@Override
	public Integer call() throws Exception {
		System.out.println("Zoom for all");
		
		return 1;
	}
}

class AdMatayCallable implements Callable<String> {

	@Override
	public String call() throws Exception {
		System.out.println("Ad Matay");

		return "Ad Matay";
    }
}