template <typename date>
ostream& Print(ostream& out, const date& container){
bool is_first = true;
    for (const auto& element : container) {
        if (!is_first) {
            out << ", "s;
        }
        is_first = false;
        out << element;
    }
  return out;
}

template <typename Key,typename Value>
ostream& Print(ostream& out, const map<Key,Value>& container){
bool is_first = true;
    for(const auto& [key,value] : container){
      if (!is_first) {
            out << ", "s;
        }
        is_first = false;   
    out <<key <<": "s <<value;    
    }
  return out;
}


template <typename Out_Vector>
ostream& operator<<(ostream& out, const vector <Out_Vector> & container) {
out << "["s;
Print(out,container);
out << "]"s;
return out;
}  

template <typename Out_Vector>
ostream& operator<<(ostream& out, const set <Out_Vector> & container) {
out << "{"s;
Print(out,container);
out << "}"s;
return out; 
}  

template <typename Out_Key, typename Out_Vol >
ostream& operator<<(ostream& out, const map <Out_Key,Out_Vol> & container) {
out << "{"s;
Print(out,container);
out << "}"s;
return out; 
}  



template <typename T, typename U>
void AssertEqualImpl(const T& t, const U& u, const string& t_str, const string& u_str, const string& file,
                     const string& func, unsigned line, const string& hint) {
    if (t != u) {
        cerr << boolalpha;
        cerr << file << "("s << line << "): "s << func << ": "s;
        cerr << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s;
        cerr << t << " != "s << u << "."s;
        if (!hint.empty()) {
            cerr << " Hint: "s << hint;
        }
        cerr << endl;
        abort();
    }
}

#define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, ""s)

#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint))

void AssertImpl(bool value, const string& expr_str, const string& file, const string& func, unsigned line,
                const string& hint) {
    if (!value) {
        cerr << file << "("s << line << "): "s << func << ": "s;
        cerr << "ASSERT("s << expr_str << ") failed."s;
        if (!hint.empty()) {
            cerr << " Hint: "s << hint;
        }
        cerr << endl;
        abort();
    }
}

#define ASSERT(expr) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, ""s)

#define ASSERT_HINT(expr, hint) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, (hint))

template <typename Func>
void RunTestImpl(Func func, const string& func_name) {
    /* Напишите недостающий код */
   func();
   cerr <<func_name <<" OK"  <<endl;
}

#define RUN_TEST(func) RunTestImpl((func),#func)