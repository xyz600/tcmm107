import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.io.*;
import java.security.*;
import java.util.*;
import javax.swing.*;

public class PointsOnGridVis {
    private static int minN = 5, maxN = 50;
    private static int minM = 2;
    private static int minK1 = 1, minK2 = 0;
    private static int minNum = 0, maxNum = 9;

    int H;
    int W;
    int P;
    int Q;
    int Kmin;
    int Kmax;

    String[] numbers;

    // -----------------------------------------
    void generate(String seedStr) {
        try {
            // generate test case
            SecureRandom r1 = SecureRandom.getInstance("SHA1PRNG");
            long seed = Long.parseLong(seedStr);
            r1.setSeed(seed);

            H = r1.nextInt(maxN - minN + 1) + minN;
            W = r1.nextInt(maxN - minN + 1) + minN;

            P = r1.nextInt((H - 1) - minM + 1) + minM;
            Q = r1.nextInt((W - 1) - minM + 1) + minM;

            Kmax = r1.nextInt((P * Q - 1) - minK1 + 1) + minK1;
            Kmin = r1.nextInt((Kmax - 1) - minK2 + 1) + minK2;

            if (seed == 1) {
                H = 8;
                W = 8;
                P = 3;
                Q = 4;
                Kmin = 0;
                Kmax = 5;
            }

            numbers = new String[H];
            for (int i = 0; i < H; i++) {
                numbers[i] = "";
                for (int k = 0; k < W; k++)
                    numbers[i] += (r1.nextInt(maxNum - minNum + 1) + minNum);
            }

            System.out.println("seed = " + seedStr);
            System.out.println(
                    "H = " + H + ", W = " + W + ", h = " + P + ", w = " + Q + ", Kmin =" + Kmin + ", Kmax = " + Kmax);
            for (int i = 0; i < H; i++)
                System.out.println(numbers[i]);
        } catch (Exception e) {
            addFatalError("An exception occurred while generating test case.");
            e.printStackTrace();
        }
    }

    // -----------------------------------------
    int getScore() {
        int total = 0;
        for (int i = 0; i < H; ++i)
            for (int j = 0; j < W; ++j)
                if (board[i].charAt(j) == 'x')
                    total += (int) (numbers[i].charAt(j) - '0');

        return total;
    }

    // -----------------------------------------
    public double runTest(String seed) {
        try {
            generate(seed);

            if (proc != null) {
                // call the solution
                String[] grid;
                try {
                    grid = callSolution();
                } catch (Exception e) {
                    addFatalError("Failed to get result from your solution.");
                    return -1.0;
                }

                // check the return and score it
                if (grid == null) {
                    addFatalError("Your return contained invalid number of elements.");
                    return -1.0;
                }
                if (grid.length != H) {
                    addFatalError("Your return did not contain " + H + " elements.");
                    return -1.0;
                }

                for (int r = 0; r < H; r++) {
                    if (grid[r].length() != W) {
                        addFatalError("Row " + r + " does not contain " + W + " elements.");
                        return -1.0;
                    }

                    for (int c = 0; c < W; c++) {
                        char a = grid[r].charAt(c);
                        if (a != '.' && a != 'x') {
                            addFatalError("Row " + r + " column " + c + " contains an illegal character");
                            return -1.0;
                        }
                    }
                }

                // check that all subgrids contains K or fewer points
                for (int r1 = 0; r1 < H - (P - 1); r1++)
                    for (int c1 = 0; c1 < W - (Q - 1); c1++) {
                        int count = 0;
                        for (int r2 = r1; r2 < r1 + P; r2++)
                            for (int c2 = c1; c2 < c1 + Q; c2++)
                                if (grid[r2].charAt(c2) == 'x')
                                    count++;

                        if (count > Kmax) {
                            addFatalError(
                                    "Subgrid starting at (" + r1 + "," + c1 + ") contains too many painted cells");
                            return -1.0;
                        }
                        if (count < Kmin) {
                            addFatalError(
                                    "Subgrid starting at (" + r1 + "," + c1 + ") contains not enough painted cells");
                            return -1.0;
                        }
                    }

                board = grid;

                if (vis) {
                    jf.setSize((W + 3) * SZ + 50, H * SZ + 40);
                    jf.setVisible(true);
                    draw();
                }
            }

            return getScore();
        } catch (Exception e) {
            addFatalError("An exception occurred while trying to get your program's results.");
            e.printStackTrace();
            return -1.0;
        }
    }

    // ------------- visualization part ------------
    JFrame jf;
    Vis v;
    static String exec;
    static boolean vis;
    static Process proc;
    InputStream is;
    OutputStream os;
    BufferedReader br;
    static int SZ;

    static String[] board;

    // -----------------------------------------
    private String[] callSolution() throws IOException, NumberFormatException {
        if (exec == null)
            return null;

        String s = H + "\n" + W + "\n" + P + "\n" + Q + "\n" + Kmin + "\n" + Kmax + "\n" + H + "\n";
        os.write(s.getBytes());
        for (int i = 0; i < H; i++) {
            s = numbers[i] + "\n";
            os.write(s.getBytes());
        }
        os.flush();

        // and get the return value
        String[] grid = new String[Integer.parseInt(br.readLine())];
        for (int i = 0; i < grid.length; i++)
            grid[i] = br.readLine();
        return grid;
    }

    // -----------------------------------------
    void draw() {
        if (!vis)
            return;
        v.repaint();
    }

    // -----------------------------------------
    public class Vis extends JPanel implements MouseListener, WindowListener {
        public void paint(Graphics g) {

            // background
            g.setColor(new Color(0xDDDDDD));
            g.fillRect(0, 0, W * SZ + 120, H * SZ + 40);
            g.setColor(Color.WHITE);
            g.fillRect(0, 0, W * SZ, H * SZ);

            for (int i = 0; i < H; ++i)
                for (int j = 0; j < W; ++j) {
                    if (board[i].charAt(j) == 'x') {
                        g.setColor(Color.CYAN);
                        g.fillRect(j * SZ + 1, i * SZ + 1, SZ - 1, SZ - 1);
                    }

                    g.setFont(new Font("Arial", Font.BOLD, 14));
                    g.setColor(Color.BLACK);
                    g.drawString("" + numbers[i].charAt(j), j * SZ + SZ / 2 - 3, i * SZ + SZ / 2 + 3);
                }

            // lines between cells
            g.setColor(Color.BLACK);
            for (int i = 0; i <= H; i++)
                g.drawLine(0, i * SZ, W * SZ, i * SZ);
            for (int i = 0; i <= W; i++)
                g.drawLine(i * SZ, 0, i * SZ, H * SZ);

            g.setFont(new Font("Arial", Font.BOLD, 14));
            g.drawString("SCORE", SZ * W + 25, 30);
            g.drawString(String.format("%d", getScore()), SZ * W + 25, 50);
        }

        // -------------------------------------
        public Vis() {
            jf.addWindowListener(this);
        }

        // -------------------------------------
        // WindowListener
        public void windowClosing(WindowEvent e) {
            if (proc != null)
                try {
                    proc.destroy();
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            System.exit(0);
        }

        public void windowActivated(WindowEvent e) {
        }

        public void windowDeactivated(WindowEvent e) {
        }

        public void windowOpened(WindowEvent e) {
        }

        public void windowClosed(WindowEvent e) {
        }

        public void windowIconified(WindowEvent e) {
        }

        public void windowDeiconified(WindowEvent e) {
        }

        // -------------------------------------
        // MouseListener
        public void mouseClicked(MouseEvent e) {
        }

        public void mousePressed(MouseEvent e) {
        }

        public void mouseReleased(MouseEvent e) {
        }

        public void mouseEntered(MouseEvent e) {
        }

        public void mouseExited(MouseEvent e) {
        }
    }

    // -----------------------------------------
    public PointsOnGridVis(String seed) {
        try {
            if (vis) {
                jf = new JFrame();
                v = new Vis();
                jf.getContentPane().add(v);
            }
            if (exec != null) {
                try {
                    Runtime rt = Runtime.getRuntime();
                    proc = rt.exec(exec);
                    os = proc.getOutputStream();
                    is = proc.getInputStream();
                    br = new BufferedReader(new InputStreamReader(is));
                    new ErrorReader(proc.getErrorStream()).start();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            System.out.println("Score = " + runTest(seed));
            if (proc != null)
                try {
                    proc.destroy();
                } catch (Exception e) {
                    e.printStackTrace();
                }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // -----------------------------------------
    public static void main(String[] args) {
        String seed = "1";
        vis = true;
        SZ = 20;
        for (int i = 0; i < args.length; i++) {
            if (args[i].equals("-seed"))
                seed = args[++i];
            if (args[i].equals("-exec"))
                exec = args[++i];
            if (args[i].equals("-novis"))
                vis = false;
            if (args[i].equals("-size"))
                SZ = Integer.parseInt(args[++i]);
        }
        if (seed.equals("1"))
            SZ = 50;

        PointsOnGridVis f = new PointsOnGridVis(seed);
    }

    // -----------------------------------------
    void addFatalError(String message) {
        System.out.println(message);
    }
}

class ErrorReader extends Thread {
    InputStream error;

    public ErrorReader(InputStream is) {
        error = is;
    }

    public void run() {
        try {
            byte[] ch = new byte[50000];
            int read;
            while ((read = error.read(ch)) > 0) {
                String s = new String(ch, 0, read);
                System.out.print(s);
                System.out.flush();
            }
        } catch (Exception e) {
        }
    }
}
