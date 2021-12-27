package machine;

public class Machine1actions {
    private String buffer;
    private String n_str;
    private String type;
    private String var;
    private String literal;

    public Machine1actions() {
        buffer = "";
        n_str = "";
        type = "";
        var = "";
        literal = "";
    }

    public String Get_type() {
        return type;
    }

    public String Get_var() {
        return var;
    }

    public String Get_n_str() {
        return n_str;
    }

    public void addToBuffer(char symb) {
        buffer = buffer + symb;
    }

    public void save_n_str() {
        n_str = buffer;
    }

    public void clear_buf() {
        buffer = "";
    }

    public void save_type() {
        type = buffer;
    }

    public void save_type_int() {
        type = "int";
    }

    public void save_var() {
        var = buffer;
    }

    public void save_literal() {
        literal = buffer;
    }

    public boolean IsVariable() {
        if (buffer.length() == 0) {
            return false;
        }
        char s = buffer.charAt(0);
        if (buffer.length() <= 16 && ((s >= 'a' && s <= 'z') || (s >= 'A' && s <= 'Z'))) {
            return true;
        } else {
            return false;
        }
    }

    public boolean IsType() {
        if (buffer.length() == 0) {
            return false;
        }
        if ((buffer.equals("int")) || (buffer.equals("short")) || (buffer.equals("long"))) {
            return true;
        } else {
            return false;
        }
    }

    public boolean IsNum() {
        if (buffer.length() == 0) {
            return false;
        }
        for (int i = 0; i < buffer.length(); i++) {
            char s = buffer.charAt(i);
            if ((s < '0') || (s > '9')) {
                return false;
            }
        }
        return true;
    }
}
