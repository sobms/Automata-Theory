package machine;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Hashtable;

public class Timing_Main {
    public static void main(String[] args) throws IOException {
        Machine1actions actions = new Machine1actions();
        Machine1 machine = new Machine1(actions);
        long sum = 0;
        ArrayList<String> line_list= new ArrayList<String>();
        for (int i=0; i < 10; i++){
            FileReader fileReader = new FileReader("C:\\Users\\79192\\Desktop\\Automata_Theory\\LAB1\\file.txt");
            BufferedReader reader = new BufferedReader(fileReader);

            while (true){
                String line = reader.readLine();
                if (line == null) {
                    break;
                }
                line += '\n';
                line_list.add(line);
            }

            long nano_startTime = System.nanoTime();
            for (int j = 0; j < line_list.size(); j++) {
                boolean result = machine.Check_str(line_list.get(j));
            }
            long nano_endTime = System.nanoTime();

            System.out.println(nano_endTime-nano_startTime);
            sum += nano_endTime-nano_startTime;
            line_list.clear();
        }
        System.out.println(sum/10);
        System.out.println(sum);
    }
}
