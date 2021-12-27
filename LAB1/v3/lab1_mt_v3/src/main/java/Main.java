import java.io.BufferedReader;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Hashtable;

public class Main {
    /*public static ArrayList<String> clone(ArrayList<String> list) {
        ArrayList<String> copy = new ArrayList<String>();
        for (int i = 0; i < list.size(); i+=1) {
            copy.add(new String(list.get(i)));
        }
        return copy;
    }*/

    public static void main(String[] args) throws IOException {
        Hashtable<String, ArrayList<String[]>> statistic = new Hashtable<String, ArrayList<String[]>>();
        HashSet<String> variables = new HashSet<String>();
        FileWriter fw = new FileWriter("C:\\Users\\79192\\Desktop\\Automata_Theory\\LAB1\\file_old.txt",true); //the true will append the new data
        fw.write("\r\n"); //дозапись конца стро
        fw.close();

        FileReader fileReader = new FileReader("C:\\Users\\79192\\Desktop\\Automata_Theory\\LAB1\\file_old.txt");
        BufferedReader reader = new BufferedReader(fileReader);
        //String line = reader.readLine();
        //StringReader lineReader = new StringReader(line+'\n');

        Lexer ftp = new Lexer(reader);//Lexer ftp = new Lexer(lineReader);
        //while (true) {
            /*System.out.println(line);
            if (line == null) {
                break;
            }*/
            ftp.yylex();//int res = ftp.yylex();
            for (int i = 0; i < ftp.lines_res.size(); i++) {
                String[] line_data = new String[2];
                line_data[0] = ftp.lines_res.get(i).get(0);
                line_data[1] = ftp.lines_res.get(i).get(1);

                if (!statistic.containsKey(ftp.lines_res.get(i).get(2))) {
                    ArrayList<String[]> lst = new ArrayList<String[]>();
                    lst.add(line_data);
                    statistic.put(ftp.lines_res.get(i).get(2), lst);
                }
                else {
                    if (!line_data[1].equals(statistic.get(ftp.lines_res.get(i).get(2)))) { //проверяем, если тип в очередной строке не совпадает с типом в предыдущей
                        variables.add(ftp.lines_res.get(i).get(2)); //заполняем сет теми переменными, которые необходимо будет вывести
                    }
                    statistic.get(ftp.lines_res.get(i).get(2)).add(line_data);
                }
            }
            //line = reader.readLine();
            //lineReader = new StringReader(line+'\n');
            //ftp.yyreset(lineReader);
        //}
        FileReader Reader2 = new FileReader("C:\\Users\\79192\\Desktop\\Automata_Theory\\LAB1\\file_old.txt");
        BufferedReader reader2 = new BufferedReader(Reader2);
        String line = reader2.readLine();
        for (int j = 0; j < ftp.answers.size(); j += 1) {
            if (line.equals("")) {
                break;
            }System.out.printf("%s : %s\n",line,ftp.answers.get(j));
            line = reader2.readLine();

        }
        for (String it: variables){
            for (String[] l: statistic.get(it)) {
                System.out.printf("%s - %s type: %s\n", l[0], it, l[1]);
            }
        }
    }
}