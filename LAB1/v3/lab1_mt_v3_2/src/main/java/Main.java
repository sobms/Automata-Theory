import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Hashtable;

public class Main {
    public static ArrayList<String> clone(ArrayList<String> list) {
        ArrayList<String> copy = new ArrayList<String>();
        for (int i = 0; i < list.size(); i+=1) {
            copy.add(new String(list.get(i)));
        }
        return copy;
    }

    public static void main(String[] args) throws IOException {
        //ArrayList<ArrayList<String>> statistic = new ArrayList<ArrayList<String>>();
        Hashtable<String, ArrayList<String[]>> statistic = new Hashtable<String, ArrayList<String[]>>();
        HashSet<String> variables = new HashSet<String>();

        FileReader fileReader = new FileReader("C:\\Users\\79192\\Desktop\\Automata_Theory\\LAB1\\file_old.txt");
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
                String[] line_data = new String[2];
                line_data[0] = ftp.results.get(0);
                line_data[1] = ftp.results.get(1);

                if (!statistic.containsKey(ftp.results.get(2))) {
                    ArrayList<String[]> lst = new ArrayList<String[]>();
                    lst.add(line_data);
                    statistic.put(ftp.results.get(2), lst);
                }
                else {
                    if (!line_data[1].equals(statistic.get(ftp.results.get(2)).get(0)[1])) { //проверяем, если тип в очередной строке не совпадает с типом в предыдущей
                        variables.add(ftp.results.get(2)); //заполняем сет теми переменными, которые необходимо будет вывести
                    }
                    statistic.get(ftp.results.get(2)).add(line_data);
                }
            }
            line = reader.readLine();
            lineReader = new StringReader(line+'\n');
            ftp.yyreset(lineReader);
        }
        for (String it: variables){
            for (String[] l: statistic.get(it)) {
                System.out.printf("%s - %s type: %s\n", l[0], it, l[1]);
            }
        }
    }
}