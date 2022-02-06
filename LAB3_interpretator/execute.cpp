#include <stdio.h>
#include "StreeNodes.h"
#include <algorithm>
#include <stdlib.h>
#include <time.h>

Program_data data = Program_data();
bool win = false;
std::ofstream path;
std::vector<std::vector<bool>> visited;

int get_dim_factor(int idx, std::vector<InodeType*> vec_dims);

int intToBool(int val);

void function_destructor(std::string key) {
    for (auto& var: data.func_table[key]) {
        delete var.second;
    }
    data.func_table[key].clear();
    data.func_table.erase(key);
}

YYSTYPE execute(InodeType* node) {
    if (!node || win || !data.robot.get_state()) {
        return 0;
    }
    int key_node = typeid(*node).hash_code();
    int key_val = typeid(ValNode).hash_code();
    int key_var = typeid(VarNode).hash_code();
    int key_oper = typeid(OperNode).hash_code();
    if  (key_node == key_val) {
        return node->value;
    }
    else if (key_node == key_var) {
        VarNode* var_node = dynamic_cast<VarNode*>(node);
        std::string name = var_node->get_name();
        return &name;
    }
    else if (key_node == key_oper) {
                OperNode* op_node = dynamic_cast<OperNode*>(node);
                std::string op_name = op_node->get_operation_name();
                if (op_name == "PROGRAM") { //+
                    std::cout << "PROGRAM" << std::endl;
                    path.open ("robot_path.txt");
                    srand(time(NULL));

                    std::string coordinates = std::to_string(data.robot.get_pos_x()) + "," + std::to_string(data.robot.get_pos_y()) + "\n";
                    path << coordinates;
                    data.this_func = "$global$";
                    // go throw children of root
                    for (auto child_node: op_node->get_child_vec()) {
                        OperNode* oper_node = dynamic_cast<OperNode*>(child_node);
                        if (oper_node->get_operation_name() == "VARDECLARATION") {
                            execute(child_node);
                        } else if (oper_node->get_operation_name() == "Func: main") {
                            data.this_func = "main";
                            data.func_table["main"] = std::unordered_map<std::string, variable*>();
                            data.functions["main"] = child_node;
                            execute(child_node);

                            function_destructor("main");
                            path.close();
                        }
                        else if (oper_node->get_operation_name() == "SETSTART") {
                            std::cout << "SETSTART" << std::endl;
                            OperNode* x_node = dynamic_cast<OperNode*>(oper_node->get_child_vec()[0]);
                            std::string namex = *(x_node->get_child_vec()[0]->value.strval);
                            data.func_table[data.this_func][namex]->values[0] = new ValNode(data.robot.get_pos_x(), int(type_t::INTVAL));
                            std::cout << data.this_func << "::" << *(data.func_table[data.this_func][namex]) << std::endl;
                            
                            OperNode* y_node = dynamic_cast<OperNode*>(oper_node->get_child_vec()[1]);
                            std::string namey = *(y_node->get_child_vec()[0]->value.strval);
                            data.func_table[data.this_func][namey]->values[0] = new ValNode(data.robot.get_pos_y(), int(type_t::INTVAL));
                            std::cout << data.this_func << "::" << *(data.func_table[data.this_func][namey]) << std::endl; 
                        } else { // function declaration 
                            std::string op_name = oper_node->get_operation_name();
                            std::string f_name = op_name.substr(6, op_name.size()-6);
                            
                            data.functions[f_name] = child_node;
                        }
                    }
                }
                else if (op_name == "CALL") { //+
                    // take from child name of called function
                    std::string name_func = *(execute(op_node->get_child_vec()[0]).strval);
                    std::string prev_func = data.this_func;
                    if (data.functions.count(name_func)) {
                        if (data.func_table.count(name_func) > 0) { // в случае рекурсивного вызова
                            int i = 1;
                            while (data.func_table.count(name_func + std::to_string(i))>0) {
                                i++;
                            }
                            data.func_table[name_func + std::to_string(i)] = std::unordered_map<std::string, variable*>();
                            data.this_func = name_func + std::to_string(i);
                        } else {
                            data.this_func = name_func;
                        }
                        execute(data.functions[name_func]);
                        function_destructor(data.this_func);
                        data.this_func = prev_func;
                    } else {
                        std::cout << "Error! Function " << name_func << " was not declared!" << std::endl;
                    }
                }
                else if (op_name == "CYCLE") { //+
                    while(execute(op_node->get_child_vec()[0]).intval) {
                        for (int i = 1; i < op_node->get_child_vec().size(); i++) {
                            execute(op_node->get_child_vec()[i]);
                        }
                    }
                }

                else if (op_name == "SWITCH") { //+
                    for (int i = 0; i < op_node->get_child_vec().size(); i++) {
                            if (execute(op_node->get_child_vec()[i]).intval) {
                                break;
                            };
                    }
                }

                else if (op_name == "CONDITION") { //+
                    if(execute(op_node->get_child_vec()[0]).intval) {
                        
                        for (int i = 1; i < op_node->get_child_vec().size(); i++) {
                            execute(op_node->get_child_vec()[i]);
                        }
                        return 1;
                    }
                    else {
                        return 0;
                    }
                }

                else if (op_name == "ADDRESSING") { //return value saved in variable //+
                    std::vector<InodeType*> vec_nodes = op_node->get_child_vec();
                    std::string variable_name = *(execute(vec_nodes[0]).strval);
                    std::string field_view;
                    if (data.func_table[data.this_func].count(variable_name)) {
                        field_view = data.this_func;
                    } else {
                        field_view = "$global$";
                    }
                    if (vec_nodes.size()>1) {
                        // addressing to element of the array
                        int idx = 0;
                        std::vector<InodeType*> vec_dim = data.func_table[field_view][variable_name]->mas_dim;
                        for (int i = 1; i < vec_nodes.size(); i++) { //проход по сохраненным индексам
                            int factor = 1;
                            for (int j = i; j < vec_dim.size(); j++) {
                                factor*=execute(vec_dim[j]).intval;    
                            }
                            int idx = execute(vec_nodes[i]).intval;
                            //error run time
                            if (idx>vec_dim[i-1]->value.intval) {
                                std::cout << "Error! Index " << idx << " out of range! ADDRESSING return 0..." << std::endl;
                                return 0;
                            }
                            //error
                            idx+=i*factor;
                        }
                        return data.func_table[field_view][*(vec_nodes[0]->value.strval)]->values[idx]->value;
                    }
                    else {
                        // addressing to variable
                        if ((data.func_table[field_view][variable_name]->dims_count == 1) && 
                        (data.func_table[field_view][variable_name]->mas_dim[0]->value.intval == 1)){
                            //std::cout << variable_name << "::" << data.func_table[field_view][variable_name]->values[0]->value.intval << std::endl;
                            return data.func_table[field_view][variable_name]->values[0]->value;
                        } else {
                            // array
                            //it's better to teturn it name
                            return &(data.func_table[field_view][variable_name]->name);
                        }
                    }
                }
                else if (op_name == "ASSIGNMENT") { //+         
                    std::vector<InodeType*> vars_vec;
                    std::vector<InodeType*> vals_vec;

                    OperNode* variable_node = dynamic_cast<OperNode*>(op_node->get_child_vec()[0]);
                    vars_vec.insert(vars_vec.end(), variable_node->get_child_vec().begin(), variable_node->get_child_vec().end());
                    OperNode* val_node = dynamic_cast<OperNode*>(op_node->get_child_vec()[1]);
                    for (auto child_node: val_node->get_child_vec()) {
                        if (typeid(*child_node).hash_code() == typeid(OperNode).hash_code()) {
                            OperNode* oper_node = dynamic_cast<OperNode*>(child_node);
                            child_node->value.intval = execute(oper_node).intval;
                            vals_vec.push_back(child_node);
                        } else {
                            vals_vec.push_back(child_node);
                        }
                    }                    

                    for (auto addr: vars_vec) {
                        OperNode* addressing = dynamic_cast<OperNode*>(addr); ///
                        std::vector<InodeType*> vec_nodes = addressing->get_child_vec();
                        std::string variable_name = *(execute(vec_nodes[0]).strval);
                        std::string field_view;
                        if (data.func_table[data.this_func].count(variable_name)) {
                            field_view = data.this_func;
                        } else {
                            field_view = "$global$";
                        }
                        if (vec_nodes.size()>1) {
                            // addressing to element of the array
                            int idx = 0;
                            std::vector<InodeType*> vec_dim = data.func_table[field_view][variable_name]->mas_dim;
                            
                            for (int i = 1; i < vec_nodes.size(); i++) { //проход по сохраненным индексам
                                int factor = 1;
                                for (int j = i; j < vec_dim.size(); j++) {
                                        factor*=execute(vec_dim[j]).intval;  
                                }
                                idx += execute(vec_nodes[i]).intval*factor;
                                
                            }
                            data.func_table[field_view][variable_name]->values[idx]->value.intval = execute(vals_vec[0]).intval;
                            std::cout << field_view << "::" << *(data.func_table[field_view][variable_name]) << std::endl;
                        }
                        else {
                            // addressing to variable
                            for (int i = 0; i < data.func_table[field_view][variable_name]->values.size(); i++) {
                                data.func_table[field_view][variable_name]->values[i]->value.intval = execute(vals_vec[i]).intval;
                            }
                            
                            std::cout << field_view << "::" << *(data.func_table[field_view][variable_name]) << std::endl;
                        }
                    }
                }
                else if (op_name == "VARDECLARATION") {//+
                    for (auto child_node: op_node->get_child_vec()) {
                        execute(child_node);
                    }
                }
                else if (op_name == "VAR") { //+
                    std::vector<InodeType*> child_vec = op_node->get_child_vec();
                    std::string var_name = *(execute(child_vec[0]).strval);
                    int type = execute(child_vec[1]).intval;
                    bool isconst = execute(child_vec[2]).intval;
                    int dims_num = execute(child_vec[3]).intval;
                    std::vector<InodeType*> dim_vec;
                    std::vector<InodeType*> vals_vec;
                    for (int i=4; i<4+dims_num; i++) {
                        
                        dim_vec.push_back(child_vec[i]);
                    }
                    for (int i=4+dims_num; i<child_vec.size(); i++) {
                        InodeType* value_copy = child_vec[i]->copy();
                        //так как значения переменных в разных областях видимости не должны зависеть друг от друга
                        vals_vec.push_back(value_copy);
                    }
                    data.func_table[data.this_func][var_name] = new variable(var_name, type, 
                    isconst, dims_num, dim_vec, vals_vec);
                    std::cout << data.this_func << "::" << *(data.func_table[data.this_func][var_name]) << std::endl;
                }
                else if (op_name == "ADD") { 
                    //std::cout << "ADD: ";
                    int sum = 0;
                    for (auto child_node: op_node->get_child_vec()) {
                        int a;
                        a = execute(child_node).intval;
                        //std::cout << a << " ";
                        sum += a;
                    }
                    //std::cout << std::endl;
                    return sum;
                }
                else if (op_name == "MUL") {
                    int mul = 0;
                     for (auto child_node: op_node->get_child_vec()) {
                        mul *= execute(child_node).intval;
                    }
                    return mul;
                }
                else if (op_name == "SUB") {
                    return execute(op_node->get_child_vec()[0]).intval - execute(op_node->get_child_vec()[1]).intval;
                }
                else if (op_name == "DIV") {
                    return execute(op_node->get_child_vec()[0]).intval / execute(op_node->get_child_vec()[1]).intval;
                }
                else if (op_name == "OR") {
                    int res = 0;
                     for (auto child_node: op_node->get_child_vec()) {
                        res = res || intToBool(execute(child_node).intval);
                    }
                    return res;
                }
                else if (op_name == "AND") {
                    int res = 1;
                     for (auto child_node: op_node->get_child_vec()) {
                        res = res && intToBool(execute(child_node).intval);
                    }
                    return res;
                }
                else if (op_name == "NOT") {
                    return int(!execute(op_node->get_child_vec()[0]).intval);
                }
                else if (op_name == "EQ") {  
                    int eq = 1;
                    for (auto child_node: op_node->get_child_vec()) {
                        eq = eq * (execute(op_node->get_child_vec()[0]).intval == execute(child_node).intval);

                    }
                    
                    return eq;
                }
                else if (op_name == "MAX") {
                    std::vector<int> arr;
                    for (auto child: op_node->get_child_vec()) {
                            arr.push_back(execute(child).intval);
                    }
                    int max = *max_element(arr.begin(), arr.end());
                    return max;
                }
                else if (op_name == "MIN") {
                    std::vector<int> arr;
                    for (auto child: op_node->get_child_vec()) {
                            arr.push_back(execute(child).intval);
                    }
                    int min = *min_element(arr.begin(), arr.end());
                    return min;
                }
                else if (op_name == "LEFT") {
                    int step = op_node->get_child_vec()[0]->value.intval;
                    for (int i = 0; i < step; i++) {
                        int x = data.robot.get_pos_x();
                        int y = data.robot.get_pos_y(); 
                        std::string coordinates = std::to_string(x-1) + "," + std::to_string(y) + "\n";
                        if (x == 0) {
                            return 0;
                        }
                        if (data.actual_map.get_cell(x-1, y) == cellval::EXIT) {
                            data.robot.move_left(1);
                            path << coordinates;
                            win = true;
                            std::cout << "WIN!!!!!!" << std::endl;
                            return 0;
                        }
                        else if (data.actual_map.get_cell(x-1, y) == cellval::WALL) {
                            data.robot.move_left(1);
                            path << coordinates;
                            data.robot.destroy();
                            std::cout << "..............DESTRUCTION.............." << std::endl;
                            return 0;
                        }
                        else {
                            data.robot.move_left(1);
                            path << coordinates;
                            data.robot.define_explored_map(data.actual_map);
                        }
                        std::cout << "coordinates: " << data.robot.get_pos_x() << " " << data.robot.get_pos_y() << std::endl;
                    }
                }
                else if (op_name == "RIGHT") {
                    int step = op_node->get_child_vec()[0]->value.intval;
                    for (int i = 0; i < step; i++) {
                        int x = data.robot.get_pos_x();
                        int y = data.robot.get_pos_y();
                        std::string coordinates = std::to_string(x+1) + "," + std::to_string(y) + "\n";
                        if (x >= data.actual_map.get_side() - 1) {
                            return 0;
                        }
                        if (data.actual_map.get_cell(x+1, y) == cellval::EXIT) {
                            data.robot.move_right(1);
                            path << coordinates;
                            win = true;
                            std::cout << "WIN!!!!!!" << std::endl;
                            return 0;
                        }
                        else if (data.actual_map.get_cell(x+1, y) == cellval::WALL) {
                            data.robot.move_right(1);
                            path << coordinates;
                            data.robot.destroy();
                            std::cout << "..............DESTRUCTION.............." << std::endl;
                            return 0;
                        }
                        else {
                            data.robot.move_right(1);
                            path << coordinates;
                            data.robot.define_explored_map(data.actual_map);
                        }
                        std::cout << "coordinates: " << data.robot.get_pos_x() << " " << data.robot.get_pos_y() << std::endl;
                    }
                }
                else if (op_name == "UP") {
                    int step = op_node->get_child_vec()[0]->value.intval;
                    for (int i = 0; i < step; i++) {
                        int x = data.robot.get_pos_x();
                        int y = data.robot.get_pos_y();
                        std::string coordinates = std::to_string(x) + "," + std::to_string(y-1) + "\n";
                        if (y == 0) {
                            return 0;
                        }
                        if (data.actual_map.get_cell(x, y-1) == cellval::EXIT) {
                            data.robot.move_up(1);
                            path << coordinates;
                            win = true;
                            std::cout << "WIN!!!!!!" << std::endl;
                            return 0;
                        }
                        else if (data.actual_map.get_cell(x, y-1) == cellval::WALL) {
                            data.robot.move_up(1);
                            path << coordinates;
                            data.robot.destroy();
                            std::cout << "..............DESTRUCTION.............." << std::endl;
                            return 0;
                        }
                        else {
                            data.robot.move_up(1);
                            path << coordinates;
                            data.robot.define_explored_map(data.actual_map);
                        }
                        std::cout << "coordinates: " << data.robot.get_pos_x() << " " << data.robot.get_pos_y() << std::endl;
                    }
                }
                else if (op_name == "DOWN") {
                    int step = op_node->get_child_vec()[0]->value.intval;
                    for (int i = 0; i < step; i++) {
                        int x = data.robot.get_pos_x();
                        int y = data.robot.get_pos_y();
                        std::string coordinates = std::to_string(x) + "," + std::to_string(y+1) + "\n";
                        if (y >= data.actual_map.get_side() - 1) {
                            return 0;
                        }
                        if (data.actual_map.get_cell(x, y+1) == cellval::EXIT) {
                            data.robot.move_down(1);
                            path << coordinates;
                            win = true;
                            std::cout << "WIN!!!!!!" << std::endl;
                            return 0;
                        }
                        else if (data.actual_map.get_cell(x, y+1) == cellval::WALL) {
                            data.robot.move_down(1);
                            path << coordinates;
                            data.robot.destroy();
                            std::cout << "..............DESTRUCTION.............." << std::endl;
                            return 0;
                        }
                        else {
                            data.robot.move_down(1);
                            path << coordinates;
                            data.robot.define_explored_map(data.actual_map);
                        }
                        std::cout << "coordinates: " << data.robot.get_pos_x() << " " << data.robot.get_pos_y() << std::endl;
                    }
                }
                else if (op_name == "LOOK") {
                    int x = execute(op_node->get_child_vec()[0]).intval;
                    int y = execute(op_node->get_child_vec()[1]).intval;
                    if ((x < 0) || (y < 0) || (x > data.actual_map.get_side() - 1) || (y > data.actual_map.get_side() - 1)) {
                        return 0;
                    }
                    std::cout << int(data.actual_map.get_cell(x,y)) << std::endl;
                    if ((data.actual_map.get_cell(x,y) != cellval::WALL) &&
                        (!visited[x][y])) {
                            std::cout << "LOOK " << x << " " << y << std::endl;
                            return 1;
                        }
                    else {
                            return 0;
                        }
                }
                else if (op_name == "SETUSED") {
                    if (!visited.size()) {
                        for (int i = 0; i < data.actual_map.get_side(); i++) {
                            visited.push_back(std::vector<bool>(data.actual_map.get_side(), false));
                        }
                    }
                    int x = execute(op_node->get_child_vec()[0]).intval;
                    int y = execute(op_node->get_child_vec()[1]).intval;
                    visited[x][y] = true;
                    std::cout << "Use:" << x << " " << y << std::endl;
                }
                else if (op_name == "GETDRONSCOUNT") {
                    std::string variable_name = *(op_node->get_child_vec()[0]->value.strval);
                    std::string field_view;
                    if (data.func_table[data.this_func].count(variable_name)) {
                        field_view = data.this_func;
                    } else {
                        field_view = "$global$";
                    }
                    InodeType* value_node = new ValNode(data.robot.get_drons_count(), int(type_t::INTVAL));
                    std::cout << *(data.func_table[field_view][variable_name])  << std::endl;
                    data.func_table[field_view][variable_name]->values[0] = value_node;
                    std::cout << field_view << "::" << *(data.func_table[field_view][variable_name]) << std::endl;
                }
                else if (op_name == "SENDDRONS") {
                    //sent drons count
                    int drons = op_node->get_child_vec()[0]->value.intval;
                    if (data.robot.get_drons_count() < drons) {
                        return 0;
                    }
                    //intelligence data
                    std::string var_name = "idata";
                    int type = int(type_t::INTVAL);
                    bool isconst = false;
                    int dims_num = 2;
                    std::vector<InodeType*> dim_vec;
                    std::vector<InodeType*> vals_vec;
                    for (int i = 0 ; i < 2; i++) {
                        InodeType* dim_node = new ValNode(7, int(type_t::INTVAL));
                        dim_vec.push_back(dim_node);
                    }
                    for (int i = 0; i < 7*7; i++) {
                        InodeType* value_node = new ValNode(0, int(type_t::INTVAL));
                        vals_vec.push_back(value_node);
                    }
                    data.func_table[data.this_func][var_name] = new variable(var_name, type, 
                    isconst, dims_num, dim_vec, vals_vec);
                    //cycle for drons
                    for (int i = 0; i < drons; i++) {
                        Robot dron = Robot();
                        dron.set_side(7);
                        
                        
                        int dist = rand() % 3 + 1;
                        int r_x = data.robot.get_pos_x();
                        int r_y = data.robot.get_pos_y();
                        for (int i = 0; i < dist; i++) {
                            int random = rand();
                            if (random % 4 == 1) {
                                //проверка что не ушел
                                if (r_x + dron.get_pos_x() - 1 < 0) {
                                    continue;
                                }                            
                                std::cout << r_x + dron.get_pos_x() << "l" << r_y + dron.get_pos_y() << std::endl;
                                if (data.actual_map.get_cell(r_x + dron.get_pos_x() - 1, r_y + dron.get_pos_y()) == cellval::EXIT) {
                                    dron.move_left(1);
                                    win = true;
                                    return 0;
                                }
                                else if (data.actual_map.get_cell(r_x + dron.get_pos_x() - 1, r_y + dron.get_pos_y()) != cellval::WALL) {
                                    dron.move_left(1);
                                }
                                
                            }
                            else if (random % 4 == 3) {
                                if (r_x + dron.get_pos_x() >= data.actual_map.get_side()-1) {
                                    continue;
                                }
                                std::cout << r_x + dron.get_pos_x() << "r" << r_y + dron.get_pos_y() << std::endl;
                                if (data.actual_map.get_cell(r_x + dron.get_pos_x() + 1, r_y + dron.get_pos_y()) == cellval::EXIT) {
                                    dron.move_right(1);
                                    win = true;
                                    return 0;
                                }
                                else if (data.actual_map.get_cell(r_x + dron.get_pos_x() + 1, r_y + dron.get_pos_y()) != cellval::WALL) {
                                    dron.move_right(1);
                                }
                            }
                            else if (random % 4 == 2) {
                                if (r_y + dron.get_pos_y() - 1 < 0) {
                                    continue;
                                }
                                std::cout << r_x + dron.get_pos_x() << "u" << r_y + dron.get_pos_y() << std::endl;
                                if (data.actual_map.get_cell(r_x + dron.get_pos_x(), r_y + dron.get_pos_y() - 1) == cellval::EXIT) {
                                    dron.move_up(1);
                                    win = true;
                                    return 0;
                                }
                                else if (data.actual_map.get_cell(r_x + dron.get_pos_x(), r_y + dron.get_pos_y() - 1) != cellval::WALL) {
                                    dron.move_up(1);
                                }
                                
                            }
                            else {
                                if (r_y + dron.get_pos_y() >= data.actual_map.get_side()-1) {
                                    continue;
                                }
                                std::cout << r_x + dron.get_pos_x() << "d" << r_y + dron.get_pos_y() << std::endl;
                                if (data.actual_map.get_cell(r_x + dron.get_pos_x(), r_y + dron.get_pos_y() + 1) == cellval::EXIT) {
                                    dron.move_down(1);
                                    win = true;
                                    return 0;
                                }
                                else if (data.actual_map.get_cell(r_x + dron.get_pos_x(), r_y + dron.get_pos_y() + 1) != cellval::WALL) {
                                    dron.move_down(1);
                                }
                            }
                            std::cout << "ololo" << (dron.get_pos_y()+3)*7+(dron.get_pos_x()+3) << std::endl;
                            data.func_table[data.this_func][var_name]->values[(dron.get_pos_y()+3)*7+(dron.get_pos_x()+3)]->value.intval = 1;
                        }
                        data.robot.use_drons();
                        std::cout << data.this_func << "::" << *(data.func_table[data.this_func][var_name]) << std::endl;
                    }

                }
                else if (op_name == "Func: main") { //+
                    std::cout << "MAIN" << std::endl;
                    for (auto child: op_node->get_child_vec()) {
                        OperNode* oper_node = dynamic_cast<OperNode*>(child);
                        std::cout<<oper_node->get_operation_name() << std::endl;
                        execute(child);
                    }
                }
                else { //other functions //+
                    for (auto child: op_node->get_child_vec()) {
                        OperNode* oper_node = dynamic_cast<OperNode*>(child);
                        std::cout<<oper_node->get_operation_name() << std::endl;
                        execute(child);
                    }
                }
        
    }
    return 0;
}
