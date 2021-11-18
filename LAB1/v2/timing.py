import rstr
import numpy as np
import random
from tqdm import trange

n_str = 10000
var_num = n_str
file = open("C:\\Users\\79192\\Desktop\\Automata_Theory\\LAB1\\file.txt", "w")
variable_array = np.full(var_num, None)
for i in trange(0,var_num):
    variable_array[i] = rstr.xeger(r"([a-zA-Z][a-zA-Z0-9]{,15})")

for i in trange(0,n_str):
    idx = random.randint(0,var_num-1)
    line = rstr.xeger(r"[0-9]{4900,5031} (((int)|(long)|(short)) )?") + variable_array[idx] + rstr.xeger(r"(=(([a-zA-Z][a-zA-Z0-9]{,15})|([0-9]{1,5000})))?\n")
    if random.random()>0.6:
        indexes = random.sample(range(0,len(line)),len(line)//20) # исправить на 5 процентов
        for index in indexes:
            line = line[:index] + rstr.xeger(r"[0-9a-zA-Z ]") + line[index+1:]
    file.write(line)
file.close()    