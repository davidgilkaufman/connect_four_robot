import java.io.*;
public class ConnectFour {
	public static int[][] board;
	public static final int NUM_ROW = 6;
	public static final int NUM_COL = 7;
	public static int nextPlayer = 1;
	public static int[] cols;
    
    public static void main(String[] args) {
		// init board
		board = new int[NUM_ROW][NUM_COL];
		cols = new int[NUM_COL];
		clearBoard();
        
        System.out.println();
        while (!gameOver() ){
           
           printBoard();
           System.out.print("Player "+nextPlayer);
           System.out.print(" next move:");
           
           if (nextPlayer == 1) humanMove();
           else aiMove();
        }
        printBoard();
    }

	public static void playMove(int col) {
		if ( col<0 || col >= NUM_COL)
			System.out.println("invalid input\n\n");
		else{
			if (cols[col]==NUM_ROW)
				System.out.println("Column full");
			else{
				int m_x= 5-cols[col];
				cols[col]++; 
				board[m_x][col] = nextPlayer;
				nextPlayer = 3-nextPlayer;
			}
		}
	}

	public static void clearBoard() {
		for (int row = 0; row < NUM_ROW;row++)
			for (int col = 0; col<NUM_COL; col++)
			{
				board[row][col]=0;
			}
		for (int col = 0 ; col < NUM_COL; col++)
			cols[col] = 0;        
	}

	public static boolean gameOver() {
		int col, row;

		// Check for horizontal victory conditions
		for (col=0; col<NUM_COL-3; col++) {
			for (row=0; row<NUM_ROW; row++) {
				if (board[row][col] != 0 &&
						board[row][col] == board[row][col+1] &&
						board[row][col] == board[row][col+2] &&
						board[row][col] == board[row][col+3])
					return true;
			}
		}

		// Check for vertical victory conditions
		for (col=0; col<NUM_COL; col++) {
			for (row=0; row<NUM_ROW-3; row++) {
				if (board[row][col] != 0 &&
						board[row][col] == board[row+1][col] &&
						board[row][col] == board[row+2][col] &&
						board[row][col] == board[row+3][col])
					return true;
			}
		}

		// Check for one diagonal victory conditions
		for (col=0; col<NUM_COL-3; col++) {
			for (row=0; row<NUM_ROW-3; row++) {
				if (board[row][col] != 0 &&
						board[row][col] == board[row+1][col+1] &&
						board[row][col] == board[row+2][col+2] &&
						board[row][col] == board[row+3][col+3])
					return true;
			}
		}

		// Check for the other diagonal victory conditions
		for (col=3; col<NUM_COL; col++) {
			for (row=0; row<NUM_ROW-3; row++) {
				if (board[row][col] != 0 &&
						board[row][col] == board[row+1][col-1] &&
						board[row][col] == board[row+2][col-2] &&
						board[row][col] == board[row+3][col-3])
					return true;
			}
		}

		return false;
	}

	public static void printBoard(){
		System.out.println("   0 1 2 3 4 5 6");
		for (int i = 0; i < 6; i++){
			System.out.print(i+ ": ");
			for (int j = 0; j< 7; j++){
				System.out.print(board[i][j]);
				System.out.print(" ");
			}
			System.out.println();
		}
		System.out.println();
	}

    public static void humanMove() {
        int n = 0;
        InputStreamReader isr = new InputStreamReader( System.in );
        BufferedReader br = new BufferedReader( isr );
        String s = null;
        try {
            s = br.readLine();
        }
        catch ( IOException ioe ) {
            // won't happen too often from the keyboard
        }

        n = Integer.parseInt(s);
        playMove(n);
    }
    
    private static boolean check_row(int r) { return ((r>=0) && (r <=5));}
    private static boolean check_col(int c) { return ((c>=0) && (c <=6));}
    private static boolean valid(int i, int j) {return ((check_row(i)) && (check_col(j)));}
    
    public static void aiMove() {
        int[][] bo=board;
        int[] c = cols;
        int me = nextPlayer;
        int oppo = 3 - me;
        
        int[] h= new int[7];
        for (int i = 0; i < 7; i++) h[i]=0;
        
        
        for (int i=0; i<7; i++)
        {
          if (c[i]==6) h[i]=0; else{
            int col = i;
            int row = 5 - c[i];
            
            
            if ((col>=3) 
                && (bo[row][col-1] == me)
                && (bo[row][col-2] == me)
                && (bo[row][col-3] == me))
                h[i]=h[i]+16;
            //right
            if ((col<=3) 
                && (bo[row][col+1] == me)
                && (bo[row][col+2] == me)
                && (bo[row][col+3] == me))
                h[i]=h[i]+16;
            //check y direction
            if ((row<=2) 
                && (bo[row+1][col] == me)
                && (bo[row+2][col] == me)
                && (bo[row+3][col] == me))
                h[i]=h[i]+16;
            //check left diagonal
            if ((col>=3) && (row<=2)
                && (bo[row+1][col-1] == me)
                && (bo[row+2][col-2] == me)
                && (bo[row+3][col-3] == me))
                h[i]=h[i]+16;
            
            if ((col<=3) && (row<=2)
                && (bo[row+1][col+1] == me)
                && (bo[row+2][col+2] == me)
                && (bo[row+3][col+3] == me))
                h[i]=h[i]+16;
            
            if ((col>=3) && (row>=3)
                && (bo[row-1][col-1] == me)
                && (bo[row-2][col-2] == me)
                && (bo[row-3][col-3] == me))
                h[i]=h[i]+16;
            
            if ((col<=3) && (row>=3)
                && (bo[row-1][col+1] == me)
                && (bo[row-2][col+2] == me)
                && (bo[row-3][col+3] == me))
                h[i]=h[i]+16;
            
            if ((col>=2) 
                && (bo[row][col-1] == me)
                && (bo[row][col-2] == me))
                h[i]=h[i]+4;
            //right
            if ((col<=4) 
                && (bo[row][col+1] == me)
                && (bo[row][col+2] == me))
                h[i]=h[i]+4;
            //check y direction
            if ((row<=3) 
                && (bo[row+1][col] == me)
                && (bo[row+2][col] == me))
                h[i]=h[i]+4;
            //check left diagonal
            if ((col>=2) && (row<=3)
                && (bo[row+1][col-1] == me)
                && (bo[row+2][col-2] == me))
                h[i]=h[i]+4;
            
            if ((col<=4) && (row<=3)
                && (bo[row+1][col+1] == me)
                && (bo[row+2][col+2] == me))
                h[i]=h[i]+4;
            
            if ((col>=2) && (row>=2)
                && (bo[row-1][col-1] == me)
                && (bo[row-2][col-2] == me))
                h[i]=h[i]+4;
            
            if ((col<=4) && (row>=2)
                && (bo[row-1][col+1] == me)
                && (bo[row-2][col+2] == me))
                h[i]=h[i]+4;
            
            if ((col>=1) 
                && (bo[row][col-1] == me))
                h[i]=h[i]+2;
            //right
            
            if ((col<=5) 
                && (bo[row][col+1] == me))
                h[i]=h[i]+2;
            //check y direction
            if ((row<=4) 
                && (bo[row+1][col] == me))
                h[i]=h[i]+2;
            //check left diagonal
            if ((col>=1) && (row<=4)
                && (bo[row+1][col-1] == me))
                h[i]=h[i]+2;
            
            if ((col<=5) && (row<=4)
                && (bo[row+1][col+1] == me))
                h[i]=h[i]+2;
            
            if ((col>=1) && (row>=1)
                && (bo[row-1][col-1] == me))
                h[i]=h[i]+2;
            
            if ((col<=5) && (row>=1)
                && (bo[row-1][col+1] == me))
                h[i]=h[i]+2;
            
            //check x direction.
            //left
            if ((col>=3) 
                && (bo[row][col-1] == oppo)
                && (bo[row][col-2] == oppo)
                && (bo[row][col-3] == oppo))
                h[i]=h[i]+8;
            //right
            if ((col<=3) 
                && (bo[row][col+1] == oppo)
                && (bo[row][col+2] == oppo)
                && (bo[row][col+3] == oppo))
                h[i]=h[i]+8;
            //check y direction
            if ((row<=2) 
                && (bo[row+1][col] == oppo)
                && (bo[row+2][col] == oppo)
                && (bo[row+3][col] == oppo))
                h[i]=h[i]+8;
            //check left diagonal
            if ((col>=3) && (row<=2)
                && (bo[row+1][col-1] == oppo)
                && (bo[row+2][col-2] == oppo)
                && (bo[row+3][col-3] == oppo))
                h[i]=h[i]+8;
            
            if ((col<=3) && (row<=2)
                && (bo[row+1][col+1] == oppo)
                && (bo[row+2][col+2] == oppo)
                && (bo[row+3][col+3] == oppo))
                h[i]=h[i]+8;
            
            if ((col>=3) && (row>=3)
                && (bo[row-1][col-1] == oppo)
                && (bo[row-2][col-2] == oppo)
                && (bo[row-3][col-3] == oppo))
                h[i]=h[i]+8;
            
            if ((col<=3) && (row>=3)
                && (bo[row-1][col+1] == oppo)
                && (bo[row-2][col+2] == oppo)
                && (bo[row-3][col+3] == oppo))
                h[i]=h[i]+8;
            
            if ((col>=2) 
                && (bo[row][col-1] == oppo)
                && (bo[row][col-2] == oppo))
                h[i]=h[i]+4;
            //right
            if ((col<=4) 
                && (bo[row][col+1] == oppo)
                && (bo[row][col+2] == oppo))
                h[i]=h[i]+4;
            //check y direction
            if ((row<=3) 
                && (bo[row+1][col] == oppo)
                && (bo[row+2][col] == oppo))
                h[i]=h[i]+4;
            //check left diagonal
            if ((col>=2) && (row<=3)
                && (bo[row+1][col-1] == oppo)
                && (bo[row+2][col-2] == oppo))
                h[i]=h[i]+4;
            
            if ((col<=4) && (row<=3)
                && (bo[row+1][col+1] == oppo)
                && (bo[row+2][col+2] == oppo))
                h[i]=h[i]+4;
            
            if ((col>=2) && (row>=2)
                && (bo[row-1][col-1] == oppo)
                && (bo[row-2][col-2] == oppo))
                h[i]=h[i]+4;
            
            if ((col<=4) && (row>=2)
                && (bo[row-1][col+1] == oppo)
                && (bo[row-2][col+2] == oppo))
                h[i]=h[i]+4;
            
            if ((col>=1) 
                && (bo[row][col-1] == oppo))
                h[i]=h[i]+2;
            //right
            
            if ((col<=5) 
                && (bo[row][col+1] == oppo))
                h[i]=h[i]+2;
            //check y direction
            if ((row<=4) 
                && (bo[row+1][col] == oppo))
                h[i]=h[i]+2;
            //check left diagonal
            if ((col>=1) && (row<=4)
                && (bo[row+1][col-1] == oppo))
                h[i]=h[i]+2;
            
            if ((col<=5) && (row<=4)
                && (bo[row+1][col+1] == oppo))
                h[i]=h[i]+2;
            
            if ((col>=1) && (row>=1)
                && (bo[row-1][col-1] == oppo))
                h[i]=h[i]+2;
            
            if ((col<=5) && (row>=1)
                && (bo[row-1][col+1] == oppo))
                h[i]=h[i]+2;            
          }              
        }
        int max = 0;
        int mm = 3;
        int sum = 0;
        for (int i=0; i<7; i++) {
            if (h[i]>max) {max=h[i]; mm=i;}
            sum= sum+h[i];
        }
        if (sum==0) mm = (int) (Math.random()*7);
        //System.out.println();
        //System.out.println(mm);
        playMove(mm);
    }
}

