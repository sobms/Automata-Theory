%code requires {
       #include "StreeNodes.h"
       YYSTYPE execute(InodeType* node);
}
%{   
       #include <string>
       #include <stdio.h> 
       #include <ctype.h>
       #include <vector>
       #include <typeinfo>
       #include <algorithm>
       #include <fstream>
       std::string this_func = "$global$";
       extern FILE * yyin;
       int yylex(void);
       
       void yyerror(const char *s); 
       int intToBool(int val) {
              return val != 0;
       }
       
%} 
%locations
%code{
       Program_data pdata = Program_data();
       bool check_dimensions(variable* var1, std::vector<InodeType*> values) {
              int n_vals = 1;
              for (auto dim: var1->mas_dim) {
                     n_vals *= dim->value.intval;
              }
              return n_vals == values.size();
       }
       //получить произведение оставшихся размерностей
       int get_dim_factor(int idx, std::vector<InodeType*> vec_dims) {
              int factor = 1;
              for (int i = idx+1; i < vec_dims.size(); i++) {
                     factor*=vec_dims[i]->value.intval;    
              }
              return factor;
       }

       InodeType* find_main(std::vector<InodeType*> nodes_vec) {
              for (auto node: nodes_vec) {
                     OperNode* op_node = dynamic_cast<OperNode*>(node);
                     std::string name = op_node->get_operation_name();
                     std::cout << name.substr(6, name.size()-6) << std::endl;
                     if ((name.size()>5) && (name.substr(6, name.size()-6)=="main")) {
                            return node;
                     }
              }
              return nullptr;
       }

       InodeType* variable_declaration(std::string* name, InodeType* isconst, 
                                                 InodeType* type, InodeType* dims, InodeType* vals, std::vector<int> locs) {
              bool error_state = false;
              InodeType* var_declaration_node = nullptr;
              OperNode* values = nullptr;
              OperNode* dimensions = nullptr;
              if (vals) {
                     values  = dynamic_cast<OperNode*>(vals);
              } 
              if (dims) {
                     dimensions = dynamic_cast<OperNode*>(dims);
              }
              if (pdata.func_table[this_func].count(*(name)) || pdata.func_table["$global$"].count(*(name))) {
                     std::cout << "Error! Redeclaring the variable! (line: " << locs[0] << ")" << std::endl;
                     var_declaration_node = nullptr;
                     error_state = true;
              }
              //count number of values
              int n_vals = 1;
              if (dimensions) {
                     std::vector<InodeType*>::iterator it =  dimensions->get_child_vec().begin();
                     it++; //skip dims count value
                     for (; it != dimensions->get_child_vec().end(); it++){
                            n_vals*= (*it)->value.intval;
                     }
              }
              if (values) {
                     int values_type = values->get_child_vec()[0]->value.intval;
                     //remove from values child vector type value
                     values->get_child_vec().erase(values->get_child_vec().begin());

                     if (values->get_child_vec().size() != n_vals){
                            std::cout << "Error! Dimension and initialization values count mismatch! (line: "
                            << locs[3] << ")" << std::endl;;
                            var_declaration_node = nullptr;
                            error_state = true;
                     }
                     if ((values_type != type->value.intval) && 
                     ((type->value.intval != int(type_t::BOOLVAL)) || (values_type != int(type_t::INTVAL)))) {
                            std::cout << "Error! Values type and declared variable type don't match, no rule to cast types! (line: "
                            << locs[2] << ")" << std::endl;;
                            var_declaration_node = nullptr;
                            error_state = true;
                     } else {
                     //type casting
                            for (auto& val: values->get_child_vec()) {
                                   //int to bool
                                   if (type->value.intval == int(type_t::BOOLVAL)) {
                                          val->value.intval = intToBool(val->value.intval);
                                   }
                                   //bool to int is not necessary
                                   //CELL type cannot cast to other
                            }  
                     }
              }
              if (!error_state) 
              {
                     int const_val = 0; // by default
                     int dims_count = 1; // by default
                     std::vector<InodeType*> dims_vec = std::vector<InodeType*>(); // by default 1 dim = 1
                     std::vector<InodeType*> vals_vec = std::vector<InodeType*>(); //by defalt 1 val = 0
                     
                     if (isconst) {
                            const_val = isconst->value.intval;  
                     }
                     if (dimensions) {
                            dims_count =dimensions->get_child_vec()[0]->value.intval;
                            dimensions->get_child_vec().erase(dimensions->get_child_vec().begin());
                            dims_vec.insert(dims_vec.end(), dimensions->get_child_vec().begin(), dimensions->get_child_vec().end());
                     } else {
                            InodeType* dim = new ValNode(1, int(type_t::INTVAL));
                            dims_vec.push_back(dim);
                     }
                     if (values) {
                            vals_vec = values->get_child_vec();
                     } else 
                     {
                            int tp;
                            if (type->value.intval == int(type_t::CELLVAL)) {
                                   tp = int(type_t::CELLVAL);
                                   for (int i = 0; i < n_vals; i++) {
                                          InodeType* val = new ValNode(int(cellval::EMPTY), tp);
                                          vals_vec.push_back(val);
                                   }
                            } else {
                                   tp = int(type_t::INTVAL);
                                   for (int i = 0; i < n_vals; i++) {
                                          InodeType* val = new ValNode(0, tp);
                                          vals_vec.push_back(val);
                                   }
                            }                                   
                     }       
                     variable* var = new variable(*(name), type->value.intval, const_val, dims_count, 
                     dims_vec, vals_vec);
                     
                     std::vector<InodeType*> vec;
                     InodeType* var_node = new VarNode(*(name), this_func);
                     vec.push_back(var_node); //necessary
                     vec.push_back(type); //necessary

                     InodeType* isconst_node = new ValNode(const_val, int(type_t::INTVAL));
                     vec.push_back(isconst_node);
                     
                     InodeType* dims_num = new ValNode(dims_count, int(type_t::INTVAL));
                     vec.push_back(dims_num);
                     vec.insert(vec.end(), dims_vec.begin(), dims_vec.end());
                     vec.insert(vec.end(), vals_vec.begin(), vals_vec.end());
                     
                     InodeType* node = new OperNode("VAR", vec.size(), vec);
                     var_declaration_node = node;
                     pdata.func_table[this_func][*(name)] = var;
                     std::cout << this_func << "::" << *pdata.func_table[this_func][*(name)];
              }   
              return var_declaration_node;    
       }
}
%token <intval> INT
%token <boolval> BOOL
%token <intval> CELL
%token <strval> VARIABLE_NAME
%token <strval> NAME

%token VARDECLARATION VAR TYPE_VAL CONST TYPE DIMENSION DIMENSIONS 
%token VALUE VALUES DIM INDEX ASSIGN TO SPACE
%token WHILE CHECK DO SWITCH CONDITION SETUSED LOOK SETSTART
%token LEFT RIGHT UP DOWN SENDDRONS GETDRONSCOUNT FUNC CALL PROGRAM COUNT

%left OR AND 
%left EQ
%left NOT 
%left MAX MIN
%left ADD SUB
%left MUL DIV 
%nonassoc UMINUS


%type <vec_nodes> expr_list
%type <node_ptr> expr
%type <vec_nodes> values_list
%type <node_ptr> values
%type <node_ptr> type
%type <node_ptr> const
%type <vec_nodes> dimensions_list
%type <node_ptr> dimensions
%type <node_ptr> var_decl
%type <vec_nodes> var_list
%type <node_ptr> var_decl_block
%type <node_ptr> addressing_block
%type <vec_nodes> indexing
%type <vec_nodes> addressing_list
%type <node_ptr> assignment
%type <node_ptr> stmt
%type <vec_nodes> stmts
%type <node_ptr> func
%type <vec_nodes> content_list
%type <node_ptr> call
%type <node_ptr> cycle
%type <node_ptr> condition
%type <vec_nodes> conditions_list
%type <node_ptr> switch

%% 
script_body:
              '<' PROGRAM '>' content_list '<' '/' PROGRAM '>' 
              { 
                     if (($4) && (!pdata.stop_flag)){
                            if (!find_main(*($4))) {
                                   std::cout << "Error! Function 'main' does not exist!" << std::endl;
                            }
                            else {
                                   
                                   InodeType* root = new OperNode("PROGRAM", $4->size(), *($4));
                                   pdata.Output_tree(root, -1);
                                   std::ofstream tree;
                                   tree.open ("output.txt");
                                   tree << pdata.data;
                                   tree.close();
                                   execute(root);
                            }   
                     } 
              }
              | '<' PROGRAM error PROGRAM '>' 
              {
                     std::cout << "Syntax error in line: " << @3.first_line << std::endl;
              }
              ;
func:
              func_decl stmts '<' '/' FUNC '>' 
              {
                     if ($2) {
                            InodeType* node = new OperNode("Func: " + this_func, $2->size(), *($2));
                            $$ = node;
                            //return to the global view
                            this_func = "$global$";
                     } else {
                            std::cout << "Empty function\n" << std::endl;
                            $$ = nullptr;
                     }
                     
              }
              | func_decl error '>' 
              {
                     std::cout << "Syntax error in function block, line: " << @2.last_line << std::endl;
                     $$ = nullptr;
                     pdata.stop_flag = true;
              }
              ;
func_decl:
              '<' FUNC SPACE NAME SPACE '=' SPACE VARIABLE_NAME '>' 
              {
                     if (pdata.func_table.count(*($8))==0) {
                            std::cout << "func: " << *($8) << std::endl;   
                            this_func = *($8);
                            pdata.func_table[*($8)] = std::unordered_map<std::string, variable*>();
                     } else {
                            std::cout << "Error! Function redeclaration! (line: " 
                            << @8.first_line << ")" << std::endl;
                            pdata.stop_flag = true;
                     }
              }
              | '<' FUNC error '>' 
              {
                     std::cout << "Syntax error in function declaration block, line: " << @3.first_line << std::endl;
                     pdata.stop_flag = true;
              }
              ;
content_list:
              content_list var_decl_block 
              {
                     if (($$) && ($2)){
                            $$->push_back($2);
                     } 
              }
              | content_list func 
              {
                     if (($$) && ($2)) {
                            $$->push_back($2);   
                     }
              }
              | func 
              {
                     if ($1) {
                            $$ = new std::vector<InodeType*>();
                            $$->push_back($1);
                     } else {
                            $$ = nullptr;
                     }
              }
              | var_decl_block 
              {
                     if ($1) {
                            $$ = new std::vector<InodeType*>();
                            $$->push_back($1);
                     } else {
                            $$ = nullptr;
                     }
              }

              | content_list '<' SETSTART '>' addressing_block SPACE addressing_block '<' '/' SETSTART '>' 
              {
                     if ($$) {
                            InodeType* set_st_node = new OperNode("SETSTART", 2, $5, $7);
                            $$->push_back(set_st_node);
                     } 
                     
              }
              | '<' SETSTART '>' addressing_block SPACE addressing_block '<' '/' SETSTART '>'
              {
                     $$ = new std::vector<InodeType*>();
                     InodeType* set_st_node = new OperNode("SETSTART", 2, $4, $6);
                     $$->push_back(set_st_node);
              } 
              | error func
              {
                     std::cout << "Syntax error in content block, line: " << @1.last_line << std::endl;
                     $$ = nullptr;
                     pdata.stop_flag = true;
              }
              | error var_decl_block
              {
                     std::cout << "Syntax error in content block, line: " << @1.last_line<< std::endl;
                     $$ = nullptr;
                     pdata.stop_flag = true;
              }
              ;
stmts:
       stmts stmt 
       {
              if (($$) && ($2)) 
              {
                     $$->push_back($2);
              } 
              else if ($2) 
              {
                     $$ = new std::vector<InodeType*>();
                     $$->push_back($2);
              }
       }
       | stmt 
       {
              if ($1) {
                     $$ = new std::vector<InodeType*>();
                     $$->push_back($1);
              }else {
                     $$ = nullptr;
              }
       }
       | error stmt
       {
              std::cout << "Syntax error in statement block, line: " << @1.last_line << std::endl;
              $$ = nullptr;
              pdata.stop_flag = true;
       }
       ;
stmt:
       var_decl_block 
       {
              $$ = $1;
       }
       | assignment 
       {
              $$ = $1;
       }
       | call
       {
              $$ = $1;
       }
       | cycle 
       {
              $$ = $1;
       }
       | switch 
       {
              $$ = $1;
       }
       | '<' LEFT '>' expr '<' '/' LEFT '>'
       {
              $$ = new OperNode("LEFT", 1, $4);
       }
       | '<' RIGHT '>' expr '<' '/' RIGHT '>'
       {
              $$ = new OperNode("RIGHT", 1, $4);
       }
       | '<' UP '>' expr '<' '/' UP '>'
       {
              $$ = new OperNode("UP", 1, $4);
       }
       | '<' DOWN '>' expr '<' '/' DOWN '>'
       {
              $$ = new OperNode("DOWN", 1, $4);
       }
       | '<' SETUSED '>' expr SPACE expr '<' '/' SETUSED '>'
       {
              $$ = new OperNode("SETUSED", 2, $4, $6);
       }
       | '<' GETDRONSCOUNT '>' VARIABLE_NAME '<' '/' GETDRONSCOUNT '>' 
       {
              InodeType* v_node = new ValNode($<strval>4, int(type_t::STRVAL));
              $$ = new OperNode("GETDRONSCOUNT", 1, v_node);
       }
       | '<' SENDDRONS '>' expr '<' '/' SENDDRONS '>' 
       {
              $$ = new OperNode("SENDDRONS", 1, $4);
       }
       ;

call:
       '<' CALL '>' VARIABLE_NAME '<' '/' CALL '>' 
       {
              if (pdata.func_table.count(*($4))==0){
                     std::cout << "Error! Addressing to function, which is not declared before! (line: " 
                     << @4.first_line << ")" << std::endl;
                     $$ = nullptr;
              }
              else {
                     InodeType* name_node = new ValNode($4, int(type_t::STRVAL));
                     InodeType* call_node = new OperNode("CALL", 1, name_node);
                     $$ = call_node; 
              }
       }
       ;
cycle:
       '<' WHILE '>' '<' CHECK '>' expr '<' '/' CHECK '>' '<' DO '>' stmts '<' '/' DO '>' '<' '/' WHILE '>'
       {
              if ($15) {
                     std::vector<InodeType*> child_vec;
                     child_vec.push_back($7);
                     child_vec.insert(child_vec.end(), $15->begin(), $15->end());
                     InodeType* cycle_node = new OperNode("CYCLE", child_vec.size(), child_vec);
                     $$ = cycle_node;
              } else {
                     $$ = nullptr;
              }
       }
       | '<' WHILE error '/' WHILE '>'
       {
              std::cout << "Syntax error in cycle block, line: " << @3.first_line << std::endl;
              $$ = nullptr;
              pdata.stop_flag = true;
       }
       ;
switch:
       '<' SWITCH '>' conditions_list '<' '/' SWITCH '>'
       {
              if ($4) {
                     InodeType* switch_node = new OperNode("SWITCH", $4->size(), *($4));
                     $$ = switch_node;
              } else {
                     $$ = nullptr;
              }
       }
       | '<' SWITCH error SWITCH '>' 
       {
              std::cout << "Syntax error in switch block, line: " << @3.first_line << std::endl;
              $$ = nullptr;
              pdata.stop_flag = true;
       }
       ;
conditions_list:
       conditions_list condition
       {
              if (($$) && ($2)) 
              {
                     $$->push_back($2);
              } 
              else if ($2) 
              {
                     $$ = new std::vector<InodeType*>();
                     $$->push_back($2);
              }
       }
       | condition
       {
              if ($1) {
                     $$ = new std::vector<InodeType*>();
                     $$->push_back($1);
              }else {
                     $$ = nullptr;
              }
       }
       ;
condition:
       '<' CONDITION '>' '<' CHECK '>' expr '<' '/' CHECK '>' 
       '<' DO '>' stmts '<' '/' DO '>' '<' '/' CONDITION '>' 
       {
              if ($15) {
                     std::vector<InodeType*> child_vec;
                     child_vec.push_back($7);
                     child_vec.insert(child_vec.end(), $15->begin(), $15->end());
                     InodeType* condition_node = new OperNode("CONDITION", child_vec.size(), child_vec);
                     $$ = condition_node;
              } else {
                     $$ = nullptr;
              }
       }
       | '<' CONDITION error CONDITION '>'
       {
              std::cout << "Syntax error in condition block, line: " << @3.first_line << std::endl;
              $$ = nullptr;
              pdata.stop_flag = true;
       }
       ;
addressing_block:
       '<' VAR SPACE NAME SPACE '=' SPACE VARIABLE_NAME  '>' '<' '/' VAR '>' 
       {
              if ($8) {
                     std::string* name = $8;
                     std::string field_view = "";
                     if (pdata.func_table[this_func].count(*name) > 0) {
                            field_view = this_func;
                     } else if (pdata.func_table["$global$"].count(*name) > 0) {
                            field_view = "$global$";
                     } else {
                            std::cout << "Error! There is no such variable in this field of view! (line: " 
                            << @8.first_line << ")" << std::endl;
                            $$ = nullptr;
                     }
                     if (field_view.size() > 0) {
                            std::vector<InodeType*> vec = std::vector<InodeType*>();
                            InodeType* name_n = new ValNode(name, int(type_t::STRVAL));
                            
                            vec.push_back(name_n);
                            
                            InodeType* node = new OperNode("ADDRESSING", vec.size(), vec);
                            $$ = node;
                            //std::cout << (*pdata.func_table[field_view][*($8)]) << std::endl;
                     }
              } else {
                     $$ = nullptr;
                     delete $8;
              }
       } 
       | '<' VAR SPACE NAME SPACE '=' SPACE VARIABLE_NAME  '>' '<' DIM '>' indexing '<' '/' DIM '>' '<' '/' VAR '>'
       { 
              
              if ($13 && $8) {
                     std::string* name = $8;
                     std::string field_view = "";
                     if (pdata.func_table[this_func].count(*name) > 0) {
                            //if in this_func field of view
                            field_view = this_func;
                     } else if (pdata.func_table["$global$"].count(*name)>0) {
                            //if in global field of view
                            field_view = "$global$";
                     } else {
                            std::cout << "Error! There is no such variable in this field of view! (line: " 
                            << @8.first_line << ")" << std::endl;
                            $$ = nullptr;
                     }
                     
                     if (field_view.size() > 0) {
                            bool error_state = false;
                            if ($13->size() != pdata.func_table[field_view][*name]->dims_count) {
                                   std::cout << "Error! Incorrect dimensionality! (line: " 
                                   << @13.first_line << ")" << std::endl;
                                   error_state = true;
                            } else {
                                   for (int dim = 0; dim < $13->size(); dim++) {
                                          if ($13->at(dim)->value.intval >= pdata.func_table[field_view][*name]->mas_dim[dim]->value.intval) {
                                                 std::cout << "Error! Index out of range! (line: " 
                                                 << @13.first_line << ")" << std::endl;
                                                 error_state = true;
                                          }
                                   }
                            }
                            
                            if (!error_state) { 
                                   std::vector<InodeType*> vec;
                                   InodeType* name_node = new ValNode(name, int(type_t::STRVAL));
                                   vec.push_back(name_node);
                                   vec.insert(vec.end(), $13->begin(), $13->end());
                                   InodeType* node = new OperNode("ADDRESSING", vec.size(), vec);
                                   $$ = node;
                            } 
                            else {
                                   $$ = nullptr;
                                   delete $13;
                            }     
                     }    
              } else {
                     
                     $$ = nullptr;
                     delete $13;
                     delete $8;
              }
       }
       | '<' VAR SPACE NAME error VAR '>'
       {
              std::cout << "Syntax error in addressing block, line: " << @4.first_line << std::endl;
              $$ = nullptr;
              pdata.stop_flag = true;
       }
       ;

indexing:
       '<' INDEX '>' expr '<' '/' INDEX '>' 
       {
              if ($4) {
                     $$ = new std::vector<InodeType*>();
                     $$->push_back($4);
              }
       }
       | indexing '<' INDEX '>' expr '<' '/' INDEX '>' 
       {
              if ($5 && $$) {
                     $$->push_back($5);
              } else {
                     $$ = nullptr;
              }
       }
       | '<' INDEX error INDEX '>'
       {
              std::cout << "Syntax error in indexing block, line: " << @3.first_line << std::endl;
              $$ = nullptr;
              pdata.stop_flag = true;
       }
       ;

addressing_list:
       addressing_block 
       {
              if ($1){
                     $$ = new std::vector<InodeType*>();
                     $$->push_back($1);
              }
              else {
                     $$ = nullptr;
              }
              
       }
       | addressing_list  addressing_block 
       {
              if ($$ && $2){
                 $$->push_back($2);    
              }  else if ($2) {
                     $$ = new std::vector<InodeType*>();
                     $$->push_back($2);
              }
       }
       ;

assignment:
       '<' ASSIGN '>' values_list '<' TO '>' addressing_list '<' '/' TO '>' '<' '/' ASSIGN '>' 
       {
              if ($4 && $8) {
                     bool error_state;
                     $4->erase($4->begin());
                     std::vector<InodeType*> values = *($4);
                     std::vector<InodeType*> addressing_list = *($8);

                     std::vector<InodeType*> ch_vec;
                     ch_vec.insert(ch_vec.begin(), addressing_list.begin(), addressing_list.end());

                     for (auto addressing_block: addressing_list) {
                            error_state = false;
                            OperNode* addr_block = dynamic_cast<OperNode*>(addressing_block);
                            std::string field_view;
                            if (pdata.func_table[this_func].count(*(addr_block->get_child_vec()[0]->value.strval)) > 0) {
                                   //if in this_func field of view
                                   field_view = this_func;
                            } else if (pdata.func_table["$global$"].count(*(addr_block->get_child_vec()[0]->value.strval))>0) {
                                   //if in global field of view
                                   field_view = "$global$";
                            }
                            if (addr_block->get_child_vec().size() == 1) { //assignment to whole variable
                                   if (!check_dimensions(pdata.func_table[field_view][*(addr_block->get_child_vec()[0]->value.strval)], values)) {
                                          std::cout << "Error! Number of dimensions don't match for value and variable! (line: " 
                                          << @4.first_line << ")" << std::endl;
                                          error_state = true;
                                   }      
                            } 
                            else if (addr_block->get_child_vec().size() > 1) { //assignment to element of array
                                   if (values.size()!=1) {
                                          std::cout << "Error! Incorrect number of values! (line: " 
                                          << @4.first_line << ")" << std::endl;
                                          error_state = true;
                                   }
                            }
                            if (error_state) {
                                   delete $4;
                                   delete $8;  
                                   $$ = nullptr;
                                   break;
                            }  
                     }
                     addressing_list.clear();
                     if (!error_state) {
                            ////
                            InodeType* var_node = new OperNode("VARIABLES", ch_vec.size(),ch_vec);
                            ch_vec.clear();
                            ch_vec.insert(ch_vec.end(), values.begin(), values.end());
                            InodeType* val_node = new OperNode("VALUES", ch_vec.size(),ch_vec);
                            ////
                            //ch_vec.insert(ch_vec.end(), values.begin(), values.end());
                            InodeType* node = new OperNode("ASSIGNMENT", 2, var_node, val_node);
                            $$ = node;
                     }
              } else{
                     $$ = nullptr;
              }
       }
       | '<' ASSIGN error ASSIGN '>' 
       {
              std::cout << "Syntax error in assign block, line: " << @3.first_line << std::endl;
              $$ = nullptr;
              pdata.stop_flag = true;
       }
       ;

var_decl_block:
       '<' VARDECLARATION '>' var_list '<' '/' VARDECLARATION '>' 
       {
              if ($4) {
                     InodeType* var_decl_node = new OperNode("VARDECLARATION", $4->size(), *$4);
                     $$ = var_decl_node;
              } else {
                     $$ = nullptr;
              }
              
       }
       ; 

var_list:
       var_list var_decl 
       {
              if (($$) && ($2))
              {
                 $$->push_back($2);    
              } else if ($2) {
                     $$ = new std::vector<InodeType*>();
                     $$->push_back($2);
              }
       }
       | var_decl 
       {
              if ($1) 
              {
                     $$ = new std::vector<InodeType*>();
                     $$->push_back($1);
              } else 
              {
                     $$ = nullptr;
              }
              
       }
       ;

var_decl:
       '<' VAR SPACE '=' SPACE VARIABLE_NAME '>' type '<' '/' VAR '>' 
       {
              std::vector<int> locs = {@6.first_line, -1, @8.first_line, -1, -1 };
              $$ = variable_declaration($6, nullptr, $8, nullptr, nullptr, locs);
       }
       | '<' VAR SPACE '=' SPACE VARIABLE_NAME const '>' type '<' '/' VAR '>' 
       {
              std::vector<int> locs = {@6.first_line, @7.first_line, @9.first_line, -1, -1 };
              $$ = variable_declaration($6, $7, $9, nullptr, nullptr, locs);
       }
       | '<' VAR SPACE '=' SPACE VARIABLE_NAME '>' type dimensions values '<' '/' VAR '>' 
       {
              std::vector<int> locs = {@6.first_line, -1, @8.first_line, @9.first_line, @10.first_line };
              $$ = variable_declaration($6, nullptr, $8, $9, $10, locs);
       }
       | '<' VAR SPACE '=' SPACE VARIABLE_NAME '>' type dimensions '<' '/' VAR '>' 
       {
              std::vector<int> locs = {@6.first_line, -1, @8.first_line, @9.first_line, -1 };
              $$ = variable_declaration($6, nullptr, $8, $9, nullptr, locs);
       }
       | '<' VAR SPACE '=' SPACE VARIABLE_NAME const '>' type dimensions '<' '/' VAR '>' 
       {
              std::vector<int> locs = {@6.first_line, @7.first_line, @9.first_line, @10.first_line, -1 };
              $$ = variable_declaration($6, $7, $9, $10, nullptr, locs);
       }
       | '<' VAR SPACE '=' SPACE VARIABLE_NAME const '>' type dimensions values '<' '/' VAR '>' 
       {
              std::vector<int> locs = {@6.first_line, @7.first_line, @9.first_line, @10.first_line, @11.first_line};
              $$ = variable_declaration($6, $7, $9, $10, $11, locs);
       } 
       | '<' VAR SPACE '=' error VAR '>' 
       {
              std::cout << "Syntax error in variable declaration block, line: " << @5.first_line << std::endl;
              $$ = nullptr;
              pdata.stop_flag = true;
       }
       ;
const:
       SPACE CONST SPACE '=' SPACE BOOL 
       {
              int val = $<boolval>6;
              InodeType* const_node = new ValNode(val, int(type_t::INTVAL));
              $$ = const_node;
       }
       ;

type:
       '<' TYPE '>' TYPE_VAL '<' '/' TYPE '>' 
       {
              InodeType* type_node = new ValNode($<intval>4, int(type_t::INTVAL));
              $$ = type_node;
       }
       ;

dimensions:
       '<' DIMENSIONS SPACE COUNT SPACE '=' SPACE expr '>' dimensions_list '<' '/' DIMENSIONS '>'
       {
              if (($8 && $10) || ($8->value.intval != $10->size())) {
                     std::vector<InodeType*> vec_child;
                     vec_child.push_back($8);
                     vec_child.insert(vec_child.end(), $10->begin(), $10->end());
                     InodeType* node = new OperNode("DIMENSIONS", vec_child.size(), vec_child);
                     $$ = node;
              } else {
                     std::cout << "Incorrect dimensions declaration!(Incorrect number of dimensions!)" << std::endl;
                     $$ = nullptr;
              }
       }
       | '<' DIMENSIONS error DIMENSIONS '>' 
       {
              std::cout << "Syntax error in dimensions block, line: " << @3.first_line << std::endl;
              $$ = nullptr;
              pdata.stop_flag = true;
       }
       ;

dimensions_list:
       dimensions_list '<' DIMENSION '>' expr '<' '/' DIMENSION '>' 
       {
              if ($$) {
                     $$->push_back($5);
              }
       }
       | '<' DIMENSION '>' expr '<' '/' DIMENSION '>' 
       {
              if ($$) {
                     $$ = new std::vector<InodeType*>();
                     $$->push_back($4);
              } else {
                     $$ = nullptr;
              }
              
              
       }
       | '<' DIMENSION error DIMENSION '>' 
       {
              std::cout << "Syntax error in dimension block, line: " << @3.first_line << std::endl;
              $$ = nullptr;
              pdata.stop_flag = true;
       }
       ;

values:
       '<' VALUES '>' values_list '<' '/' VALUES '>'
       {
              std::vector<InodeType*> vec_child;
              if ($4) {
                     InodeType* node = new OperNode("VALUES", $4->size(), *($4));
                     $$ = node;
              } else {
                     std::cout << "Incorrect values declaration!" << std::endl;
                     $$ = nullptr;
              }
       }
       | '<' VALUES error VALUES '>' 
       {
              std::cout << "Syntax error in values block, line: " << @3.first_line << std::endl;
              $$ = nullptr;
              pdata.stop_flag = true;
       }
       ;

values_list:
       values_list '<' VALUE '>' expr '<' '/' VALUE '>' 
       {

              if ($$) {                    
                     if (int(type_t::INTVAL) == $$->at(0)->value.intval) {
                            $$->push_back($5);
                     } else {
                            std::cout << "Error! Cannot cast types! (line: " 
                            << @5.first_line << ")" << std::endl; 
                            delete $$;
                            $$ = nullptr;
                     }
              }
       }
       | '<' VALUE '>' expr '<' '/' VALUE '>' 
       {
              $$ = new std::vector<InodeType*>();
              InodeType* type_val = new InodeType();
              type_val->value.intval = int(type_t::INTVAL);
              $$->push_back(type_val);
              $$->push_back($4);
       }
       | '<' VALUE '>' CELL '<' '/' VALUE '>'
       {
              $$ = new std::vector<InodeType*>();
              InodeType* type_val = new InodeType();
              type_val->value.intval = int(type_t::CELLVAL);
              $$->push_back(type_val);
              ValNode* v_node = new ValNode($<intval>4, int(type_t::CELLVAL));
              $$->push_back(v_node);   
       }
       | values_list '<' VALUE '>' CELL '<' '/' VALUE '>' 
       {
              if ($$) {
                     if (int(type_t::CELLVAL) == $$->at(0)->value.intval) {
                            ValNode* v_node = new ValNode($<intval>5, int(type_t::CELLVAL));
                            $$->push_back(v_node);
                     } else {
                            std::cout << "Error! Cannot cast types! (line: " 
                            << @5.first_line << ")" << std::endl; 
                            delete $$;
                            $$ = nullptr;
                     }
              }
       }
       ;

expr:
       BOOL 
       {
              //casting bool to int
              ValNode* node= new ValNode($<boolval>1, int(type_t::INTVAL));
              node->set_type(int(type_t::BOOLVAL));
              InodeType* v_node = node;
              $$ = v_node;
       }
       | INT
       {
              InodeType* v_node = new ValNode($<intval>1, int(type_t::INTVAL));
              $$ = v_node;
       }
       | addressing_block
       {  
              if ($1) {
                     OperNode* addr_node = dynamic_cast<OperNode*>($1);
                     std::string var_name = *(addr_node->get_child_vec()[0]->value.strval);

                     std::string field_view;
                     if (pdata.func_table[this_func].count(var_name)>0) {
                            field_view = this_func;
                     } else 
                     {
                            field_view = "$global$";
                     }
                     if (((addr_node->get_child_vec().size()==1) && //addressing to whole variable
                                   (pdata.func_table[field_view][var_name]->dims_count==1) &&
                                   (pdata.func_table[field_view][var_name]->mas_dim[0]->value.intval==1)) ||
                                   (addr_node->get_child_vec().size()>1)) { //addressing to element of array 
                            $$ = $1;
                     } else {
                            std::cout << "Error! Using arrays in expressions is not allowed. You should specify the indexecutes! (line: " 
                            << @1.first_line << ")" << std::endl;
                            InodeType* node = new ValNode(0, int(type_t::INTVAL));
                            $$ = node;
                     }
              } else {
                     InodeType* node = new ValNode(0, int(type_t::INTVAL));
                     $$ = node;
              }
       }
       |'<' ADD '>' expr_list '<' '/' ADD '>' 
       {
              std::vector<InodeType*> vec_child;
              int sum = 0;
              std::cout << "sum of: ";
              for (auto a: *($4)) {
                     std::cout << a->value.intval << ", ";
                     sum += a->value.intval;
                     vec_child.push_back(a);
              }
              $4->clear();
              delete $4;
              std::cout << "=" << sum << std::endl;
              
              InodeType* op_node = new OperNode("ADD", vec_child.size(), vec_child);
              $$ = op_node;
              $$->value.intval = sum;
       }
       |'<' MUL '>' expr_list '<' '/' MUL '>' 
       {
              std::vector<InodeType*> vec_child;
              int mul = 1;
              
              std::cout << "multiplication of: ";
              for (auto a: *($4)) {
                     std::cout << a->value.intval << ", ";
                     mul *= a->value.intval;
                     vec_child.push_back(a);
              }
              $4->clear();
              delete $4;
              std::cout << "=" << mul << std::endl;

              InodeType* op_node = new OperNode("MUL", vec_child.size(), vec_child);
              $$ = op_node;
              $$->value.intval = mul;
       }
       |'<' SUB '>' expr SPACE expr '<' '/' SUB '>' 
       {
              OperNode* op_node = new OperNode("SUB", 2, $4, $6);
              std::cout << "difference of: " << std::to_string($4->value.intval) << ", " << std::to_string($6->value.intval);
              int sub = $4->value.intval - $6->value.intval;
              std::cout << "=" << std::to_string(sub) << std::endl;
              $$ = op_node;
              $$->value.intval = sub;
       }
       |'<' DIV '>' expr SPACE expr '<' '/' DIV '>' 
       {
              OperNode* op_node = new OperNode("DIV", 2, $4, $6);
              std::cout << "division of: " << std::to_string($4->value.intval) << ", " << std::to_string($6->value.intval);
              int div = $4->value.intval / $6->value.intval;
              std::cout << "=" << std::to_string(div) << std::endl;
              $$ = op_node;
              $$->value.intval = div;
       }
       | '<' OR '>' expr_list '<' '/' OR '>'
       {
              std::vector<InodeType*> vec_child;
              std::cout << "or for: ";
              bool res = 0;
              
              for (auto val: *($4)) {
                     bool valn = intToBool(val->value.intval);
                     res = res || valn;
                     std::cout << valn << ", ";
                     vec_child.push_back(val);
              }
              $4->clear();
              delete $4;
              std::cout << "=" << res << std::endl;

              OperNode* op_node = new OperNode("OR", vec_child.size(), vec_child);
              $$ = op_node;
              $$->value.intval = res;
       }
       | '<' AND '>' expr_list '<' '/' AND '>'
       {
              std::vector<InodeType*> vec_child;
              std::cout << "and for: ";
              bool res = 1;

              for (auto val: *($4)) {
                     bool valn = intToBool(val->value.intval);
                     res = res && valn;
                     std::cout << valn << ", ";
                     vec_child.push_back(val);
              }
              $4->clear();
              delete $4;
              std::cout << "=" << res << std::endl;

              OperNode* op_node = new OperNode("AND", vec_child.size(), vec_child);
              $$ = op_node;
              $$->value.intval = res;
       }
       | '<' NOT '>' expr '<' '/' NOT '>'
       {
              std::cout << "Not " << intToBool($4->value.intval) << " is " << !intToBool($4->value.intval) << std::endl;
              OperNode* op_node = new OperNode("NOT", 1, $4);
              $$ = op_node;
              $$->value.intval = !intToBool($4->value.intval);
       }
       | '<' EQ '>' expr_list '<' '/' EQ '>'
       {
              std::vector<InodeType*> vec_child;
              std::cout << "equal of: ";
              bool res = 1;
              
              for (auto val: *($4)) {
                     res = res*($4->at(0)->value.intval == val->value.intval);
                     std::cout << val->value.intval << ", ";
                     vec_child.push_back(val);
              }
              std::cout << "=" << res << std::endl;
              $4->clear();
              delete $4;


              OperNode* op_node = new OperNode("EQ", vec_child.size(), vec_child);
              $$ = op_node;
              $$->value.intval = res;
       }
       | '<' MAX '>' expr_list '<' '/' MAX '>'
       {
              std::vector<InodeType*> vec_child;
              std::vector<int> arr;

              for (auto val: *($4)) {
                     vec_child.push_back(val);
                     arr.push_back(val->value.intval);
              }
              OperNode* op_node = new OperNode("MAX", vec_child.size(), vec_child);
              $$ = op_node;

              $$->value.intval = *max_element(arr.begin(), arr.end());
              std::cout << "Maximum: " << $$->value.intval << std::endl;
              $4->clear();
              delete $4;
       }
       | '<' MIN '>' expr_list '<' '/' MIN '>'
       {
              std::vector<InodeType*> vec_child;
              std::vector<int> arr;

              for (auto val: *($4)) {
                     vec_child.push_back(val);
                     arr.push_back(val->value.intval);
              }
              OperNode* op_node = new OperNode("MIN", vec_child.size(), vec_child);
              $$ = op_node;

              $$->value.intval = *min_element(arr.begin(), arr.end());
              std::cout << "Minimum: " << $$->value.intval << std::endl;
              $4->clear();
              delete $4;
       }
       | '<' LOOK '>' expr SPACE expr '<' '/' LOOK '>' 
       {
              $$ = new OperNode("LOOK", 2, $4, $6);
       }
       ;

expr_list:
       expr_list SPACE expr 
       {
              $1->push_back($3);
              $$ = $1;
       }
       | expr 
       {      
              $$ = new std::vector<InodeType*>;
              $$->push_back($1);
       }
       ;
%% 
void yyerror(const char *s) { 
    fprintf(stderr, "%s\n", s); 
} 
int main(int argc, const char* argv[]) { 
       yyin = fopen(argv[1], "r");     
       if (!yyin)
       {       
              yyin = stdin; 
       }    
       yyparse();         
       return 0; 
}