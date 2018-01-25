
#include "./test_case.hpp"

namespace yml = c4::yml;

const char ex[] = R"(# this is a comment
foo: fsdfkjhsdfkh
bar: sdfjkhfuu
bat: 1
seq: [0, 1, 2, 3, 4]
map:
  foo: 1
  bar: 2
  baz: 3
  submap:
    subfoo: 11
    subbar: 12
    subbaz: 13
    subbat: 14
fdx: crl
)";

int do_test();

int main()
{
    C4_ASSERT( ! yml::cspan(":").begins_with(": "));

    int stat = do_test();
    return stat;

#ifdef OLDLIBYAML_PARSER
    yml::Tree s(2);
    yml::Parser p;
    p.parse(&s, ex);
    C4_ASSERT(s.root() != nullptr);
    auto &root = *s.root();
    C4_ASSERT(root.is_root());
    show_children(root);
    auto &doc = *s.first_doc();
    C4_ASSERT(doc.type() == yml::DOCMAP);
    C4_ASSERT(doc.num_children() == 6);
    show_children(doc);
    show_children(doc["seq"]);
    show_children(doc["map"]);
    show_children(doc["map"]["submap"]);
    C4_ASSERT(&doc[0] == &doc["foo"]);
    C4_ASSERT(&doc[1] == &doc["bar"]);
    C4_ASSERT(&doc[2] == &doc["bat"]);
    C4_ASSERT(&doc[3] == &doc["seq"]);
    C4_ASSERT(&doc[4] == &doc["map"]);
    C4_ASSERT(&doc[5] == &doc["fdx"]);
    C4_ASSERT(doc[0].name() == "foo" && doc[0].is_val() && doc[0].val() == "fsdfkjhsdfkh");
    C4_ASSERT(doc[1].name() == "bar" && doc[1].is_val() && doc[1].val() == "sdfjkhfuu");
    C4_ASSERT(doc[2].name() == "bat" && doc[2].is_val() && doc[2].val() == "1");
    C4_ASSERT(doc[3].name() == "seq" && doc[3].is_seq());
    C4_ASSERT(doc[4].name() == "map" && doc[4].is_map());
    C4_ASSERT(doc[5].name() == "fdx" && doc[5].is_val() && doc[5].val() == "crl");

    auto &seq = doc["seq"];
    C4_ASSERT(seq.num_children() == 5);
    C4_ASSERT(seq[0].val() == "0");
    C4_ASSERT(seq[1].val() == "1");
    C4_ASSERT(seq[2].val() == "2");
    C4_ASSERT(seq[3].val() == "3");
    C4_ASSERT(seq[4].val() == "4");

    #define _chm(m, i, n, v) C4_ASSERT(&m[n] == &m[i] && m[n].name() == n && m[n].val() == v);
    auto &map = doc["map"];
    C4_ASSERT(map.num_children() == 4);
    _chm(map, 0, "foo", "1");
    _chm(map, 1, "bar", "2");
    _chm(map, 2, "baz", "3");

    C4_ASSERT(&map[3] == &map["submap"] && map["submap"].is_map());
    auto &smap = doc["map"]["submap"];
    _chm(smap, 0, "subfoo", "11");
    _chm(smap, 1, "subbar", "12");
    _chm(smap, 2, "subbaz", "13");
    _chm(smap, 3, "subbat", "14");

    //-----------------------------------------------

    //    doc["new_child"] = "new_value";
    doc.append_child("new_child", "new_value");
    C4_ASSERT(&doc[6] == &doc["new_child"]);
    C4_ASSERT(doc["new_child"].val() == "new_value");

    doc.append_child("new_map", yml::MAP);
    C4_ASSERT(&doc[7] == &doc["new_map"]);
    C4_ASSERT(doc["new_map"].type() == yml::MAP);

    auto &nm = doc["new_map"];
    nm.prepend_child("fdx", "crl");
    nm.prepend_child("pqp", "ccm");
    C4_ASSERT(nm.num_children() == 2);
    C4_ASSERT(&nm[0] == &nm["pqp"] && nm[0] == "ccm");
    C4_ASSERT(&nm[1] == &nm["fdx"] && nm[1] == "crl");
    nm.insert_child("xxx", "yyy", &nm["pqp"]);
    show_children(nm);
    C4_ASSERT(nm.num_children() == 3);
    C4_ASSERT(&nm[0] == &nm["pqp"] && nm[0] == "ccm");
    C4_ASSERT(&nm[1] == &nm["xxx"] && nm[1] == "yyy");
    C4_ASSERT(&nm[2] == &nm["fdx"] && nm[2] == "crl");

    yml::Parser p2;
    nm.append_child("serialized", yml::MAP);
    C4_ASSERT(nm.num_children() == 4);

    C4_ASSERT(nm["serialized"].type() == yml::MAP);
    s.load(&nm["serialized"], R"(
prop1: val1
prop2: val2
prop3: val3
prop4: [seq1, seq2, seq3]
)", &p2);
    show_children((*s.first_doc())["new_map"]["serialized"]);


    //------------------------------------------------------------------------
    printf(".....\n");

    yml::Node *n = s.first_doc()->find_child("seq");
    size_t num = yml::emit(n);
    printf("stdout result: %zd chars\n", num);

    char buf[1024] = {0};
    yml::span sp(buf, sizeof(buf));

    auto result = yml::emit(n, sp);
    printf("str result: %zd chars:\n%.*s\n", result.len, (int)result.len, result.str);

    C4_ASSERT(yml::emit(n) == yml::emit(n, sp).len);

    printf("-----------------\nchecks ok!!!!!\n-----------------\n");


    //-----------------------------------------------------------------------------
const char ex2[] = R"(# this is a comment
seq:
  - 0
  - 1
  - 2
  - 3
  - 4
foo: fsdfkjhsdfkh
bar: sdfjkhfuu
bat: 1
map:
  foo: 1
  bar: 2
  baz: 3
explseq: [0, 1, 2, 3, 4]
explmap: {foo: 1, bar: 2, baz: 3}
explseq2: [[0, 1, 2], [3, 4, 5], [6, 7, 8], [9, 10, 11]]
explmap2: {keyfoo: {foo: 1, bar: 2, baz: 3}, keybar: {foo: 4, bar: 5, baz: 6}, keybaz: {foo: 7, bar: 8, baz: 9}}
explseq3: [
  [0, 1, 2],
  [3, 4, 5],
  [6, 7, 8],
  [9, 10, 11]
]
explmap3: {
  keyfoo: {foo: 1, bar: 2, baz: 3},
  keybar: {foo: 4, bar: 5, baz: 6},
  keybaz: {foo: 7, bar: 8, baz: 9}
}
explseq4: [
  [
    0,
    1,
    2
  ]
  ,
  [ 3,
    4,
    5],
  [6,
    7,
    8],[
   9, 10, 11]]
explmap3: {
  keyfoo: {
    foo: 1,
    bar: 2,
    baz: 3
  }
  ,
  keybar: {
    foo: 4,
    bar: 5,
    baz: 6
  },
  keybaz:
  {
    foo: 7,
    bar: 8,
    baz: 9
}
}
seq2:
  - - 0
    - 1
    - 2
  - [3, 4, 5]
  - - 6
    - 7
    - 8
  - [9, 10, 11]
map2:
  indent: 1
  submap:
    subfoo: 11
    subbar: 12
    subbaz: 13
    subbat: 14
  deindent: 1
  submap2:
    subfoo: 11
    subbar: 12
    subbaz: 13
    subbat: 14
seq3:
  - - - 0
      - 1
      - 2
    - - 3
      - 4
      - 5
  - -
      - 6
  -
    -
      - 7
  -
    -
      -
        8
map3:
  indent: 1
  submap:
    subfoo: 11
    subbar: 12
    subbaz: 13
    subbat: 14
  deindent: 1
  submap2:
    subfoo: 11
    subbar: 12
    subbaz: 13
    subbat: 14
seqmap:
  -
    foo: 1
    bar: 2
    baz: 3
  -
    foo: 4
    bar: 5
    baz: 6
fdx: crl
)";

    yml::NextParser np;
    yml::Tree t = np.parse("inline source", ex2);

    return 0;
#endif
}






int do_test()
{
    using namespace c4::yml;

    using C = Case;
    using N = CaseNode;
    using L = CaseNode::iseqmap;

    {
        L tl1 = {DOC, DOC};
        L tl2 = {(DOC), (DOC)};

        C4_ASSERT(tl1.size() == tl2.size());
        N const& d1 = *tl1.begin();
        N const& d2 = *(tl1.begin() + 1);
        C4_ASSERT(d1.reccount() == d2.reccount());
        C4_ASSERT(d1.type == DOC);
        C4_ASSERT(d2.type == DOC);

        N n1(tl1);
        N n2(tl2);
        C4_ASSERT(n1.reccount() == n2.reccount());
    }
    CaseContainer tests({

//-----------------------------------------------------------------------------

C("empty file",
"",
    NOTYPE
),

C("empty file, multiline",
R"(



)",
    NOTYPE
),

//-----------------------------------------------------------------------------

C("one empty doc",
R"(---
)",
    N(STREAM, L{DOC})
),

C("one empty doc, explicit termination",
R"(---
...
)",
    N(STREAM, L{DOC})
),

C("two empty docs",
R"(---
---
)",
    N(STREAM, L{DOC, DOC})
),

//-----------------------------------------------------------------------------

C("empty map",
"{}",
    MAP
),

C("empty map, multiline",
R"({
}
)",
    MAP
),

C("empty map, multilines",
R"({
# foo bar baz akjasdkj
}
)",
    MAP
),

C("simple map, explicit, single line",
"{foo: 0, bar: 1, baz: 2, bat: 3}",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, explicit, multiline, unindented",
R"({
foo: 0,
bar: 1,
baz: 2,
bat: 3
})",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, explicit, multiline, indented",
R"({
  foo: 0,
  bar: 1,
  baz: 2,
  bat: 3
})",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map",
R"(
foo: 0
bar: 1
baz: 2
bat: 3
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, with comments",
R"(
foo: 0   # this is a foo
bar: 1   # this is a bar
baz: 2   # this is a bar
bat: 3   # this is a bar
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, with comments interspersed",
R"(
# this is a foo
foo: 0
# this is a bar
bar: 1
# this is a baz
baz: 2
# this is a bat
bat: 3
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),


//-----------------------------------------------------------------------------

C("nested map x2, explicit, same line",
R"({foo: {foo0: 00, bar0: 01, baz0: 02}, bar: {foo1: 10, bar1: 11, baz1: 12}, baz: {foo2: 20, bar2: 21, baz2: 22}})",
    L{
      N{"foo", L{N{"foo0", "00"}, N{"bar0", "01"}, N{"baz0", "02"}}},
      N{"bar", L{N{"foo1", "10"}, N{"bar1", "11"}, N{"baz1", "12"}}},
      N{"baz", L{N{"foo2", "20"}, N{"bar2", "21"}, N{"baz2", "22"}}},
          }
),

C("nested map x2, explicit",
R"({
foo: {foo0: 00, bar0: 01, baz0: 02},
bar: {foo1: 10, bar1: 11, baz1: 12},
baz: {foo2: 20, bar2: 21, baz2: 22}
})",
    L{
      N{"foo", L{N{"foo0", "00"}, N{"bar0", "01"}, N{"baz0", "02"}}},
      N{"bar", L{N{"foo1", "10"}, N{"bar1", "11"}, N{"baz1", "12"}}},
      N{"baz", L{N{"foo2", "20"}, N{"bar2", "21"}, N{"baz2", "22"}}},
          }
),

C("nested map x2",
R"(
foo:
  foo0: 00
  bar0: 01
  baz0: 02
bar:
  foo1: 10
  bar1: 11
  baz1: 12
baz:
  foo2: 20
  bar2: 21
  baz2: 22
)",
    L{
      N{"foo", L{N{"foo0", "00"}, N{"bar0", "01"}, N{"baz0", "02"}}},
      N{"bar", L{N{"foo1", "10"}, N{"bar1", "11"}, N{"baz1", "12"}}},
      N{"baz", L{N{"foo2", "20"}, N{"bar2", "21"}, N{"baz2", "22"}}},
          }
),

//-----------------------------------------------------------------------------
C("nested map x3, explicit",
R"({
  foo0: {
     foo1: {foo2: 000, bar2: 001, baz2: 002},
     bar1: {foo2: 010, bar2: 011, baz2: 012},
     baz1: {foo2: 020, bar2: 021, baz2: 022}
  },
  bar0: {
     foo1: {foo2: 100, bar2: 101, baz2: 102},
     bar1: {foo2: 110, bar2: 111, baz2: 112},
     baz1: {foo2: 120, bar2: 121, baz2: 122}
  },
  baz0: {
     foo1: {foo2: 200, bar2: 201, baz2: 202},
     bar1: {foo2: 210, bar2: 211, baz2: 212},
     baz1: {foo2: 220, bar2: 221, baz2: 222}
  }
})",
    L{
      N{"foo0", L{
         N{"foo1", L{N{"foo2", "000"}, N{"bar2", "001"}, N{"baz2", "002"}}},
         N{"bar1", L{N{"foo2", "010"}, N{"bar2", "011"}, N{"baz2", "012"}}},
         N{"baz1", L{N{"foo2", "020"}, N{"bar2", "021"}, N{"baz2", "022"}}} }},
      N{"bar0", L{
         N{"foo1", L{N{"foo2", "100"}, N{"bar2", "101"}, N{"baz2", "102"}}},
         N{"bar1", L{N{"foo2", "110"}, N{"bar2", "111"}, N{"baz2", "112"}}},
         N{"baz1", L{N{"foo2", "120"}, N{"bar2", "121"}, N{"baz2", "122"}}} }},
      N{"baz0", L{
         N{"foo1", L{N{"foo2", "200"}, N{"bar2", "201"}, N{"baz2", "202"}}},
         N{"bar1", L{N{"foo2", "210"}, N{"bar2", "211"}, N{"baz2", "212"}}},
         N{"baz1", L{N{"foo2", "220"}, N{"bar2", "221"}, N{"baz2", "222"}}} }},
          }
),

C("nested map x3",
R"(
foo0:
  foo1:
    foo2: 000
    bar2: 001
    baz2: 002
  bar1:
    foo2: 010
    bar2: 011
    baz2: 012
  baz1:
    foo2: 020
    bar2: 021
    baz2: 022
bar0:
  foo1:
    foo2: 100
    bar2: 101
    baz2: 102
  bar1:
    foo2: 110
    bar2: 111
    baz2: 112
  baz1:
    foo2: 120
    bar2: 121
    baz2: 122
baz0:
  foo1:
    foo2: 200
    bar2: 201
    baz2: 202
  bar1:
    foo2: 210
    bar2: 211
    baz2: 212
  baz1:
    foo2: 220
    bar2: 221
    baz2: 222
)",
    L{
      N{"foo0", L{
         N{"foo1", L{N{"foo2", "000"}, N{"bar2", "001"}, N{"baz2", "002"}}},
         N{"bar1", L{N{"foo2", "010"}, N{"bar2", "011"}, N{"baz2", "012"}}},
         N{"baz1", L{N{"foo2", "020"}, N{"bar2", "021"}, N{"baz2", "022"}}} }},
      N{"bar0", L{
         N{"foo1", L{N{"foo2", "100"}, N{"bar2", "101"}, N{"baz2", "102"}}},
         N{"bar1", L{N{"foo2", "110"}, N{"bar2", "111"}, N{"baz2", "112"}}},
         N{"baz1", L{N{"foo2", "120"}, N{"bar2", "121"}, N{"baz2", "122"}}} }},
      N{"baz0", L{
         N{"foo1", L{N{"foo2", "200"}, N{"bar2", "201"}, N{"baz2", "202"}}},
         N{"bar1", L{N{"foo2", "210"}, N{"bar2", "211"}, N{"baz2", "212"}}},
         N{"baz1", L{N{"foo2", "220"}, N{"bar2", "221"}, N{"baz2", "222"}}} }},
      }
),

//-----------------------------------------------------------------------------
C("nested map x4, explicit",
R"({
  foo0: {
     foo1: { foo2: {foo3: 0000, bar3: 0001, baz3: 0002}, bar2: {foo3: 0010, bar3: 0011, baz3: 0012}, baz2: {foo3: 0020, bar3: 0021, baz3: 0022} },
     bar1: { foo2: {foo3: 0100, bar3: 0101, baz3: 0102}, bar2: {foo3: 0110, bar3: 0111, baz3: 0112}, baz2: {foo3: 0120, bar3: 0121, baz3: 0122} },
     baz1: { foo2: {foo3: 0200, bar3: 0201, baz3: 0202}, bar2: {foo3: 0210, bar3: 0211, baz3: 0212}, baz2: {foo3: 0220, bar3: 0221, baz3: 0222} },
  },
  bar0: {
     foo1: { foo2: {foo3: 1000, bar3: 1001, baz3: 1002}, bar2: {foo3: 1010, bar3: 1011, baz3: 1012}, baz2: {foo3: 1020, bar3: 1021, baz3: 1022} },
     bar1: { foo2: {foo3: 1100, bar3: 1101, baz3: 1102}, bar2: {foo3: 1110, bar3: 1111, baz3: 1112}, baz2: {foo3: 1120, bar3: 1121, baz3: 1122} },
     baz1: { foo2: {foo3: 1200, bar3: 1201, baz3: 1202}, bar2: {foo3: 1210, bar3: 1211, baz3: 1212}, baz2: {foo3: 1220, bar3: 1221, baz3: 1222} },
  },
  baz0: {
     foo1: { foo2: {foo3: 2000, bar3: 2001, baz3: 2002}, bar2: {foo3: 2010, bar3: 2011, baz3: 2012}, baz2: {foo3: 2020, bar3: 2021, baz3: 2022} },
     bar1: { foo2: {foo3: 2100, bar3: 2101, baz3: 2102}, bar2: {foo3: 2110, bar3: 2111, baz3: 2112}, baz2: {foo3: 2120, bar3: 2121, baz3: 2122} },
     baz1: { foo2: {foo3: 2200, bar3: 2201, baz3: 2202}, bar2: {foo3: 2210, bar3: 2211, baz3: 2212}, baz2: {foo3: 2220, bar3: 2221, baz3: 2222} },
  },
})",
    L{
      N("foo0", L{
         N("foo1", L{N("foo2", L{N("foo3", "0000"), N("bar3", "0001"), N("baz3", "0002")}),  N("bar2", L{N("foo3", "0010"), N("bar3", "0011"), N("baz3", "0012")}),  N("baz2", L{N("foo3", "0020"), N("bar3", "0021"), N("baz3", "0022")})}),
         N("bar1", L{N("foo2", L{N("foo3", "0100"), N("bar3", "0101"), N("baz3", "0102")}),  N("bar2", L{N("foo3", "0110"), N("bar3", "0111"), N("baz3", "0112")}),  N("baz2", L{N("foo3", "0120"), N("bar3", "0121"), N("baz3", "0122")})}),
         N("baz1", L{N("foo2", L{N("foo3", "0200"), N("bar3", "0201"), N("baz3", "0202")}),  N("bar2", L{N("foo3", "0210"), N("bar3", "0211"), N("baz3", "0212")}),  N("baz2", L{N("foo3", "0220"), N("bar3", "0221"), N("baz3", "0222")})}),
      }),
      N("bar0", L{
         N("foo1", L{N("foo2", L{N("foo3", "1000"), N("bar3", "1001"), N("baz3", "1002")}),  N("bar2", L{N("foo3", "1010"), N("bar3", "1011"), N("baz3", "1012")}),  N("baz2", L{N("foo3", "1020"), N("bar3", "1021"), N("baz3", "1022")})}),
         N("bar1", L{N("foo2", L{N("foo3", "1100"), N("bar3", "1101"), N("baz3", "1102")}),  N("bar2", L{N("foo3", "1110"), N("bar3", "1111"), N("baz3", "1112")}),  N("baz2", L{N("foo3", "1120"), N("bar3", "1121"), N("baz3", "1122")})}),
         N("baz1", L{N("foo2", L{N("foo3", "1200"), N("bar3", "1201"), N("baz3", "1202")}),  N("bar2", L{N("foo3", "1210"), N("bar3", "1211"), N("baz3", "1212")}),  N("baz2", L{N("foo3", "1220"), N("bar3", "1221"), N("baz3", "1222")})}),
      }),
      N("baz0", L{
         N("foo1", L{N("foo2", L{N("foo3", "2000"), N("bar3", "2001"), N("baz3", "2002")}),  N("bar2", L{N("foo3", "2010"), N("bar3", "2011"), N("baz3", "2012")}),  N("baz2", L{N("foo3", "2020"), N("bar3", "2021"), N("baz3", "2022")})}),
         N("bar1", L{N("foo2", L{N("foo3", "2100"), N("bar3", "2101"), N("baz3", "2102")}),  N("bar2", L{N("foo3", "2110"), N("bar3", "2111"), N("baz3", "2112")}),  N("baz2", L{N("foo3", "2120"), N("bar3", "2121"), N("baz3", "2122")})}),
         N("baz1", L{N("foo2", L{N("foo3", "2200"), N("bar3", "2201"), N("baz3", "2202")}),  N("bar2", L{N("foo3", "2210"), N("bar3", "2211"), N("baz3", "2212")}),  N("baz2", L{N("foo3", "2220"), N("bar3", "2221"), N("baz3", "2222")})}),
      })
    }
),

C("nested map x4",
R"(
foo0:
  foo1:
    foo2:
      foo3: 0000
      bar3: 0001
      baz3: 0002
    bar2:
      foo3: 0010
      bar3: 0011
      baz3: 0012
    baz2:
      foo3: 0020
      bar3: 0021
      baz3: 0022
  bar1:
    foo2:
      foo3: 0100
      bar3: 0101
      baz3: 0102
    bar2:
      foo3: 0110
      bar3: 0111
      baz3: 0112
    baz2:
      foo3: 0120
      bar3: 0121
      baz3: 0122
  baz1:
    foo2:
      foo3: 0200
      bar3: 0201
      baz3: 0202
    bar2:
      foo3: 0210
      bar3: 0211
      baz3: 0212
    baz2:
      foo3: 0220
      bar3: 0221
      baz3: 0222
bar0:
  foo1:
    foo2:
      foo3: 1000
      bar3: 1001
      baz3: 1002
    bar2:
      foo3: 1010
      bar3: 1011
      baz3: 1012
    baz2:
      foo3: 1020
      bar3: 1021
      baz3: 1022
  bar1:
    foo2:
      foo3: 1100
      bar3: 1101
      baz3: 1102
    bar2:
      foo3: 1110
      bar3: 1111
      baz3: 1112
    baz2:
      foo3: 1120
      bar3: 1121
      baz3: 1122
  baz1:
    foo2:
      foo3: 1200
      bar3: 1201
      baz3: 1202
    bar2:
      foo3: 1210
      bar3: 1211
      baz3: 1212
    baz2:
      foo3: 1220
      bar3: 1221
      baz3: 1222
baz0:
  foo1:
    foo2:
      foo3: 2000
      bar3: 2001
      baz3: 2002
    bar2:
      foo3: 2010
      bar3: 2011
      baz3: 2012
    baz2:
      foo3: 2020
      bar3: 2021
      baz3: 2022
  bar1:
    foo2:
      foo3: 2100
      bar3: 2101
      baz3: 2102
    bar2:
      foo3: 2110
      bar3: 2111
      baz3: 2112
    baz2:
      foo3: 2120
      bar3: 2121
      baz3: 2122
  baz1:
    foo2:
      foo3: 2200
      bar3: 2201
      baz3: 2202
    bar2:
      foo3: 2210
      bar3: 2211
      baz3: 2212
    baz2:
      foo3: 2220
      bar3: 2221
      baz3: 2222
)",
    L{
      N("foo0", L{
         N("foo1", L{N("foo2", L{N("foo3", "0000"), N("bar3", "0001"), N("baz3", "0002")}),  N("bar2", L{N("foo3", "0010"), N("bar3", "0011"), N("baz3", "0012")}),  N("baz2", L{N("foo3", "0020"), N("bar3", "0021"), N("baz3", "0022")})}),
         N("bar1", L{N("foo2", L{N("foo3", "0100"), N("bar3", "0101"), N("baz3", "0102")}),  N("bar2", L{N("foo3", "0110"), N("bar3", "0111"), N("baz3", "0112")}),  N("baz2", L{N("foo3", "0120"), N("bar3", "0121"), N("baz3", "0122")})}),
         N("baz1", L{N("foo2", L{N("foo3", "0200"), N("bar3", "0201"), N("baz3", "0202")}),  N("bar2", L{N("foo3", "0210"), N("bar3", "0211"), N("baz3", "0212")}),  N("baz2", L{N("foo3", "0220"), N("bar3", "0221"), N("baz3", "0222")})}),
      }),
      N("bar0", L{
         N("foo1", L{N("foo2", L{N("foo3", "1000"), N("bar3", "1001"), N("baz3", "1002")}),  N("bar2", L{N("foo3", "1010"), N("bar3", "1011"), N("baz3", "1012")}),  N("baz2", L{N("foo3", "1020"), N("bar3", "1021"), N("baz3", "1022")})}),
         N("bar1", L{N("foo2", L{N("foo3", "1100"), N("bar3", "1101"), N("baz3", "1102")}),  N("bar2", L{N("foo3", "1110"), N("bar3", "1111"), N("baz3", "1112")}),  N("baz2", L{N("foo3", "1120"), N("bar3", "1121"), N("baz3", "1122")})}),
         N("baz1", L{N("foo2", L{N("foo3", "1200"), N("bar3", "1201"), N("baz3", "1202")}),  N("bar2", L{N("foo3", "1210"), N("bar3", "1211"), N("baz3", "1212")}),  N("baz2", L{N("foo3", "1220"), N("bar3", "1221"), N("baz3", "1222")})}),
      }),
      N("baz0", L{
         N("foo1", L{N("foo2", L{N("foo3", "2000"), N("bar3", "2001"), N("baz3", "2002")}),  N("bar2", L{N("foo3", "2010"), N("bar3", "2011"), N("baz3", "2012")}),  N("baz2", L{N("foo3", "2020"), N("bar3", "2021"), N("baz3", "2022")})}),
         N("bar1", L{N("foo2", L{N("foo3", "2100"), N("bar3", "2101"), N("baz3", "2102")}),  N("bar2", L{N("foo3", "2110"), N("bar3", "2111"), N("baz3", "2112")}),  N("baz2", L{N("foo3", "2120"), N("bar3", "2121"), N("baz3", "2122")})}),
         N("baz1", L{N("foo2", L{N("foo3", "2200"), N("bar3", "2201"), N("baz3", "2202")}),  N("bar2", L{N("foo3", "2210"), N("bar3", "2211"), N("baz3", "2212")}),  N("baz2", L{N("foo3", "2220"), N("bar3", "2221"), N("baz3", "2222")})}),
      })
    }
),

//-----------------------------------------------------------------------------

C("empty seq, explicit",
"[]",
    SEQ
),

C("empty seq, multiline",
R"([
]
)",
    SEQ
),

C("empty seq, multilines",
R"([
# ksjdfkjhsdfkjhsdfkjh


]
)",
    SEQ
),


//-----------------------------------------------------------------------------

C("simple seq",
R"(- 0
- 1
- 2
- 3
)",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, explicit, single line",
"[0, 1, 2, 3]",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, explicit, multiline, unindented",
R"([
0,
1,
2,
3
])",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, explicit, multiline, comments inline",
R"([
0,  # bla0
1,  # bla1
2,  # bla2
3   # bla3
])",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, explicit, multiline, comments prev line",
R"([
# bla0
0,
# bla1
1,
# bla2
2,
# bla3
3
])",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, explicit, multiline, indented",
R"([
  0,
  1,
  2,
  3
])",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, comments inline",
R"(
- 0   # this is a foo
- 1   # this is a bar
- 2   # this is a bar
- 3   # this is a bar
)",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, comments prev line",
R"(
# this is a foo
- 0
# this is a bar
- 1
# this is a baz
- 2
# this is a bat
- 3
)",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

//-----------------------------------------------------------------------------

C("nested seq x2, empty, oneline",
R"([[], [], []])",
    L{SEQ, SEQ, SEQ}
),

C("nested seq x2, explicit, same line",
R"([[00, 01, 02], [10, 11, 12], [20, 21, 22]])",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
),


//-----------------------------------------------------------------------------

C("nested seq x2, explicit first+last level, same line, no spaces",
R"([[00,01,02],[10,11,12],[20,21,22]])",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
),


//-----------------------------------------------------------------------------

C("nested seq x2, explicit",
R"([
[00, 01, 02],
[10, 11, 12],
[20, 21, 22],
])",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
),

//-----------------------------------------------------------------------------

C("nested seq x2",
R"(
- - 00
  - 01
  - 02
- - 10
  - 11
  - 12
- - 20
  - 21
  - 22
)",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
),

C("nested seq x2, implicit first, explicit last level",
R"(
- [00, 01, 02]
- [10, 11, 12]
- [20, 21, 22]
)",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
),

//-----------------------------------------------------------------------------
C("nested seq x3, explicit",
R"([
[[000, 001, 002], [010, 011, 012], [020, 021, 022]],
[[100, 101, 102], [110, 111, 112], [120, 121, 122]],
[[200, 201, 202], [210, 211, 212], [220, 221, 222]],
])",
    L{
      N{L{N{L{N{"000"}, N{"001"}, N{"002"}}}, N{L{N{"010"}, N{"011"}, N{"012"}}}, N{L{N{"020"}, N{"021"}, N{"022"}}}}},
      N{L{N{L{N{"100"}, N{"101"}, N{"102"}}}, N{L{N{"110"}, N{"111"}, N{"112"}}}, N{L{N{"120"}, N{"121"}, N{"122"}}}}},
      N{L{N{L{N{"200"}, N{"201"}, N{"202"}}}, N{L{N{"210"}, N{"211"}, N{"212"}}}, N{L{N{"220"}, N{"221"}, N{"222"}}}}},
          }
),

C("nested seq x3",
R"(
- - - 000
    - 001
    - 002
  - - 010
    - 011
    - 012
  - - 020
    - 021
    - 022
- - - 100
    - 101
    - 102
  - - 110
    - 111
    - 112
  - - 120
    - 121
    - 122
- - - 200
    - 201
    - 202
  - - 210
    - 211
    - 212
  - - 220
    - 221
    - 222
)",
    L{
      N{L{N{L{N{"000"}, N{"001"}, N{"002"}}}, N{L{N{"010"}, N{"011"}, N{"012"}}}, N{L{N{"020"}, N{"021"}, N{"022"}}}}},
      N{L{N{L{N{"100"}, N{"101"}, N{"102"}}}, N{L{N{"110"}, N{"111"}, N{"112"}}}, N{L{N{"120"}, N{"121"}, N{"122"}}}}},
      N{L{N{L{N{"200"}, N{"201"}, N{"202"}}}, N{L{N{"210"}, N{"211"}, N{"212"}}}, N{L{N{"220"}, N{"221"}, N{"222"}}}}},
          }
),


C("nested seq x4, explicit",
R"([
[[[0000, 0001, 0002], [0010, 0011, 0012], [0020, 0021, 0022]],
 [[0100, 0101, 0102], [0110, 0111, 0112], [0120, 0121, 0122]],
 [[0200, 0201, 0202], [0210, 0211, 0212], [0220, 0221, 0222]]],

[[[1000, 1001, 1002], [1010, 1011, 1012], [1020, 1021, 1022]],
 [[1100, 1101, 1102], [1110, 1111, 1112], [1120, 1121, 1122]],
 [[1200, 1201, 1202], [1210, 1211, 1212], [1220, 1221, 1222]]],

[[[2000, 2001, 2002], [2010, 2011, 2012], [2020, 2021, 2022]],
 [[2100, 2101, 2102], [2110, 2111, 2112], [2120, 2121, 2122]],
 [[2200, 2201, 2202], [2210, 2211, 2212], [2220, 2221, 2222]]],
])",
    L{
      N{L{N{L{N{L{N{"0000"}, N{"0001"}, N{"0002"}}}, N{L{N{"0010"}, N{"0011"}, N{"0012"}}}, N{L{N{"0020"}, N{"0021"}, N{"0022"}}}}}, N{L{N{L{N{"0100"}, N{"0101"}, N{"0102"}}}, N{L{N{"0110"}, N{"0111"}, N{"0112"}}}, N{L{N{"0120"}, N{"0121"}, N{"0122"}}}}}, N{L{N{L{N{"0200"}, N{"0201"}, N{"0202"}}}, N{L{N{"0210"}, N{"0211"}, N{"0212"}}}, N{L{N{"0220"}, N{"0221"}, N{"0222"}}}}}}},
      N{L{N{L{N{L{N{"1000"}, N{"1001"}, N{"1002"}}}, N{L{N{"1010"}, N{"1011"}, N{"1012"}}}, N{L{N{"1020"}, N{"1021"}, N{"1022"}}}}}, N{L{N{L{N{"1100"}, N{"1101"}, N{"1102"}}}, N{L{N{"1110"}, N{"1111"}, N{"1112"}}}, N{L{N{"1120"}, N{"1121"}, N{"1122"}}}}}, N{L{N{L{N{"1200"}, N{"1201"}, N{"1202"}}}, N{L{N{"1210"}, N{"1211"}, N{"1212"}}}, N{L{N{"1220"}, N{"1221"}, N{"1222"}}}}}}},
      N{L{N{L{N{L{N{"2000"}, N{"2001"}, N{"2002"}}}, N{L{N{"2010"}, N{"2011"}, N{"2012"}}}, N{L{N{"2020"}, N{"2021"}, N{"2022"}}}}}, N{L{N{L{N{"2100"}, N{"2101"}, N{"2102"}}}, N{L{N{"2110"}, N{"2111"}, N{"2112"}}}, N{L{N{"2120"}, N{"2121"}, N{"2122"}}}}}, N{L{N{L{N{"2200"}, N{"2201"}, N{"2202"}}}, N{L{N{"2210"}, N{"2211"}, N{"2212"}}}, N{L{N{"2220"}, N{"2221"}, N{"2222"}}}}}}},
          }
),

C("nested seq x4",
R"(
- - - - 0000
      - 0001
      - 0002
    - - 0010
      - 0011
      - 0012
    - - 0020
      - 0021
      - 0022
  - - - 0100
      - 0101
      - 0102
    - - 0110
      - 0111
      - 0112
    - - 0120
      - 0121
      - 0122
  - - - 0200
      - 0201
      - 0202
    - - 0210
      - 0211
      - 0212
    - - 0220
      - 0221
      - 0222
- - - - 1000
      - 1001
      - 1002
    - - 1010
      - 1011
      - 1012
    - - 1020
      - 1021
      - 1022
  - - - 1100
      - 1101
      - 1102
    - - 1110
      - 1111
      - 1112
    - - 1120
      - 1121
      - 1122
  - - - 1200
      - 1201
      - 1202
    - - 1210
      - 1211
      - 1212
    - - 1220
      - 1221
      - 1222
- - - - 2000
      - 2001
      - 2002
    - - 2010
      - 2011
      - 2012
    - - 2020
      - 2021
      - 2022
  - - - 2100
      - 2101
      - 2102
    - - 2110
      - 2111
      - 2112
    - - 2120
      - 2121
      - 2122
  - - - 2200
      - 2201
      - 2202
    - - 2210
      - 2211
      - 2212
    - - 2220
      - 2221
      - 2222
)",
    L{
      N{L{N{L{N{L{N{"0000"}, N{"0001"}, N{"0002"}}}, N{L{N{"0010"}, N{"0011"}, N{"0012"}}}, N{L{N{"0020"}, N{"0021"}, N{"0022"}}}}}, N{L{N{L{N{"0100"}, N{"0101"}, N{"0102"}}}, N{L{N{"0110"}, N{"0111"}, N{"0112"}}}, N{L{N{"0120"}, N{"0121"}, N{"0122"}}}}}, N{L{N{L{N{"0200"}, N{"0201"}, N{"0202"}}}, N{L{N{"0210"}, N{"0211"}, N{"0212"}}}, N{L{N{"0220"}, N{"0221"}, N{"0222"}}}}}}},
      N{L{N{L{N{L{N{"1000"}, N{"1001"}, N{"1002"}}}, N{L{N{"1010"}, N{"1011"}, N{"1012"}}}, N{L{N{"1020"}, N{"1021"}, N{"1022"}}}}}, N{L{N{L{N{"1100"}, N{"1101"}, N{"1102"}}}, N{L{N{"1110"}, N{"1111"}, N{"1112"}}}, N{L{N{"1120"}, N{"1121"}, N{"1122"}}}}}, N{L{N{L{N{"1200"}, N{"1201"}, N{"1202"}}}, N{L{N{"1210"}, N{"1211"}, N{"1212"}}}, N{L{N{"1220"}, N{"1221"}, N{"1222"}}}}}}},
      N{L{N{L{N{L{N{"2000"}, N{"2001"}, N{"2002"}}}, N{L{N{"2010"}, N{"2011"}, N{"2012"}}}, N{L{N{"2020"}, N{"2021"}, N{"2022"}}}}}, N{L{N{L{N{"2100"}, N{"2101"}, N{"2102"}}}, N{L{N{"2110"}, N{"2111"}, N{"2112"}}}, N{L{N{"2120"}, N{"2121"}, N{"2122"}}}}}, N{L{N{L{N{"2200"}, N{"2201"}, N{"2202"}}}, N{L{N{"2210"}, N{"2211"}, N{"2212"}}}, N{L{N{"2220"}, N{"2221"}, N{"2222"}}}}}}},
          }
),


//-----------------------------------------------------------------------------

C("map of empty seqs",
R"({foo: [], bar: [], baz: []})",
     L{
         N(KEYSEQ, "foo", L()),
         N(KEYSEQ, "bar", L()),
         N(KEYSEQ, "baz", L()),
     }
),

C("map of seqs, one line",
R"({men: [John Smith, Bill Jones], women: [Mary Smith, Susan Williams]})",
     L{
         N("men", L{N{"John Smith"}, N{"Bill Jones"}}),
         N("women", L{N{"Mary Smith"}, N{"Susan Williams"}})
     }
),

C("map of seqs",
R"(
men:
  - John Smith
  - Bill Jones
women:
  - Mary Smith
  - Susan Williams
)",
     L{
         N("men", L{N{"John Smith"}, N{"Bill Jones"}}),
         N("women", L{N{"Mary Smith"}, N{"Susan Williams"}})
     }
),

//-----------------------------------------------------------------------------

C("seq of empty maps, one line",
R"([{}, {}, {}])",
  L{MAP, MAP, MAP}
),

C("seq of maps, one line",
R"([{name: John Smith, age: 33}, {name: Mary Smith, age: 27}])",
  L{
      N{L{N("name", "John Smith"), N("age", "33")}},
      N{L{N("name", "Mary Smith"), N("age", "27")}}
  }
),

C("seq of maps, implicit seq, explicit maps",
R"(
- {name: John Smith, age: 33}
- {name: Mary Smith, age: 27}
)",
  L{
      N{L{N("name", "John Smith"), N("age", "33")}},
      N{L{N("name", "Mary Smith"), N("age", "27")}}
  }
),

C("seq of maps",
R"(
- name: John Smith
  age: 33
- name: Mary Smith
  age: 27
)",
  L{
      N{L{N("name", "John Smith"), N("age", "33")}},
      N{L{N("name", "Mary Smith"), N("age", "27")}}
  }
),

#ifdef JAVAI
//-----------------------------------------------------------------------------
// https://en.wikipedia.org/wiki/YAML

C("simple seq",
R"(- Casablanca
- North by Northwest
- The Man Who Wasn't There
)",
    L{N{"Casablanca"}, N{"North by Northwest"}, N{"The Man Who Wasn't There"}}
),

C("simple seq in a doc",
R"(--- # Favorite movies
- Casablanca
- North by Northwest
- The Man Who Wasn't There
)",
    N{DOC, {N{"Casablanca"}, N{"North by Northwest"}, N{"The Man Who Wasn't There"}}}
),

C("simple seq in a doc with explicit termination",
R"(--- # Favorite movies
- Casablanca
- North by Northwest
- The Man Who Wasn't There
...
)",
    N{DOC, {N{"Casablanca"}, N{"North by Northwest"}, N{"The Man Who Wasn't There"}}}
),

//-----------------------------------------------------------------------------
C("explicit seq",
R"([milk, pumpkin pie, eggs, juice])",
  L{N{"milk"}, N{"pumpkin pie"}, N{"eggs"}, N{"juice"}}
),

C("explicit seq in a doc",
R"(--- # Shopping list
[milk, pumpkin pie, eggs, juice]
)",
  N{DOC, {N{"milk"}, N{"pumpkin pie"}, N{"eggs"}, N{"juice"}}}
),

//-----------------------------------------------------------------------------
C("simple map",
R"(
name: John Smith
age: 33
)",
  L{
      N{DOC, L{N("name", "John Smith"), N("age", "33")}},
      N{DOC, L{N("name", "John Smith"), N("age", "33")}},
  }
),

//-----------------------------------------------------------------------------
C("simple map, nested",
R"(
  name: John Smith
  age: 33
)",
  L{
      N{DOC, L{N("name", "John Smith"), N("age", "33")}},
      N{DOC, L{N("name", "John Smith"), N("age", "33")}},
  }
),

//-----------------------------------------------------------------------------
C("two docs with the same map",
R"(--- # Indented Block
  name: John Smith
  age: 33
--- # Inline Block
{name: John Smith, age: 33}
)",
  L{
      N{DOC, L{N("name", "John Smith"), N("age", "33")}},
      N{DOC, L{N("name", "John Smith"), N("age", "33")}},
  }
),


//-----------------------------------------------------------------------------
C("anchor example",
R"(
---
receipt:     Oz-Ware Purchase Invoice
date:        2012-08-06
customer:
    first_name:   Dorothy
    family_name:  Gale

items:
    - part_no:   A4786
      descrip:   Water Bucket (Filled)
      price:     1.47
      quantity:  4

    - part_no:   E1628
      descrip:   High Heeled \"Ruby\" Slippers
      size:      8
      price:     133.7
      quantity:  1

bill-to:  &id001
    street: |
            123 Tornado Alley
            Suite 16
    city:   East Centerville
    state:  KS

ship-to:  *id001

specialDelivery:  >
    Follow the Yellow Brick
    Road to the Emerald City.
    Pay no attention to themR
    man behind the curtain.
...
)",
     L{N{DOC, L{
N{"receipt", "Oz-Ware Purchase Invoice"},
N{"date", "2012-08-06"},
N{"customer", L{N{"first_name", "Dorothy"}, N{"family_name", "Gale"}}},
N{"items", L{
        N{L{N{"part_no", "A4786"},
          N{"descrip",   "Water Bucket (Filled)"},
          N{"price",     "1.47"},
          N{"quantity",  "4"},}},
        N{L{N{"part_no", "E1628"},
          N{"descrip",   "High Heeled \"Ruby\" Slippers"},
          N{"size",     "8"},
          N{"price",     "133.7"},
          N{"quantity",  "1"},}}}},
N{"bill-to", L{
        N{"street", "123 Tornado Alley\nSuite 16\n"},
        N{"city", "East Centerville"},
        N{"state", "KS"},}},
N{"ship-to", L{
        N{"street", "123 Tornado Alley\nSuite 16\n"},
        N{"city", "East Centerville"},
        N{"state", "KS"},}},
N{"specialDelivery", "Follow the Yellow Brick Road to the Emerald City. Pay no attention to the man behind the curtain.\n"}
    }}}
),


//-----------------------------------------------------------------------------
C("anchor example, 2",
R"(
# sequencer protocols for Laser eye surgery
---
- step:  &id001                  # defines anchor label &id001
    instrument:      Lasik 2000
    pulseEnergy:     5.4
    pulseDuration:   12
    repetition:      1000
    spotSize:        1mm

- step: &id002
    instrument:      Lasik 2000
    pulseEnergy:     5.0
    pulseDuration:   10
    repetition:      500
    spotSize:        2mm
- step: *id001                   # refers to the first step (with anchor &id001)
- step: *id002                   # refers to the second step
- step:
    <<: *id001
    spotSize: 2mm                # redefines just this key, refers rest from &id001
- step: *id002
)",
     L{N{DOC, L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.4"},
    N{"pulseDuration",   "12"},
    N{"repetition",      "1000"},
    N{"spotSize",        "1mm"},
        }},
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.0"},
    N{"pulseDuration",   "10"},
    N{"repetition",      "500"},
    N{"spotSize",        "2mm"},
        }},
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.4"},
    N{"pulseDuration",   "12"},
    N{"repetition",      "1000"},
    N{"spotSize",        "1mm"},
        }},
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.0"},
    N{"pulseDuration",   "10"},
    N{"repetition",      "500"},
    N{"spotSize",        "2mm"},
        }},
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.4"},
    N{"pulseDuration",   "12"},
    N{"repetition",      "1000"},
    N{"spotSize",        "2mm"},
        }},
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.0"},
    N{"pulseDuration",   "10"},
    N{"repetition",      "500"},
    N{"spotSize",        "2mm"},
        }},
         }
     }}
),


//-----------------------------------------------------------------------------
C("literal block scalar as map entry",
R"(
data: |
   There once was a short man from Ealing
   Who got on a bus to Darjeeling
       It said on the door
       \"Please don't spit on the floor\"
   So he carefully spat on the ceiling
)",
     N{"data", "There once was a short man from Ealing\nWho got on a bus to Darjeeling\n    It said on the door\n    \"Please don't spit on the floor\"\nSo he carefully spat on the ceiling\n"}
),

//-----------------------------------------------------------------------------
C("folded block scalar as map entry",
R"(
data: >
   Wrapped text
   will be folded
   into a single
   paragraph

   Blank lines denote
   paragraph breaks
)",
  N{"data", "Wrapped text will be folded into a single paragraph\nBlank lines denote paragraph breaks\n"}
),

//-----------------------------------------------------------------------------
C("two scalars in a block, html example",
R"(
---
example: >
        HTML goes into YAML without modification
message: |
        <blockquote style=\"font: italic 12pt Times\">
        <p>\"Three is always greater than two,
           even for large values of two\"</p>
        <p>--Author Unknown</p>
        </blockquote>
date: 2007-06-01
)",
     N{DOC, L{
          N{"example", "HTML goes into YAML without modification"},
          N{"message", R"(<blockquote style=\"font: italic 12pt Times\">
<p>\"Three is always greater than two,
   even for large values of two\"</p>
<p>--Author Unknown</p>
</blockquote>
)"},
          N{"date","2007-06-01"},
              }}
),



//-----------------------------------------------------------------------------
C("scalar block, literal, no chomp, no indentation",
R"(example: |
  Several lines of text,
  with some \"quotes\" of various 'types',
  and also a blank line:

  plus another line at the end.

another: text
)",
     L{
      N{"example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"},
      N{"another", "text"},
          }
),

//-----------------------------------------------------------------------------
C("scalar block, folded, no chomp, no indentation",
R"(example: >
  Several lines of text,
  with some \"quotes\" of various 'types',
  and also a blank line:

  plus another line at the end.

another: text
)",
     L{
      N{"example", "Several lines of text,  with some \"quotes\" of various 'types',  and also a blank line:\nplus another line at the end.\n"},
      N{"another", "text"},
          }
),

#endif
    }); // end examples


    return tests.run();
}


#ifdef MORE_EXAMPLES
char const* const structure_examples[] = {
// https://en.wikipedia.org/wiki/YAML
R"(
---
a: 123                     # an integer
b: \"123\"                   # a string, disambiguated by quotes
c: 123.0                   # a float
d: !!float 123             # also a float via explicit data type prefixed by (!!)
e: !!str 123               # a string, disambiguated by explicit type
f: !!str Yes               # a string via explicit type
g: Yes                     # a boolean True (yaml1.1), string \"Yes\" (yaml1.2)
h: Yes we have No bananas  # a string, \"Yes\" and \"No\" disambiguated by context.
)",
R"(
---
picture: !!binary |
 R0lGODdhDQAIAIAAAAAAANn
 Z2SwAAAAADQAIAAACF4SDGQ
 ar3xxbJ9p0qa7R0YxwzaFME
 1IAADs=
)",
R"(
---
myObject:  !myClass { name: Joe, age: 15 }
)",
R"(

)",
R"(
)",
R"(
)",
R"(
)",
R"(
)",
};
#endif
