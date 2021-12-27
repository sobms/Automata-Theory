package machine;

import java.io.*;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Hashtable;

public class Main {

    public static void main(String[] args) throws IOException {
        Machine1actions actions = new Machine1actions();
        Machine1 machine = new Machine1(actions);
        int retcode = 0;
        Hashtable<String, ArrayList<String[]>> table = new Hashtable<String, ArrayList<String[]>>();
        HashSet<String> variables = new HashSet<String>();

        FileReader fileReader = new FileReader("C:\\Users\\79192\\Desktop\\Automata_Theory\\LAB1\\file.txt");
        BufferedReader reader = new BufferedReader(fileReader);
        String line = reader.readLine();
        while (true) {
            System.out.println(line);
            if (line == null) {
                break;
            }
            line += '\n';
            boolean result = machine.Check_str(line);
            if (result)
            {
                String var  = actions.Get_var();
                String[] line_data = new String[2];
                line_data[0]=actions.Get_type();
                line_data[1]=actions.Get_n_str();
                //заполняем хэш-таблицу, где ключи - названия переменных, а значения - тип и номер строки.
                if (!table.containsKey(var)) {
                    ArrayList<String[]> lst = new ArrayList<String[]>();
                    lst.add(line_data);
                    table.put(var, lst);
                }
                else {
                    if (!line_data[0].equals(table.get(var).get(0)[0])) { //проверяем, если тип в очередной строке не совпадает с типом в предыдущей
                        variables.add(var); //заполняем сет теми переменными, которые необходимо будет вывести
                    }
                    table.get(var).add(line_data);
                }
                System.out.println("Line is Correct");

            }
            else if (!result)
            {
                System.out.println("Line is Incorrect");
            }
            line = reader.readLine();
        }
        //output
        for (String it: variables){
            for (String[] l: table.get(it)) {
                System.out.printf("%s - %s type: %s\n", l[1], it, l[0]);
            }
        }
    }
}
//занесение в таблицу за O(n)... O(1) операция вставки одного элемента
//