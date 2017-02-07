import java.util.Random;

public class Matrix 
{
	public int[][] a;
	public int[][] b; 
	int size;
	
	public Matrix(int size)
    {
		this.size = size;
    	a = new int[size][size];
	    b = new int[size][size];
	}

    public int[][] subtract_mtx(int[][] a, int[][] b)
    {
        int c[][] = new int[a.length][a.length];
        
        for(int i = 0; i<a.length; i++)
        {
            for(int j = 0; j<a.length; j++) 
            {
                c[i][j] = a[i][j] - b[i][j];
            }
        }
        return c;
    }

    public void print(int[][] a) 
    {
        for(int i = 0; i < a.length; i++)
        {
            for(int j = 0; j < a.length; j++ )
            {
                System.out.print(a[i][j] + "  ");
            }
        System.out.print("\n");
        }   
    }

	public void generator_m(int size)
	{
	    Random r = new Random( );
	    for(int i = 0; i < size; i++)
	    {
	        for(int j = 0; j < size; j++)
	        {
	            a[i][j] = r.nextInt(9);
	            b[i][j] = r.nextInt(9);
	        }
	    }
	}

	public int[][] classical_mult(int a[][], int b[][]) 
	{
		int n = size;
			   
		int[][] answer = new int[n][n];
			   
		for(int i = 0; i < n; i++) 
		{
			for(int j = 0; j < n; j++) 
			{ 
				for(int k = 0; k < n; k++) 
			    { 
					answer[i][j] += a[i][k] * b[k][j];
			    }
			} 
		}
		return answer;		
	}
	
    public void copyMatrix(int[][] from, int[][] to, int row, int col)
    {
        for(int i = 0, i2 = row; i < to.length; i++, i2++) 
        {  
            for(int j = 0, j2 = col; j< to.length; j++, j2++) 
            {  
                to[i][j] = from[i2][j2];
            }
        }
    }

    public int[][] div_and_conq(int [][] a, int [][] b)
    {
        int n = a.length; 
        int [][] c = new int[n][n];

        //base
        if (n == 1) 
        {
            c[0][0] = a[0][0] * b[0][0];
        }
        else 
        {

            int[][] a11 = new int[n/2][n/2];
            copyMatrix(a, a11, 0 , 0);
            int[][] a12 = new int[n/2][n/2];
            copyMatrix(a, a12, 0 , n/2);
            int[][] a21 = new int[n/2][n/2];
            copyMatrix(a, a21, n/2, 0);
            int[][] a22 = new int[n/2][n/2];
            copyMatrix(a, a22, n/2, n/2);
            int[][] b11 = new int[n/2][n/2];
            copyMatrix(b, b11, 0 , 0);
            int[][] b12 = new int[n/2][n/2];
            copyMatrix(b, b12, 0 , n/2);
            int[][] b21 = new int[n/2][n/2];
            copyMatrix(b, b21, n/2, 0);
            int[][] b22 = new int[n/2][n/2];
            copyMatrix(b, b22, n/2, n/2);

            int[][] c11 = matrx_addition(div_and_conq(a11, b11), div_and_conq(a12, b21));
            int[][] c12 = matrx_addition(div_and_conq(a11, b12), div_and_conq(a12, b22));
            int[][] c21 = matrx_addition(div_and_conq(a21, b11), div_and_conq(a22, b21));
            int[][] c22 = matrx_addition(div_and_conq(a21, b12), div_and_conq(a22, b22));
            
            matrx_combo(c11, c, 0 , 0);
            matrx_combo(c12, c, 0 , n/2);
            matrx_combo(c21, c, n/2, 0);
            matrx_combo(c22, c, n/2, n/2);
            }
        return c;
    }
    
     //Strassenmethod
     
    public int[][] strassen(int [][] a, int [][] b)
    {
        int n = a.length;
        int [][] c = new int[n][n];
        
        //base
        if (n == 1) 
        {
            c[0][0] = a[0][0] * b[0][0];
        }
        else 
        {

        	int[][] a11 = new int[n/2][n/2];
        	copyMatrix(a, a11, 0 , 0);
        	int[][] a12 = new int[n/2][n/2];
        	copyMatrix(a, a12, 0 , n/2);
        	int[][] a21 = new int[n/2][n/2];
        	copyMatrix(a, a21, n/2, 0);
        	int[][] a22 = new int[n/2][n/2];
        	copyMatrix(a, a22, n/2, n/2);
        	int[][] b11 = new int[n/2][n/2];
          	copyMatrix(b, b11, 0 , 0);
        	int[][] b12 = new int[n/2][n/2];
        	copyMatrix(b, b12, 0 , n/2);
        	int[][] b21 = new int[n/2][n/2];
        	copyMatrix(b, b21, n/2, 0);
        	int[][] b22 = new int[n/2][n/2];
        	copyMatrix(b, b22, n/2, n/2);

        	int[][] P = strassen(matrx_addition(a11, a22), matrx_addition(b11, b22));
        	int[][] Q = strassen(matrx_addition(a21, a22), b11);
        	int[][] R = strassen(a11, subtract_mtx(b12, b22));
        	int[][] S = strassen(a22, subtract_mtx(b21, b11));
        	int[][] T = strassen(matrx_addition(a11, a12), b22);
        	int[][] U = strassen(subtract_mtx(a21, a11), matrx_addition(b11, b12));
        	int[][] V = strassen(subtract_mtx(a12, a22), matrx_addition(b21, b22));

        	int[][] c11 = matrx_addition(subtract_mtx(matrx_addition(P, S), T), V);
        	int[][] c12 = matrx_addition(R, T);
        	int[][] c21 = matrx_addition(Q, S);
        	int[][] c22 = matrx_addition(subtract_mtx(matrx_addition(P, R), Q), U);

        	matrx_combo(c11, c, 0 , 0);
        	matrx_combo(c12, c, 0 , n/2);
        	matrx_combo(c21, c, n/2, 0);
        	matrx_combo(c22, c, n/2, n/2);
    	}
        return c;
    }

    public void matrx_combo(int[][] a, int[][] b, int row, int col)
    {
        for(int i = 0, i2 = row; i<a.length; i++, i2++) 
        {
            for(int j = 0, j2 = col; j<a.length; j++, j2++) 
            {
                b[i2][j2] = a[i][j];
            }
        }
    }

    public int[][] matrx_addition(int[][] a, int[][] b)
    {
        int c[][] = new int[a.length][b.length];
        
        for(int i = 0; i<a.length; i++) 
        {
            for(int j = 0; j<b.length; j++) 
            {
                c[i][j] = a[i][j] + b[i][j];
            }
        }
        return c;
    }

}
