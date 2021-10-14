import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.StringReader;
import java.util.ArrayList;

public class Main {
    public static ArrayList<String> clone(ArrayList<String> list) {
        ArrayList<String> copy = new ArrayList<String>();
        for (int i = 0; i < list.size(); i+=1) {
            copy.add(new String(list.get(i)));
        }
        return copy;
    }

    public static void main(String[] args) throws IOException {
        ArrayList<ArrayList<String>> statistic = new ArrayList<ArrayList<String>>();
        FileReader fileReader = new FileReader("D:\\java\\lab1_mt_v3\\src\\main\\java\\file");
        BufferedReader reader = new BufferedReader(fileReader);
        String line = reader.readLine();
        StringReader lineReader = new StringReader(line+'\n');
        Lexer ftp = new Lexer(lineReader);
        while (true) {
            System.out.println(line);
            if (line == null) {
                break;
            }
            int res = ftp.yylex();
            System.out.println(res);
            if (res > 0) {
                statistic.add(clone(ftp.results));
            }
            line = reader.readLine();
            lineReader = new StringReader(line+'\n');
            ftp.yyreset(lineReader);

        }
    }
}