//
//  main.cpp
//  proj4
//
//  Created by Leslie Castelan on 6/1/21.
//

#include <iostream>
#include "Table.h"
#include <cassert>
using namespace std;


void test1()
{
    vector<string> cols = { "N", "Z" };
    Table t("Z", cols);
    assert(t.good());
    assert(t.insert("UCLA 90095"));
    assert(t.insert("Caltech 91125"));
    vector<vector<string>> v;
    t.find("90095", v);
    assert(v.size() == 1);
    assert(v[0][0] == "UCLA"  &&  v[0][1] == "90095");
    assert(t.select("Z > 90210", v) == 0);
    assert(v.size() == 1);
    assert(v[0][0] == "Caltech"  &&  v[0][1] == "91125");
    cout << "DONE" << endl;
}
 
void test2()
{
    vector<string> cols = { "N", "Z" };
    Table t("Z", cols);
    assert(t.good());
    assert(t.insert("UCLA 90095"));
    assert(t.insert("Caltech 91125"));
    assert(t.insert("Caltech 91125"));
    vector<vector<string>> v;
    t.find("91125", v);
    assert(v.size() == 2);
    assert(t.select("Z > 90210", v) == 0);
    assert(v.size() == 2);
    assert(t.insert("  Berk 333K"));
    assert(t.insert("  UCR 3X33"));
    assert(t.select("Z LT 95210", v) == 2); //two badly formed queries of Berk 33k and UCR 3x33
    assert(v.size() == 3);
    assert(t.select("Z GT 95210", v) == 2);
    assert(v.size() == 0);
    assert(t.select("Z EQ 95210", v) == 2);     //UCI 95210 => hash(95210) => hash_table[hash_index]
    assert(v.size() == 0);
    assert(t.select("Z EQ 333K", v) == -1);
    assert(t.select("Z EQ 91125", v) == 2);
    assert(v.size() == 2);
    assert(t.select("Z EQ 91125", v) == 2);
    assert(t.select("Z NE 91125", v) == 2);
    assert(v.size() == 1);
    assert(t.insert("UCI .333"));
    assert(t.select("Z GE .333", v) == 2);
    assert(v.size() == 4);
    assert(t.select("Z LE 99999", v) == 2);
    assert(v.size() == 4);
    assert(t.select("N < Z", v) == 0);
    assert(v.size() == 6);
    assert(t.select("N = U", v) == 0);
    assert(v.size() == 0);
    assert(t.select("N == U", v) == 0);
    assert(v.size() == 0);
    assert(t.select("N == UCR", v) == 0);
    assert(v.size() == 1);
}
//Some are repeats from the spec that I added onto
void testConsInsert() {
    vector<string> cols = {
        "customer", "product", "price", "location"
    };
    vector<string> bad1 = {
        "customer", "location", "price", "location"
    };
    vector<string> bad2 = {
        "", "customer"
    };
    vector<string> bad3 = {
        ""
    };
    vector<string> bad4 = {
        "customer", "customer", "price", "location", ""
    };
    Table t("customer", cols);
    assert(t.insert("Patel 12345 42.54 Westwood"));
    assert(t.insert("O'Reilly 34567     4.99 Westwood   "));
    assert(t.insert("Patel\t67890\t142.75  \t \t\t  \tHollywood"));
    assert(t.insert("O'Reilly 34567     4.99 Westwood   "));
    assert(t.insert("   Hoang  12345 30.46 'Santa Monica' "));
    assert( ! t.insert("Figueroa 54321 59.95"));
    Table t1("product", cols);
    Table t2("price", cols);
    Table t3("location", cols);
    Table s("customers", cols);
    Table p("customer", bad1);
    Table x("customer", bad2);
    Table y("customer", bad4);
    assert(t.good());
    assert(t1.good());
    assert(t2.good());
    assert(t3.good());
    assert(!s.good());
    assert(!p.good());
    assert(!x.good());
    assert(!y.good());
 
}

void testFind() {
    vector<string> cols = {
        "customer", "product", "price", "location"
    };
    Table t("customer", cols);
    assert(t.insert("Patel 12345 42.54 Westwood"));
    assert(t.insert("O'Reilly 34567     4.99 Westwood   "));
    assert(t.insert("Patel\t67890\t142.75  \t \t\t  \tHollywood"));
    assert(t.insert("O'Reilly 34567     4.99 Westwood   "));
    assert(t.insert("   Hoang  12345 30.46 'Santa Monica' "));
    assert( ! t.insert("Figueroa 54321 59.95"));
    vector<vector<string>> v;
    t.find("Patel", v);
    assert(v.size() == 2);
    vector<vector<string>> expected = {
        { "Patel", "12345", "42.54", "Westwood" },
        { "Patel", "67890", "142.75", "Hollywood" }
    };
    assert((v[0] == expected[0]  &&  v[1] == expected[1])  ||
           (v[0] == expected[1]  &&  v[1] == expected[0]) );
}


void testSel() {
    vector<string> cols = {
        "customer", "product", "price", "location"
    };
    Table t("customer", cols);
    assert(t.insert("Patel 12345 42.54 Westwood"));
    assert(t.insert("O'Reilly 34567     4.99 Westwood   "));
    assert(t.insert("Patel\t67890\t142.75  \t \t\t  \tHollywood"));
    assert(t.insert("O'Reilly 34567     4.99 Westwood   "));
    assert(t.insert("   Hoang  12345 30.46 'Santa Monica' "));
    assert( ! t.insert("Figueroa 54321 59.95"));
    
    vector<vector<string>> v;
    assert(t.select("location < Westwood", v) == 0);
    assert(v.size() == 2);
    vector<vector<string>> expected = {
        { "Hoang", "12345", "30.46", "Santa Monica" },
        { "Patel", "67890", "142.75", "Hollywood" }
    };
    assert((v[0] == expected[0]  &&  v[1] == expected[1])  ||
           (v[0] == expected[1]  &&  v[1] == expected[0]) );
}

void testSelSimp() {
    vector<string> cols = { "item name", "price" };
    Table t("item name", cols);
    assert(t.good());
    assert(t.insert("'chocolate bar' 1.69"));
    assert(t.insert("coffee 7.99"));
    assert(t.insert("hummus 3.49"));
    vector<vector<string>> v;
    assert(t.select("price LT 5", v) == 0);
    assert(v.size() == 2);  // chocolate bar and hummus
    assert(t.insert("pretzels 1.W9"));
    assert(t.select("price LT H", v) == -1);
    assert(v.size() == 0);
    assert(t.insert("hummus 4.49"));
    assert(t.insert("hummus 4.p9"));
    assert(t.insert("hummus 4.09"));
    assert(t.insert("hummus 4.99"));
    assert(t.insert("chum 4.p9"));
    assert(t.insert("chum 4.99"));
    assert(t.insert("zzzzzzzzzzzzzzzzzz 4.99"));
    assert(t.select("price LT 5", v) == 3);  // 1 because pretzels 1.W9
    //t.print();
    //std::cout << v.size() << std::endl;
    assert(v.size() == 7);  // chocolate bar and hummus
}
 
void FinalTest() {
    vector<string> cols = { "N", "Z" };
    Table t("Z", cols);
    assert(t.good());
    assert(t.insert("UCLA 90095"));
    assert(t.insert("Caltech 91125"));
    vector<vector<string>> v;
    t.find("90095", v);
    assert(v.size() == 1);
    assert(v[0][0] == "UCLA"  &&  v[0][1] == "90095");
    assert(t.select("Z > 90210", v) == 0);
    assert(v.size() == 1);
    assert(v[0][0] == "Caltech"  &&  v[0][1] == "91125");
 
    vector<string> uclacols = { "last name", "first name", "major", "UID", "GPA" };
    Table ucla("UID", uclacols);
    assert(ucla.good());
    assert(ucla.insert("Bruin Jose 'COG SCI' 304567890 3.4"));
    assert(ucla.insert("Bruin Josephine 'COM SCI' 605432109 3.8"));
    assert(ucla.insert("Trojan Tommy LOSING 000000000 1.7"));
      // Troy brought a wooden horse full of Greek soldiers inside the
      // city walls and lost the Trojan War.  How strange to look up to
      // gullible losers as role models.
    assert(ucla.select("GPA GE 3.2", v) == 0);
    assert(v.size() == 2);
    assert(v[0][0] == v[1][0]  &&  v[0][1] != v[1][1]);
}

 

int main() {
    
    test1();
    test2();
    testConsInsert();
    testFind();
    testSelSimp();
   
    FinalTest();
    
    vector<string> cols = { "N", "Z" };
           Table t("Z", cols);
           assert(t.good());
           assert(t.insert("UCLA 90095"));
           assert(t.insert("Caltech 91125"));
           vector<vector<string>> v;
           t.find("90095", v);
           assert(v.size() == 1);
           assert(v[0][0] == "UCLA"  &&  v[0][1] == "90095");
           assert(t.select("Z > 90210", v) == 0);
           assert(v.size() == 1);
           assert(v[0][0] == "Caltech"  &&  v[0][1] == "91125");

           vector<string> uclacols = { "last name", "first name", "major", "UID", "GPA" };
           Table ucla("UID", uclacols);
           assert(ucla.good());
           assert(ucla.insert("Bruin Jose 'COG SCI' 304567890 3.4"));
           assert(ucla.insert("Bruin Josephine 'COM SCI' 605432109 3.8"));
           assert(ucla.insert("Trojan Tommy LOSING 000000000 1.7"));
             // Troy brought a wooden horse full of Greek soldiers inside the
             // city walls and lost the Trojan War.  How strange to look up to
             // gullible losers as role models.
           assert(ucla.select("GPA GE 3.2", v) == 0);
           assert(v.size() == 2);
           assert(v[0][0] == v[1][0]  &&  v[0][1] != v[1][1]);

           cout << "DONE" << endl;

    

    
    return 0;
}
