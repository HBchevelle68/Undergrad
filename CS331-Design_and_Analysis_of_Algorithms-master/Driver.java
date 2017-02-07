import java.util.*;

public class Driver 
{
  static int cFlag = 0;
  static int dcFlag = 0;
  static int sFlag = 0;
	public static void main(String args[]) throws Exception
	{
        double carray[] = new double[30];
        double dcarray[] = new double[30];
        double sarray[] = new double[30];
        
        System.out.println("Setting up threads...");
        Runnable C_thread = () -> {
            int index = 0;
            for(int i = 2; i <= 1024; i*=2){
                int size = i;
                System.out.println("Calssical thread now testing with size " + i + "x" + i);
                Matrix m = new Matrix(size);
                m.generator_m(size);
                double startTime = System.nanoTime();
                m.classical_mult(m.a,m.b);
                double endTime = System.nanoTime();
                double classicTime = endTime - startTime;
                classicTime = (classicTime/(1000000000.0));
                carray[index] = classicTime;
                index++;
            }
            cFlag=1;
            System.out.println("Classical Thread Complete!!");
        }; 

        Runnable DC_thread = () -> {
            int index = 0;
            for(int i = 2; i <= 1024; i*=2){
                int size = i;
                System.out.println("DnC thread now testing with size " + i + "x" + i);
                Matrix m = new Matrix(size);
                m.generator_m(size);
                double startTime = System.nanoTime();
                m.div_and_conq(m.a,m.b);
                double endTime = System.nanoTime();
                double divandConqTime = endTime - startTime;
                divandConqTime = (divandConqTime/(1000000000.0));
                dcarray[index] = divandConqTime;
                index++;
            }
            dcFlag=1;
            System.out.println("DnC Thread Complete!!");
        };        

        Runnable S_thread = () -> {
            int index = 0;
            for(int i = 2; i <= 1024; i*=2){
                int size = i;
                System.out.println("Strassen thread now testing with size " + i + "x" + i);
                Matrix m = new Matrix(size);
                m.generator_m(size);
                double startTime = System.nanoTime();
                m.strassen(m.a,m.b);
                double endTime = System.nanoTime();
                double strassenTime = endTime - startTime;
                strassenTime = (strassenTime/(1000000000.0));
                sarray[index] = strassenTime;
                index++;
            }
            sFlag=1;
            System.out.println("Strassen Thread Complete!!");        
        };
        Thread threadC = new Thread(C_thread);
        threadC.start();
        System.out.println("Classical Matrix Multiplication thread running...");
        Thread threadDC = new Thread(DC_thread);
        threadDC.start();
        System.out.println("Divide and Conquer Matrix Multiplication thread running...");
        Thread threadS = new Thread(S_thread);
        threadS.start();
        System.out.println("Strassen Matrix Multiplication thread running...");
        Thread.sleep(1);
        System.out.println("Threads are running...please wait");
        int c = 2;
        while(sFlag!=1 || dcFlag!=1 || cFlag!=1){
            if(c++%2==0){System.out.println("Threads still calculating...");}
            Thread.sleep(7500);   

        }
        int index = 0;
        System.out.println("\nN    | Classical | DnC\t\t| Strassen");
        for(int i = 2; i<=1024; i=i*2){
            System.out.printf("%-4d | %-3.6f\t | %-3.6f\t| %-3.6f\n", i, carray[index], dcarray[index], sarray[index]);
            index++;
        }        

	}
}
