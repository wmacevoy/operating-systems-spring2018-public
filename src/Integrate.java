import java.util.*;

class Worker implements Runnable {
    String name;
    Thread thread;
    Integrate problem;
    Worker(Integrate _problem, String _name) {
	problem = _problem;
	name = _name;
    }
    Random rng = new Random();
    public void run() {
	System.out.println("worker " + name + " running.");
	try {
	    Thread.sleep(rng.nextInt(10));
	} catch (InterruptedException ex) {
	    System.out.println("worker " + name + " interrupted.");
	}

	synchronized(problem.totalMutex) {
	    problem.total += rng.nextInt(100);
	}
	
	System.out.println("worker " + name + " done.");
    }
    void start() {
	thread = new Thread(this);
	thread.start();
    }
}

public class Integrate {
    double total = 0;
    Object totalMutex = new Object();

    public static void main(String [] args) {
	new Integrate().go();
    }

    void go() {
	Worker alice = new Worker(this, "alice");
	Worker bob = new Worker(this, "bob");
	alice.start();	
	bob.start();
	System.out.println("total=" + total);
	try {
	    Thread.sleep(100);
	} catch (InterruptedException ex) {
	    System.out.println("integrate interrupted.");
	}
	System.out.println("total=" + total);	
    }
}

