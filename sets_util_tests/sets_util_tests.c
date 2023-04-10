////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////        Test για το sets_util.h module                                                     /////////////////////////////////
///////////////                                                                                            /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing
#include "sets_util.h"
#include "state.h"


int compare_ints(Pointer a, Pointer b) {          //Copy paste απο το εργαστηριο.Θα χρησιμοποιηθει μονο εντος του τεστ
    int* ia = a;
    int* ib = b;
   // printf("comp int\n");
    return *ia - *ib;       // αρνητικός, μηδέν ή θετικός, ανάλογα με τη διάταξη των a,b
}

int* create_int(int value) {
	int* pointer = malloc(sizeof(int));		// δέσμευση μνήμης
	*pointer = value;						// αναθεση τιμης 
	return pointer;
}



void test_eq_or_greater(){
    Set s=set_create(compare_ints,NULL);

    set_insert(s,create_int(5));
    set_insert(s,create_int(10));
    set_insert(s,create_int(15));
    
    int* i=set_find_eq_or_greater(s,create_int(15));       //////Ψαχνουμε να δουμε αν το 15 ειναι στο set
    TEST_ASSERT(*i==*create_int(15));                      /////Αφου ειναι αναμενουμε να το παρουμε πισω
    
    i=set_find_eq_or_greater(s,create_int(8));             /////Ψαχνουμε να δουμε αν το 8 ειναι στο set
    TEST_ASSERT(*i==*create_int(10));                       ////Αφου δεν ειναι αναμενουμε να παρουμε το αμεσως μεγαλυτερο στοιχειο που ειναι το 10
    
    i=set_find_eq_or_greater(s,create_int(0));              ////Ψαχνουμε να δουμε αν το 0 ειναι στο set
    TEST_ASSERT(*i==*create_int(5));                        ////Αφου δεν ειναι αναμενουμε να παρουμε το αμεσως μεγαλυτερο στοιχειο που ειναι το 5

    i=set_find_eq_or_greater(s,create_int(20));            ////Ψαχνουμε να δουμε αν το 20 ειναι στο set
    TEST_ASSERT(i==NULL);                                  ////Προφανως δεν ειναι και απο τη στιγμη που ειναι και μεγαλυτερο απο ολα τα στοιχεια του set πρεπει να παρουμε NULL

    
    assert(set_set_destroy_value(s,free)==NULL);
    set_destroy(s);


}


void test_eq_or_smaller(){
    Set s=set_create(compare_ints,NULL);

    set_insert(s,create_int(5));
    set_insert(s,create_int(10));
    set_insert(s,create_int(15));    

    int* i=set_find_eq_or_smaller(s,create_int(15));       //////Ψαχνουμε να δουμε αν το 15 ειναι στο set
    TEST_ASSERT(*i==*create_int(15));                      /////Αφου ειναι αναμενουμε να το παρουμε πισω

    i=set_find_eq_or_smaller(s,create_int(8));       //////Ψαχνουμε να δουμε αν το 8 ειναι στο set
    TEST_ASSERT(*i==*create_int(5));                      /////Αφου δεν ειναι αναμενουμε να το παρουμε πισω το 5

    i=set_find_eq_or_smaller(s,create_int(16));       //////Ψαχνουμε να δουμε αν το 16 ειναι στο set
    TEST_ASSERT(*i==*create_int(15));                      /////Αφου δεν ειναι αναμενουμε να το παρουμε πισω το 15
    
    i=set_find_eq_or_smaller(s,create_int(2));             /////Ψαχνουμε να δουμε αν το 2 ειναι στο set
    TEST_ASSERT(i==NULL);                            /////Απο τη στιγμη που το 2 ειναι μικροτερο απο ολα τα στοιχεια του set πρεπει να παρουμε πισω NULL

    assert(set_set_destroy_value(s,free)==NULL);
    set_destroy(s);
}



TEST_LIST = {
	{ "test_eq_or_greater", test_eq_or_greater },
	{ "test_eq_or_smaller", test_eq_or_smaller },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};