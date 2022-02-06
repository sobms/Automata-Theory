#ifndef STREENODES_H_
#define STREENODES_H_
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <stdarg.h>
#include <fstream>
enum class type_t {
    INTVAL = 100,
    BOOLVAL,
    STRVAL,
    TYPEVAL,
    CELLVAL,
    NODE_PTR
};



enum class cellval {
     EMPTY = 200,
     WALL,
     EXIT,
     UNDEF
};


class OperNode;
class InodeType;

union YYSTYPE {
    int intval;
    bool boolval;
    std::string* strval;
    std::vector<InodeType*>* vec_nodes;
    InodeType* node_ptr;
    YYSTYPE() { node_ptr = nullptr; };
    YYSTYPE(int val) : intval(val) {}
    YYSTYPE(bool val) : boolval(val) {}
    YYSTYPE(std::string* str) : strval(str) {}
    YYSTYPE(InodeType* ptr): node_ptr(ptr) {}
    ~YYSTYPE() {};
};



class InodeType {
public:
    YYSTYPE value;
    virtual ~InodeType() {};
    virtual std::ostream& print(std::ostream& out) {
        out << value.intval;
        return out;
    };
    friend std::ostream& operator << (std::ostream& out, InodeType* node) {
        return node->print(out);
    }
    virtual InodeType* copy() const {
			return new InodeType(*this);
	}
};

class ValNode : public InodeType {
private:
    int tp;
public:
    ValNode() : InodeType() {}
    ValNode(YYSTYPE val, int type) : InodeType() {
        if (type == int(type_t::INTVAL)) {
            value.intval = val.intval;
            tp = type;
        }
        if (type == int(type_t::BOOLVAL)) {
            value.boolval = val.boolval;
            tp = type;
        }
        if (type == int(type_t::STRVAL)) {
            value.strval = val.strval;
            tp = type;
        }
        if (type == int(type_t::TYPEVAL)) {
            value.intval = val.intval;
            tp = type;
        }
        if (type == int(type_t::CELLVAL)) {
            value.intval = val.intval;
            tp = type;
        }
        // что если node_ptr
    }
    std::ostream& print(std::ostream& out) {
        if (tp == int(type_t::INTVAL)) {
            out << "Value: " << value.intval << ", type: " << "int" << std::endl;
        }
        if (tp == int(type_t::BOOLVAL)) {
            out << "Value: " << value.boolval << ", type: " << "bool" << std::endl;
        }
        if (tp == int(type_t::STRVAL)) {
            out << "Value: " << *(value.strval) << ", type: " << "string" << std::endl;
        }
        if (tp == int(type_t::TYPEVAL)) {
            out << "Value: " << value.intval << ", type: " << "type" << std::endl;
        }
        if (tp == int(type_t::CELLVAL)) {
            out << "Value: " << value.intval << ", type: " << "cell" << std::endl;
        } 
        return out;
    }
    void set_type(int type) {
        tp = type;
    }
    int get_type() {
        return tp;
    }
    ValNode* copy() const {
			return new ValNode(*this);
	}
    ~ValNode() {};
};

class VarNode : public InodeType {
private:
    std::string name;
    std::string view_field;
public:
    VarNode(std::string var_name, std::string v_field) : InodeType() {
        name = var_name;
        view_field = v_field;
    }
    std::ostream& print(std::ostream& out) {
        out << "Variable: " << name << std::endl; 
        return out;
    }
    std::string get_name() {
        return name;
    }

    std::string get_view_field() {
        return view_field;
    }
    VarNode* copy() const {
			return new VarNode(*this);
	}
    ~VarNode() {};
};

class OperNode : public InodeType {
private:
    std::string operation_name;
    int nchild;
    std::vector<InodeType*> child_vec;
public:
    
    OperNode() {}

    OperNode(std::string oper_name, int num_child, std::vector<InodeType*> vec_child) : InodeType(), 
     operation_name(oper_name), nchild(num_child), child_vec(vec_child) {}

    OperNode(std::string oper_name, int num_child, ...) : InodeType() {
        va_list op;
        operation_name = oper_name;
        nchild = num_child;

        va_start(op, num_child);
        for (int i = 0; i < nchild; i++) {
            child_vec.push_back(va_arg(op, InodeType*));
        }
        va_end(op);
    }
    std::vector<InodeType*>& get_child_vec() {
        return child_vec;
    }
    std::string get_operation_name() {
        return operation_name;
    }
    std::ostream& print(std::ostream& out) {
        out << operation_name << " value: " << value.intval << std::endl;
        for (auto n: child_vec) {
            out << n;
        }
        return out;
    }
    OperNode* copy() const {
			return new OperNode(*this);
	}
    ~OperNode() {};

};

struct variable {
    std::string name;
    type_t type;
    bool is_const;
    int dims_count;
    std::vector<InodeType*> mas_dim;
    std::vector<InodeType*> values;
    variable(std::string name, int type2, bool is_const, int dims_count, std::vector<InodeType*> mas_dim, std::vector<InodeType*> values) :
        name(name), is_const(is_const), dims_count(dims_count), mas_dim(mas_dim), values(values) 
        {
            switch(type2) {
                case int(type_t::INTVAL):
                    type = type_t::INTVAL;
                    break;
                case int(type_t::BOOLVAL):
                    type = type_t::BOOLVAL;
                    break;
                case int(type_t::CELLVAL):
                    type = type_t::CELLVAL;
                    break;
            }
        }
    friend std::ostream& operator << (std::ostream& out, const variable& var) {
        out << "Var: " << var.name << ", type: " << int(var.type) << ", const = " << 
        var.is_const << " Dims: " << var.dims_count << ": ";
        for (int i = 0; i < var.mas_dim.size()-1; i++) {
            out << "dim" << i+1 << " " << var.mas_dim[i]->value.intval << ", ";
        }
        
        out << "dim" << var.mas_dim.size() << " " << var.mas_dim[var.mas_dim.size()-1]->value.intval << " ";
        
        out << "Values: ";
        for (auto val: var.values) {
            std::string ret_val;
            if (var.type == type_t::CELLVAL) {
                
                if (val->value.intval == int(cellval::EMPTY)) {
                    ret_val = "EMPTY";
                }
                else if (val->value.intval == int(cellval::WALL)) {
                    ret_val = "WALL";
                }
                else if (val->value.intval == int(cellval::EXIT)) {
                    ret_val = "EXIT";
                }
                else if (val->value.intval == int(cellval::UNDEF)) {
                    ret_val = "UNDEF";
                }
                out << ret_val << ", ";
            }
            else {
                out << val->value.intval << ", ";
            }
        }
        out << std::endl;
        return out;
    }
};

class Map {
private:
    int side;
    std::vector<std::vector<cellval>> map_field;
public:
    Map(): side(0) {}
    int get_side() {
        return side;
    }
    void set_side(int s) {
        side = s;
    }
    void set_cell(int x, int y, cellval val) {
        map_field[x][y] = val;
    }
    cellval get_cell(int x, int y) {
        return map_field[x][y];
    }
    void create_field(cellval val) {
        for (int i = 0; i < side; i++) {
            map_field.push_back(std::vector<cellval>(side, val));
        }
    }
};

class Robot: public Map {
private:
    int pos_x;
    int pos_y;
    int drons_count;
    bool is_activated;
public:
    Robot(): Map(), pos_x(0), pos_y(0), drons_count(0), is_activated(true) {}
    Robot(int x, int y, int d_count): Map(), pos_x(x), pos_y(y), drons_count(d_count), is_activated(true) {}

    int get_drons_count() {
        return drons_count;
    }
    bool get_state() {
        return is_activated;
    }
    int get_pos_x() {
        return pos_x;
    }
    int get_pos_y() {
        return pos_y;
    }
    void set_pos_x(int x) {
        pos_x = x;
    }
    void set_pos_y(int y) {
        pos_y = y;
    }
    void set_drons_count(int count) {
        drons_count = count;
    }
    int use_drons() {
        return drons_count--;
    }
    void move_left(int dist) {
        if (is_activated) {
            pos_x -= dist;
        }
    }
    void move_right(int dist) {
        if (is_activated) {
            pos_x += dist;
        }
    }
    void move_up(int dist) {
        if (is_activated) {
            pos_y -= dist;
        }
    }
    void move_down(int dist) {
        if (is_activated) {
            pos_y += dist;
        }
    }
    void destroy() {
        is_activated=false;
    }
    void define_explored_map(Map& actual_map) {
        if ((pos_x-1)>0 && (pos_y-1)>0 && (pos_x+1)<actual_map.get_side() && (pos_y+1)<actual_map.get_side()) {
            set_cell(pos_x, pos_y, actual_map.get_cell(pos_x, pos_y));
            set_cell(pos_x-1, pos_y, actual_map.get_cell(pos_x-1, pos_y));
            set_cell(pos_x+1, pos_y, actual_map.get_cell(pos_x+1, pos_y));
            set_cell(pos_x, pos_y-1, actual_map.get_cell(pos_x, pos_y-1));
            set_cell(pos_x, pos_y+1, actual_map.get_cell(pos_x, pos_y+1));
            set_cell(pos_x+1, pos_y+1, actual_map.get_cell(pos_x+1, pos_y+1));
            set_cell(pos_x-1, pos_y-1, actual_map.get_cell(pos_x-1, pos_y-1));
            set_cell(pos_x+1, pos_y-1, actual_map.get_cell(pos_x+1, pos_y-1));
            set_cell(pos_x-1, pos_y+1, actual_map.get_cell(pos_x-1, pos_y+1));
        }
        
    }
    void read_map(std::string filename, Map& map) {
        std::ifstream file;
        file.open("labirint.txt");
        std::string line;
        if (file.is_open()) {
            getline(file, line);
            map.set_side(atoi(line.c_str()));
            getline(file, line);
            set_pos_x(atoi(line.c_str()));
            getline(file, line);
            set_pos_y(atoi(line.c_str()));
            getline(file, line);
            set_drons_count(atoi(line.c_str()));
        }
        // create field
        map.create_field(cellval::EMPTY);
        int l = 0, c = 0;
        while (!file.eof()) {
            char next = file.get();
            if (next != '\n') {
                if (next == 'E') {
                    map.set_cell(c++, l, cellval::EMPTY);
                } else if (next == 'Q') {
                    map.set_cell(c++, l, cellval::EXIT);
                } else if (next == 'W') {
                    map.set_cell(c++, l, cellval::WALL);
                } else if (next == 'U') {
                    map.set_cell(c++ ,l, cellval::UNDEF);
                }
            }
            else {
                c = 0;
                l++;
            }
        }
    }
    friend std::ostream& operator << (std::ostream& out, Robot& robot) {
        out << "...ROBOT..." << std::endl;
        out << "X: " << robot.get_pos_x() << std::endl;
        out << "Y: " << robot.get_pos_y() << std::endl;
        out << "Drons count: " << robot.get_drons_count() << std::endl;
        return out;
    }
};

class Program_data {
public:
    std::string data = "digraph G {\n";
    bool stop_flag = false;
    int id = 0;
    InodeType* root;
    std::string this_func = "$global$";
    std::unordered_map<std::string, std::unordered_map<std::string, variable*>> func_table;
    std::unordered_map<std::string, InodeType*> functions;
    Robot robot;
    Map actual_map;
    
    void robot_init() {
        std::string file_name = "labirint.txt";
        actual_map = Map();
        robot = Robot();
        robot.read_map(file_name, actual_map);
        robot.set_side(actual_map.get_side());
        robot.create_field(cellval::UNDEF);
        robot.define_explored_map(actual_map);
    }
    Program_data() {
        robot_init();
    };
    
    void Output_tree(InodeType* node, int parent_id) {
        if (parent_id >= 0) {
            data += std::to_string(parent_id) + "->" + std::to_string(id) + ";\n";
        } 
        if (typeid(*node).hash_code() == typeid(OperNode).hash_code()) {
            OperNode* op_node = dynamic_cast<OperNode*>(node);
            data += std::to_string(id) + "[label=\"" + op_node->get_operation_name() + "\"];\n";
            int par_id = id++;
            for (auto child_node: op_node->get_child_vec()){
                if (child_node) {
                    Output_tree(child_node, par_id);
                }
            }
        }
        if (typeid(*node).hash_code() == typeid(ValNode).hash_code()) {
            ValNode* val_node = dynamic_cast<ValNode*>(node);
            if (val_node->get_type() == int(type_t::INTVAL))
            {
                data += std::to_string(id) + "[label=\"" + std::to_string(val_node->value.intval) + " type: int\"];\n";
            }
            else if (val_node->get_type() == int(type_t::BOOLVAL)) 
            {
                data += std::to_string(id) + "[label=\"" + std::to_string(val_node->value.intval) + " type: bool\"];\n";
            }
            else if (val_node->get_type() == int(type_t::STRVAL))
            {
                data += std::to_string(id) + "[label=\"" + *(val_node->value.strval) + " type: str\"];\n";
            }
            else if (val_node->get_type() == int(type_t::CELLVAL))
            {
                std::string val;
                switch (val_node->value.intval) {
                    case int(cellval::EMPTY):
                        val = "EMPTY";
                        break;
                    case int(cellval::WALL):
                        val = "WALL";
                        break;
                    case int(cellval::EXIT):
                        val = "EXIT";
                        break;
                    case int(cellval::UNDEF):
                        val = "UNDEF";
                        break;
                }
                data += std::to_string(id) + "[label=\"" + val + " type: cell\"];\n";
            }
            
        }
        if (typeid(*node).hash_code() == typeid(VarNode).hash_code()) {
            VarNode* var_node = dynamic_cast<VarNode*>(node);
            data += std::to_string(id) + "[label=\"" + var_node->get_view_field() + "::" + var_node->get_name() + "\"];\n";
        }
        id++;
        
    }
    
};
#endif