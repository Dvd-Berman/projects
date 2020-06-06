package il.co.ilrd.threadpool;

import il.co.ilrd.waitablequeue.WaitableQueueSem;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

/*---------------------------ThreadPool class---------------------------------*/

public class ThreadPool implements Executor {	
	private List<TPThread<?>> threadList = new ArrayList<>();
	private WaitableQueueSem<ThreadPoolTask<?>> taskQueue = 
												new WaitableQueueSem<>();
	private final static int DEAFULT_NUM_THREADS = 
				Runtime.getRuntime().availableProcessors();
	private int numOfThreads = 0;
	private static final int VIP_PRIORITY = TaskPriority.NUM_PRIORITY.ordinal();
	private static final int LOWEST_PRIORITY = TaskPriority.MIN.ordinal() - 1;
	private Semaphore pauseSem = new Semaphore(0);
	private Semaphore awaitTermSem = new Semaphore(0);
	private boolean isShutDown = false;
	 
/*---------------------------TaskPriority------------------------------------*/
	
	public enum TaskPriority {
		MIN,
		NORM,
		MAX,
		NUM_PRIORITY
	}
	
/******************************************************************************/
		
	public ThreadPool() {
		this(DEAFULT_NUM_THREADS);
	}
		
	public <T> ThreadPool(int num) {
		if(0 > num) {
			/***** throw exception *****/
		}
		addThreadsToPool(num);
		activateThreads(num);
		numOfThreads = num;
	}
	
/******************************************************************************/
		
	public <T> Future<T> submitTask(Callable<T> callable) {
		
		return submitTask(callable, TaskPriority.NORM.ordinal());
	}
	
	public <T> Future<T> submitTask(Callable<T> callable, 
						TaskPriority taskPriority) {
		
		return submitTask(callable, taskPriority.ordinal());
	}
	
	public Future<Void> submitTask(Runnable runnable,
						TaskPriority taskPriority) {
        
		return submitTask(runnable, taskPriority, null);
	}
		
	public <T> Future<T> submitTask(Runnable runnable, 
						TaskPriority taskPriority, T t) {
	        
		 return submitTask(Executors.callable(runnable, t),
				 		taskPriority.ordinal());	
	}
		
/******************************************************************************/
	
	public void setNumberOfThread(int num) throws ShutDownException {
		if(num >= numOfThreads) {
			addThreadsToPool(num - numOfThreads);
			activateThreads(num - numOfThreads);
		}
		else {
			submitPrivateTasks(numOfThreads - num, 
					VIP_PRIORITY, new StopTask<Void>());
		}
		numOfThreads = num;
	}

/******************************************************************************/
	
	@Override /* @throws ShutDown exception */
	public void execute(Runnable runnable) {
		submitTask(runnable, TaskPriority.NORM);
	}
	
/******************************************************************************/
	
	public void pause() {
		submitPrivateTasks(numOfThreads, VIP_PRIORITY, new PauseTask<Void>());
	}

	private <T> void submitPrivateTasks(int amount, 
				int priority, Callable<T> privateCallable) {
		for(int i = 0; i < amount; ++i) {
			submitTask(privateCallable, priority);
		}
	}

/******************************************************************************/

	public void resume() {
		pauseSem.release(numOfThreads);
	}
	
/******************************************************************************/
	
	public void shutdown() {
		submitPrivateTasks(numOfThreads, LOWEST_PRIORITY, new StopTask<Void>());
		isShutDown = true;
	}
	
/******************************************************************************/

	public boolean awaitTermination(long timeout, TimeUnit unit) 
			throws InterruptedException {
		return awaitTermSem.tryAcquire(timeout, unit); 
	}
	
/*---------------------------ShutDownException--------------------------------*/
	
	private class ShutDownException extends RuntimeException {
		private static final long serialVersionUID = 1L;
	}
		
/*----------------------------stopTask class ---------------------------------*/
	
	private class StopTask <T> implements Callable<T>{

		@Override
		public  T call() throws Exception {
			TPThread<?> currThread = (TPThread<?>)Thread.currentThread();
			currThread.toRun = false;
			
			return null;
		}	
	}
	
/*----------------------------pauseTask class --------------------------------*/
	
	private class PauseTask <T> implements Callable<T>{

		@Override
		public  T call() throws Exception {
			pauseSem.acquire();
			
			return null;
		}	
	}
	
/******************************************************************************/
	
	private <T> Future<T> submitTask(Callable<T> callable, int taskPriority) {
		if(isShutDown) {
			throw new ShutDownException();
		}
		
		ThreadPoolTask<T> task  = 
				new ThreadPoolTask<>(taskPriority, callable);
		taskQueue.enqueue(task);
		
		return task.getFuture();	
	}
		
/******************************************************************************/
	
	private <T> void addThreadsToPool(int amount) {
		for(int i = numOfThreads; i < amount; ++i) {
			threadList.add(i, new TPThread<T>());
		}
	}
	
	private void activateThreads(int amount) {
		for(int i  = numOfThreads; i < amount; ++i) {
			threadList.get(i).start();
		}
	}
	
/*----------------------------TPThread class ---------------------------------*/
	
	private class TPThread<T> extends Thread {
		private boolean toRun = true;
		
		@Override
		public void run() {
			ThreadPoolTask<?> currTask = null;
			while(toRun) {
				try {
					currTask = taskQueue.dequeue();
					currTask.runTask();
				} catch (Exception e) {
					currTask.taskFuture.taskException = e;
					currTask.runTaskSem.release(); // runtask should throw exception not get 
				}
			}
			
			threadList.remove(this);
			if(threadList.isEmpty()) {
				awaitTermSem.release();
			}
		}
	}
	
/*----------------------------ThreadPoolTask class ---------------------------*/
	
	private class ThreadPoolTask<T> implements Comparable<ThreadPoolTask<T>> {	
		private int taskPriority;
		private Callable<T> callable;
		private TaskFuture taskFuture = new TaskFuture();
		private Semaphore runTaskSem = new Semaphore(0);
		
/******************************************************************************/
		
		public ThreadPoolTask(int taskPriority, Callable<T> callable) {
			this.taskPriority = taskPriority;
			this.callable = callable;
		}
		
/******************************************************************************/
		
		public TaskFuture getFuture() {
			
			return taskFuture;
		}
		
/******************************************************************************/

		private void runTask() throws Exception {
			taskFuture.returnObj = callable.call();
			taskFuture.isDone = true;
			runTaskSem.release();
		}
		
/*----------------------------TaskFuture class -------------------------------*/

		private class TaskFuture implements Future<T> {
			private boolean isDone = false;
			private boolean isCancelled = false;
			T returnObj = null;
			Throwable taskException = null;
			
/******************************************************************************/
			
			@Override
			public boolean cancel(boolean arg0) {
				isCancelled = taskQueue.remove(ThreadPoolTask.this);
				if(isCancelled) {
					isDone = true;
				}
				return isCancelled;
			}
			
/******************************************************************************/
			// call other get 
			@Override
			public T get() throws InterruptedException, ExecutionException {
				if(isCancelled) {
					return null;
				}
				runTaskSem.acquire();
				if(null != taskException) {
					throw new ExecutionException(taskException);
				}
				
				return returnObj;
			}
			
/******************************************************************************/
			
			@Override
			public T get(long timeout, TimeUnit timeUnit) 
					throws InterruptedException, ExecutionException, 
					TimeoutException {
				runTaskSem.tryAcquire(timeout, timeUnit);
				if(null != taskException) {
					throw new ExecutionException(taskException);
				}
				
				return returnObj;
			}

/******************************************************************************/

			@Override
			public boolean isCancelled() {
				
				return isCancelled;
			}
			
/******************************************************************************/

			@Override
			public boolean isDone() {
				
				return isDone;
			}	
		}
		
/******************************************************************************/

		@Override
		public int compareTo(ThreadPoolTask<T> otherTask) {
			
			return otherTask.taskPriority - this.taskPriority;
		}
	}
}