package machine;

public class Machine1 extends Machine1actions {

    private Machine1Context _fsm;
    private Machine1actions _actions;
    private boolean isCorrect;

    public Machine1(Machine1actions actions) //constructor
    {
        _fsm = new Machine1Context(this);
        _actions = actions;
    }
    public void Incorrect (){
        isCorrect = false;
    }
    public void Correct (){
        isCorrect = true;
    }
    public void addToBuffer(char symb) {
        _actions.addToBuffer(symb);
    }

    public void save_n_str() {
        _actions.save_n_str();
    }

    public void clear_buf() {
        _actions.clear_buf();
    }

    public void save_type() {
        _actions.save_type();
    }

    public void save_type_int() {
        _actions.save_type_int();
    }

    public void save_var() {
        _actions.save_var();
    }

    public void save_literal() {
        _actions.save_literal();
    }

    public boolean IsVariable() {
        return _actions.IsVariable();
    }

    public boolean IsType() {
        return _actions.IsType();
    }

    public boolean IsNum() {
        return _actions.IsNum();
    }

    public boolean Check_str(String line) {
        _fsm.enterStartState();
        statemap.State7 state = _fsm.getState();
        for (int i = 0; i < line.length(); i++) {
            char symb = line.charAt(i);
            if (symb >= '0' && symb <= '9') {
                _fsm.Digit(symb);
                continue;
            } else if (symb >= 'a' && symb <= 'z') {
                _fsm.Letter(symb);
                continue;
            } else if (symb >= 'A' && symb <= 'Z') {
                _fsm.BigLetter(symb);
                continue;
            } else if (symb == ' ' || symb == '\t') {
                _fsm.Space();
                continue;
            } else if (symb == '=') {
                _fsm.Equality();
                continue;
            } else if (symb == '\n') {
                _fsm.EOS();
                continue;
            } else {
                _fsm.Unknown();
                continue;
            }
        }
        _fsm.setState(state);
        return isCorrect;
    }
}