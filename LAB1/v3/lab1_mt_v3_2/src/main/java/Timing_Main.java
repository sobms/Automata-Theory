import java.io.*;
import java.util.ArrayList;

public class Timing_Main {

        public static ArrayList<String> clone(ArrayList<String> list) {
            ArrayList<String> copy = new ArrayList<String>();
            for (int i = 0; i < list.size(); i+=1) {
                copy.add(new String(list.get(i)));
            }
            return copy;
        }

        public static void main(String[] args) throws IOException {

            long sum = 0;
            FileWriter fw = new FileWriter("C:\\Users\\79192\\Desktop\\Automata_Theory\\LAB1\\file_old.txt",true); //the true will append the new data
            fw.write("\r\n"); //дозапись конца строки
            fw.close();
            for (int i=0; i < 10; i++) {
                FileReader fileReader = new FileReader("C:\\Users\\79192\\Desktop\\Automata_Theory\\LAB1\\file.txt");
                BufferedReader reader = new BufferedReader(fileReader);
                long nano_startTime = System.nanoTime();
                Lexer ftp = new Lexer(reader);
                ftp.yylex();
                long nano_endTime = System.nanoTime();
                sum += nano_endTime - nano_startTime;
                System.out.println(nano_endTime - nano_startTime);
            }
            System.out.println(sum/10);
        }
}

