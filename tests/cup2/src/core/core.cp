#req("stdint.h") {};

#gen("T")
comp ptr<T> ();

#req("stdlib.h")
mod mem {
    #bind("malloc") sub alloc();
    #bind("realloc") sub realloc();
    #bind("sizeof") sub size(); 
    #bind("free") sub free(); 

    #req("string.h")
    #bind("memcpy") sub copy();
};

#req("stdio.h")
mod fmt {
    #bind("printf") sub print();
    #bind("vprintf") sub vprint();
};

#req("stdarg.h")
mod rest {
    #bind("va_list") comp args ();
    #bind("va_start") sub start ();
    #bind("va_end") sub end ();
};

#req("string.h")
mod cstr {
    #bind("strcmp") sub cmp();
    #bind("strlen") sub len();
    #bind("strcpy") sub copy();
};

#req("ctype.h")
mod char {
    #bind("isspace") sub is_space() {};
    #bind("isdigit") sub is_num() {};
    #bind("isalpha") sub is_alpha() {};
    #bind("isalnum") sub is_alpha_num() {};
};

#req("time.h")
mod time {
    #bind("CLOCKS_PER_SEC") int clocks_per_sec;
    #bind("clock") sub clock();
    
    f64 now() {
        ret (time:clock() as f64) / time:clocks_per_sec;
    };
};

#bind("exit") sub exit();

#os("win") #bind("HANDLE") comp ConsoleHandle ();
#os("win") ConsoleHandle console;
#os("win") #bind("STD_OUTPUT_HANDLE") int std_output_handle;

enum Color (
    Magenta,
    Green,
    Red,
);

#os("win") #bind("SetConsoleTextAttribute") sub set_console_text_attribute();
#os("win") #bind("GetStdHandle") sub get_std_handle();

mod color {
    #os("win")
    sub set(Color color) {
        int color_code; 
        match color {
            Color:Magenta { color_code = 5; },
            Color:Green { color_code = 10; },
            Color:Red { color_code = 12; },
        };
        set_console_text_attribute(console, color_code);   
    };

    #os("win")
    sub reset() {
        set_console_text_attribute(console, 7);   
    };

    #os("linux")
    sub set(Color color) {
        match color {
            Color:Magenta { fmt:print("\033[35m"); },
            Color:Green { fmt:print("\033[32m"); },
            Color:Red { fmt:print("\033[0;31m"); },
        };
    };

    #os("linux")
    sub reset() {
        fmt:print("\033[0m");
    };
};

comp dstr (
    ptr<u8> buf,
    int len,
    int cap,
);

def dstr {
    dstr new_with_cap(int cap) {
        ret new dstr {
            buf = mem:alloc(mem:size<u8>() * cap),
            len = 0,
            cap = cap,
        };
    };

    dstr new_from_cstr(ptr<u8> cstr) {
        int len = cstr:len(cstr) + 1;
        ptr<u8> buf = mem:alloc(mem:size<u8>() * len * 2);
        mem:copy(buf, cstr, mem:size<u8>() * len);
        ret new dstr {
            buf = buf,
            len = len - 1,
            cap = len * 2,
        };
    };
};

comp str (
    ptr<u8> buf,
    int len,
);

def str {
    str new_from_cstr(ptr<u8> cstr) {
        int len = cstr:len(cstr) + 1;
        ptr<u8> buf = mem:alloc(mem:size<u8>() * len);
        mem:copy(buf, cstr, mem:size<u8>() * len);
        ret new str {
            buf = buf,
            len = len - 1,
        };
    };
};

#gen("T")
comp arr<T> (
    ptr<T> buf,
    int len,
);

#gen("T")
def arr<T> {
    arr<T> new_with_len(int len) {
        ret new arr<T> {
            buf = mem:alloc(mem:size<T>() * len),
            len = len,
        };
    };

    arr<T> new_from_ptr(ptr<T> buf, int len) {
        ret new arr<T> {
            buf = buf,
            len = len,
        };
    };
};

#gen("T")
comp vec<T> (
    ptr<T> buf,
    int len,
    int cap,
);

#gen("T")
def vec<T> {
    vec<T> new_with_cap(int cap) {
        ret new vec<T> {
            buf = mem:alloc(mem:size<T>() * cap),
            len = 0,
            cap = cap,
        };
    };

    #self
    sub push(T item) {
        (this.buf + this.len)@ = item;
        this.len += 1;

        if this.len == this.cap {
            this.cap *= 4;
            this.buf = mem:realloc(this.buf, mem:size<T>() * this.cap);
        };
    };

    #self
    sub push_back(T item) {
        this.len += 1;

        if this.len == this.cap {
            this.cap *= 2;
            this.buf = mem:realloc(this.buf, mem:size<T>() * this.cap);
        };

        mem:copy(this.buf + 1, this.buf, mem:size<T>() * (this.len - 1));
        this[0] = item;
    };

    #self
    sub join(vec<T> other) {
        if other.len == 0 {
            ret;
        };
        int old_len = this.len;
        this.len += other.len;
        while this.len >= this.cap {
            this.cap *= 2;
            this.buf = mem:realloc(this.buf, mem:size<T>() * this.cap);
        };
        mem:copy(this.buf + old_len, other.buf, other.len * mem:size<T>());
    };

    #self
    sub join_back(vec<T> other) {
        if other.len == 0 {
            ret;
        };
        int old_len = this.len;
        this.len += other.len;
        while this.len >= this.cap {
            this.cap *= 2;
            this.buf = mem:realloc(this.buf, mem:size<T>() * this.cap);
        };
        mem:copy(this.buf + other.len, this.buf, mem:size<T>() * old_len);
        mem:copy(this.buf, other.buf, mem:size<T>() * other.len);
    };
};

#gen("T")
enum opt<T> (
    Some(T thing),
    None,
);

#gen("T")
def opt<T> {
    #self
    T unwrap() {
        match this {
            opt:Some(thing) {
                ret thing;
            },
            opt:None {
                exit(1);
            },
        };
    };
};