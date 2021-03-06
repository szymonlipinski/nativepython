#include "all.hpp"
//#include "GeneratedTypes.hpp"
#include "DirectTypesTest.hpp"

#define my_assert(cond) if (!(cond)) { std::cerr << "  failure: " << #cond << std::endl << "  " << __FILE__ << " line " << std::dec << __LINE__ << std::endl; return 1; }
#define test_fn_header() std::cerr << " Start " << __FUNCTION__ << "()" << std::endl;

void errlog(std::string a) { std::cerr << a << std::endl; }

int test_string() {
    test_fn_header()

    String s1("abc");
    my_assert(s1.size() == 3)
    my_assert(s1.getLayout()->bytes_per_codepoint == 1)
    std::string s2a("abc");

    String s2(s2a);
    my_assert(s1 == s2)
    {
        String s3(s2);
        my_assert(s3 == s2)
        String s4 = s2;
        my_assert(s4 == s2)
        my_assert(s1.getLayout()->refcount == 1)
        my_assert(s2.getLayout()->refcount == 3)
        my_assert(s3.getLayout()->refcount == 3)
        my_assert(s4.getLayout()->refcount == 3)
    }
    my_assert(s1.getLayout()->refcount == 1)
    my_assert(s2.getLayout()->refcount == 1)

    s2 = s1.upper();
    my_assert(s2 == String("ABC"))
    my_assert(s2.getLayout()->refcount == 1)
    s2 = s2.lower();
    my_assert(s2 == String("abc"))
    my_assert(s2.getLayout()->refcount == 1)

    String s3("abcxy");
    my_assert(s3.size() == 5)
    my_assert(s3.find(String("cx")) == 2)
    my_assert(s3.substr(1,4) == String("bcx"))

    String a1("DEF");
    String a2("ghij");
    String a3("K");
    String a4("mno");
    String s("blank");
    s = a1 + a2;
    my_assert(s == String("DEFghij"))
    my_assert(s.getLayout()->refcount == 1)
    s = a1 + a2 + a3;
    my_assert(s == String("DEFghijK"))
    my_assert(s.getLayout()->refcount == 1)
    s = a1 + a2 + a3 + a4;
    my_assert(s == String("DEFghijKmno"))
    my_assert(s.getLayout()->refcount == 1)

    String s4("test");
    s4 = s4;
    my_assert(s4.getLayout()->refcount == 1)
    s4 = String("12345");
    my_assert(s4.getLayout()->refcount == 1)
    s4 = String("aaaaaaaaaaaaaaaa");
    my_assert(s4.getLayout()->refcount == 1)
    s1 = String("aBc");
    s2 = String("123X56");
    s2 = String("123");
    s2 = String("xyz");
    s2 = String("ASD");

    my_assert(String("aBc").isalpha())
    my_assert(!String("a3Bc").isalpha())
    my_assert(String("a3Bc").isalnum())
    my_assert(!String("aB%c").isalnum())
    my_assert(String("43234").isdecimal())
    my_assert(!String("43r234").isdecimal())
    my_assert(String("43234").isdigit())
    my_assert(!String("43r234").isdigit())
    my_assert(String("abc 2").islower())
    my_assert(!String("4rL34").islower())
    my_assert(String("43234").isnumeric())
    my_assert(!String("43r234").isnumeric())
    my_assert(String("efg43234").isprintable())
    my_assert(!String("\x01").isprintable())
    my_assert(String("\n \r").isspace())
    my_assert(!String("\n A\r").isspace())
    my_assert(String("One Two").istitle())
    my_assert(!String("OnE Two").istitle())
    my_assert(String("OET").isupper())
    my_assert(!String("OnE Two").isupper())

    my_assert(String("a") > String("A"))
    my_assert(String("abc") < String("abcd"))

    String s5("one two three four");
    ListOf<String> parts = s5.split();
    my_assert(parts.size() == 4)
    my_assert(parts[0] == String("one"))
    my_assert(parts[1] == String("two"))
    my_assert(parts[2] == String("three"))
    my_assert(parts[3] == String("four"))
    parts = s5.split(String("t"));
    my_assert(parts.size() == 3)
    my_assert(parts[0] == String("one "))
    my_assert(parts[1] == String("wo "))
    my_assert(parts[2] == String("hree four"))
    parts = s5.split(2);
    my_assert(parts.size() == 3)
    my_assert(parts[0] == String("one"))
    my_assert(parts[1] == String("two"))
    my_assert(parts[2] == String("three four"))
    parts = s5.split(String("t"), 1);
    my_assert(parts.size() == 2)
    my_assert(parts[0] == String("one "))
    my_assert(parts[1] == String("wo three four"))
    return 0;
}

int test_bytes() {
    test_fn_header()
    Bytes b1("\x03\x02\x01\x00\x01\x02\x03\x04", 7);
    Bytes b2("\x00\x00\x00\x00\x01\x00\x00", 6);
    my_assert(b1.size() == 7)
    my_assert(b2.size() == 6)
    Bytes b3 = b1 + b2;
    my_assert(b3.size() == 13)
    my_assert(b3.getLayout()->refcount == 1)
    {
        Bytes b4(b3);
        my_assert(b3.getLayout()->refcount == 2)
        my_assert(b4.getLayout()->refcount == 2)
        Bytes b5 = b3;
        my_assert(b3.getLayout()->refcount == 3)
        my_assert(b4.getLayout()->refcount == 3)
        my_assert(b5.getLayout()->refcount == 3)
    }
    my_assert(b3.getLayout()->refcount == 1)


    return 0;
}

int test_list_of() {
    test_fn_header()

    ListOf<int64_t> list1;
    my_assert(list1.size() == 0)
    my_assert(list1.getLayout()->refcount == 1)

    ListOf<int64_t> list2({9, 8, 7});
    my_assert(list2.size() == 3)
    list2.append(6);
    my_assert(list2.size() == 4)
    my_assert(list2.getLayout()->refcount == 1)

    {
        ListOf<int64_t> list3 = list2;
        my_assert(list2.getLayout()->refcount == 2)
        my_assert(list3.getLayout()->refcount == 2)
        ListOf<int64_t> list4(list3);
        my_assert(list2.getLayout()->refcount == 3)
        my_assert(list3.getLayout()->refcount == 3)
        my_assert(list4.getLayout()->refcount == 3)
    }
    my_assert(list2.getLayout()->refcount == 1)
    my_assert(list2[2] == 7)
    list2[2] = 42;
    my_assert(list2[2] == 42)

    ListOf<String> list4({String("ab"), String("cd")});
    my_assert(list4.size() == 2)
    my_assert(list4[0] == String("ab"))
    my_assert(list4[1] == String("cd"))
    list4[1] = String("replace");
    my_assert(list4[1] == String("replace"))

    int64_t sum1 = 0;
    for (int64_t e: list2) {
        sum1 += e;
    }
    my_assert(sum1 == 65)

    int64_t sum2 = 0;
    for (auto it = begin(list2); it != end(list2); it++) {
        sum2 += *it;
    }
    my_assert(sum2 == 65)
    return 0;
}

int test_tuple_of() {
    test_fn_header()

    TupleOf<int64_t> t1;
    my_assert(t1.size() == 0)

    TupleOf<int64_t> t2({10, 12, 14, 16});
    my_assert(t2.getLayout()->refcount == 1)
    my_assert(t2.size() == 4)
    my_assert(t2.getLayout()->refcount == 1)

    {
        TupleOf<int64_t> t3 = t2;
        my_assert(t2.getLayout()->refcount == 2)
        my_assert(t3.getLayout()->refcount == 2)
        TupleOf<int64_t> t4(t3);
        my_assert(t2.getLayout()->refcount == 3)
        my_assert(t3.getLayout()->refcount == 3)
        my_assert(t4.getLayout()->refcount == 3)
    }
    my_assert(t2.getLayout()->refcount == 1)
    my_assert(t2[2] == 14)

    int64_t sum1 = 0;
    for (int64_t e: t2) {
        sum1 += e;
    }
    my_assert(sum1 == 52)

    int64_t sum2 = 0;
    for (auto it = begin(t2); it != end(t2); it++) {
        sum2 += *it;
    }
    my_assert(sum2 == 52)
    return 0;
}

int test_dict() {
    test_fn_header()

    Dict<bool, int64_t> d1;
    d1.insertKeyValue(false, 13);
    d1.insertKeyValue(true, 17);
    const int64_t *pv = d1.lookupValueByKey(false);
    my_assert(pv)
    my_assert(*pv == 13)
    pv = d1.lookupValueByKey(true);
    my_assert(pv)
    my_assert(*pv == 17)
    my_assert(d1.getLayout()->refcount == 1)
    bool deleted = d1.deleteKey(true);
    my_assert(deleted)
    pv = d1.lookupValueByKey(true);
    my_assert(pv == 0)

    {
        Dict<bool, int64_t>d2 = d1;
        my_assert(d1.getLayout()->refcount == 2)
        my_assert(d2.getLayout()->refcount == 2)
        Dict<bool, int64_t>d3 = d2;
        my_assert(d1.getLayout()->refcount == 3)
        my_assert(d2.getLayout()->refcount == 3)
        my_assert(d3.getLayout()->refcount == 3)
    }
    my_assert(d1.getLayout()->refcount == 1)

    Dict<String, String> d4;
    d4.insertKeyValue(String("first"), String("1st"));
    d4.insertKeyValue(String("second"), String("2nd"));
    d4.insertKeyValue(String("third"), String("3rd"));
    const String *ps = d4.lookupValueByKey(String("second"));
    my_assert(ps)
    my_assert(*ps == String("2nd"))
    my_assert(d4[String("first")] == String("1st"))
    my_assert(d4[String("third")] == String("3rd"))

    String k;
    String v;
    for (auto e: d4) {
        k += e.first;
        v += e.second;
    }
    my_assert(k == String("firstsecondthird"))
    my_assert(v == String("1st2nd3rd"))

    String s;
    for (auto it = begin(d4); it != end(d4); ++it) {
        s += (*it).first + String(":") + (*it).second + String(" ");
    }
    my_assert(s == String("first:1st second:2nd third:3rd "))
    return 0;
}

int test_const_dict() {
    test_fn_header()

    ConstDict<String, int64_t> c1({{String("a"), 5}, {String("c"), 7}, {String("b"), 6}});
    my_assert(c1.size() == 3)

    const int64_t *pv = c1.lookupValueByKey(String("a"));
    my_assert(pv)
    my_assert(*pv == 5)
    pv = c1.lookupValueByKey(String("b"));
    my_assert(pv)
    my_assert(*pv == 6)
    pv = c1.lookupValueByKey(String("c"));
    my_assert(pv)
    my_assert(*pv == 7)
    {
        ConstDict<String, int64_t>c2(c1);
        my_assert(c1.getLayout()->refcount == 2)
        my_assert(c2.getLayout()->refcount == 2)
        ConstDict<String, int64_t>c3 = c1;
        my_assert(c1.getLayout()->refcount == 3)
        my_assert(c2.getLayout()->refcount == 3)
        my_assert(c3.getLayout()->refcount == 3)
    }
    my_assert(c1.getLayout()->refcount == 1)
    ConstDict<String, int64_t> c4({{String("d"), 8}, {String("e"), 9}, {String("c"), 222}, {String("f"), 10}, {String("g"), 11}});
    my_assert(c4.size() == 5)
    ConstDict<String, int64_t> c5 = c1 + c4;
    my_assert(c5.size() == 7)
    pv = c5.lookupValueByKey(String("c"));
    my_assert(pv)
    my_assert(*pv == 222)
    pv = c5.lookupValueByKey(String("d"));
    my_assert(pv)
    my_assert(*pv == 8)
    pv = c5.lookupValueByKey(String("e"));
    my_assert(pv)
    my_assert(*pv == 9)
    pv = c5.lookupValueByKey(String("f"));
    my_assert(pv)
    my_assert(*pv == 10)
    pv = c5.lookupValueByKey(String("g"));
    my_assert(pv)
    my_assert(*pv == 11)

    String k;
    int64_t sum = 0;
    for (auto e: c5) {
        k += e.first;
        sum += e.second;
    }
    my_assert(k == String("abcdefg"))
    my_assert(sum == 5+6+222+8+9+10+11)

    TupleOf<String> t1({String("b"), String("d"), String("f")});
    ConstDict<String, int64_t> c6 = c5 - t1;
    my_assert(c6.size() == 4)
    pv = c6.lookupValueByKey(String("b"));
    my_assert(!pv)
    pv = c6.lookupValueByKey(String("d"));
    my_assert(!pv)
    pv = c6.lookupValueByKey(String("f"));
    my_assert(!pv)
    pv = c6.lookupValueByKey(String("g"));
    my_assert(pv)
    my_assert(*pv == 11)

    String k2;
    int64_t sum2 = 0;
    for (auto it = begin(c6); it != end(c6); ++it) {
        k2 += (*it).first;
        sum2 += (*it).second;
    }
    my_assert(k2 == String("aceg"))
    my_assert(sum2 == 5+222+9+11)
    return 0;
}

int test_one_of() {
    test_fn_header()

    OneOf<bool, String> o1(true);
    my_assert(o1.getLayout()->which == 0)

    OneOf<bool, String> o2(String("true"));
    my_assert(o2.getLayout()->which == 1)

    OneOf<bool, String, TupleOf<int>> o3;
    bool b;
    String s;
    TupleOf<int> t;
    o3 = TupleOf<int>({9,8,7});
    my_assert(o3.getLayout()->which == 2)
    my_assert(!o3.getValue(b))
    my_assert(!o3.getValue(s))
    my_assert(o3.getValue(t))
    my_assert(t.size() == 3)
    my_assert(t[0] == 9 && t[1] == 8 && t[2] == 7)

    o3 = false;
    my_assert(o3.getLayout()->which == 0)
    my_assert(!o3.getValue(s))
    my_assert(!o3.getValue(t))
    my_assert(o3.getValue(b))
    my_assert(b == false)

    o3 = String("yes");
    my_assert(o3.getLayout()->which == 1)
    my_assert(!o3.getValue(b))
    my_assert(!o3.getValue(t))
    my_assert(o3.getValue(s))
    my_assert(s == String("yes"))

    // fails, since the template doesn't collapse contained OneOf types:
//    OneOf<OneOf<bool, String>, OneOf<int32_t, String>> o4;
//    o4 = String("a");
//    o4 = (int32_t)9;
//    o4 = true;
    return 0;
}

int test_named_tuple() {
    test_fn_header()

    NamedTupleTwoStrings t1;
    t1.X() = String("one");
    t1.Y() = String("two");
    NamedTupleTwoStrings t2(t1);
    my_assert(t2.X() == String("one"))
    my_assert(t2.Y() == String("two"))
    NamedTupleTwoStrings t3 = t1;
    my_assert(t3.X() == String("one"))
    my_assert(t3.Y() == String("two"))

    NamedTupleBoolIntStr t4(true, 4321, String("y"));
    my_assert(t4.b() == true)
    my_assert(t4.i() == 4321)
    my_assert(t4.s() == String("y"))
    t4.b() = false;
    t4.i() = 9;
    t4.s() = String("n");
    my_assert(t4.b() == false)
    my_assert(t4.i() == 9)
    my_assert(t4.s() == String("n"))
    return 0;
}

// Depends on generated types A and Overlap in GeneratedTypes.hpp
int test_alternative() {
    test_fn_header()

    A_Sub1 asub1(12, 34);
    my_assert(asub1.isSub1())
    my_assert(!asub1.isSub2())
    my_assert(asub1.b() == 12)
    my_assert(asub1.c() == 34)
    A_Sub2 asub2(String("this"), String("that"));
    my_assert(!asub2.isSub1())
    my_assert(asub2.isSub2())
    my_assert(asub2.d() == String("this"))
    my_assert(asub2.e() == String("that"))

    A a1 = A_Sub1(111, 222);
    my_assert(a1.isSub1())
    my_assert(!a1.isSub2())
    my_assert(a1.b() == 111)
    my_assert(a1.c() == 222)
    A a2 = A_Sub2(String("this"), String("that"));
    my_assert(!a2.isSub1())
    my_assert(a2.isSub2())
    my_assert(a2.d() == String("this"))
    my_assert(a2.e() == String("that"))
    my_assert(a2.getLayout()->refcount == 1)
    {
        A a3(a2);
        A a4 = a2;
        my_assert(a2.getLayout()->refcount == 3)
        my_assert(a3.getLayout()->refcount == 3)
        my_assert(a4.getLayout()->refcount == 3)
    }
    my_assert(a2.getLayout()->refcount == 1)

    Overlap o1 = Overlap::Sub1(true, 2);
    my_assert(o1.isSub1())
    bool val1;
    o1.b().getValue(val1);
    my_assert(val1 == true)

    o1 = Overlap::Sub2(String("over"), TupleOf<String>({String("a"), String("b")}));
    my_assert(o1.isSub2())
    String val2;
    o1.b().getValue(val2);
    my_assert(val2 == String("over"))

    o1 = Overlap::Sub3(45);
    my_assert(o1.isSub3())
    int64_t val3;
    o1.b().getValue(val3);
    my_assert(val3 == 45)

    Bexpress_Leaf b0;
    Bexpress b1 = Bexpress_Leaf(true);
    Bexpress b2 = Bexpress_Leaf(false);
    Bexpress b3 = Bexpress_Leaf(true);
    Bexpress b4 = Bexpress_Leaf(false);
    Bexpress b5 = Bexpress_BinOp(b1, String("and"), b2);
    Bexpress b6 = Bexpress_UnaryOp(String("not"), b3);
    Bexpress b7 = Bexpress_BinOp(b6, String("and"), b4);
    Bexpress b8 = Bexpress_BinOp(b5, String("or"), b7);
    my_assert(b1.isLeaf())
    my_assert(b1.value() == true)
    my_assert(b2.isLeaf())
    my_assert(b2.value() == false)
    my_assert(b3.isLeaf())
    my_assert(b3.value() == true)
    my_assert(b4.isLeaf())
    my_assert(b4.value() == false)
    my_assert(b5.isBinOp())
    my_assert(b5.op() == String("and"))
    my_assert(b6.isUnaryOp())
    my_assert(b6.op() == String("not"))
    my_assert(b7.isBinOp())
    my_assert(b7.op() == String("and"))
    my_assert(b8.isBinOp())
    my_assert(b8.op() == String("or"))
    my_assert(b8.left().isBinOp())
    my_assert(b8.left().op() == String("and"))
    my_assert(b8.right().isBinOp())
    my_assert(b8.right().op() == String("and"))
    my_assert(b8.left().left().isLeaf())
    my_assert(b8.left().left().value() == true)
    my_assert(b8.left().right().isLeaf())
    my_assert(b8.left().right().value() == false)
    my_assert(b8.right().left().isUnaryOp())
    my_assert(b8.right().left().op() == String("not"))
    my_assert(b8.right().right().isLeaf())
    my_assert(b8.right().right().value() == false)
    my_assert(b8.right().left().right().isLeaf())
    my_assert(b8.right().left().right().value() == true)

    return 0;
}

int test_none() {
    test_fn_header()

    OneOf<None, bool> o1(true);
    my_assert(o1.getLayout()->which == 1)
    None n;
    OneOf<None, bool> o2(n);
    my_assert(o2.getLayout()->which == 0)

    return 0;
}

int direct_cpp_tests() {
    int ret = 0;
    std::cerr << "Start " << __FUNCTION__ << "()" << std::endl;
    ret += test_string();
    ret += test_bytes();
    ret += test_list_of();
    ret += test_tuple_of();
    ret += test_dict();
    ret += test_const_dict();
    ret += test_one_of();
    ret += test_named_tuple();
    ret += test_alternative();
    ret += test_none();

    std::cerr << ret << " test" << (ret == 1 ? "" : "s") << " failed" << std::endl;

    return ret > 0;
}
