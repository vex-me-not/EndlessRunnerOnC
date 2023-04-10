
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////                                                                                         /////////////////////////////////////
///////////////// Υλοποιηση των συναρτησεων της sets_util.h                                               //////////////////////////////////////
/////////////////                                                                                        ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <stdlib.h>
#include <assert.h>

#include "sets_util.h"

Pointer set_find_eq_or_greater(Set set, Pointer value){
    assert(value!=NULL);

    Pointer greater=malloc(sizeof(*greater));

    if(set_find_node(set,value) != SET_EOF){        ////To στοιχεια υπαρχει
        greater=value;                            ////To επιστρεφουμε
    }else{                                       ////To στοιχειο δεν υπαρχει
        set_insert(set,value);                  ///Αρα το εισαγουμε
       
        if(set_next(set,set_find_node(set,value)) != SET_EOF){       ////Το στοιχειο δεν μπηκε στο τελος
            greater=set_node_value(set,set_next(set,set_find_node(set,value)));       ////Αρα εχει μεγαλυτερο
        }else{                 ////Το στοιχειο μπηκε στο τελος
            greater=NULL;     ////Αρα δεν εχει μεγαλυτερο
        }
        
        assert(set_remove(set,value)==true);      ////Το αφαιρουμε

    }

    return greater;           ////Επιστρεφουμε το στοιχειο που βρηκαμε ή NULL αν δεν βρηκαμε τιποτα
    
}

Pointer set_find_eq_or_smaller(Set set, Pointer value){
    assert(value!=NULL);

    Pointer smaller=malloc(sizeof(*smaller));

    if(set_find_node(set,value) != SET_EOF){        ////To στοιχειο υπαρχει
        smaller=value;                            ////To επιστρεφουμε
    }else{                                       ////To στοιχειο δεν υπαρχει
        set_insert(set,value);                  ///Αρα το εισαγουμε
       
        if(set_previous(set,set_find_node(set,value)) != SET_BOF){       ////Το στοιχειο δεν μπηκε στην αρχη
            smaller=set_node_value(set,set_previous(set,set_find_node(set,value)));       ////Αρα εχει μικροτερο
        }else{                 ////Το στοιχειο μπηκε στην αρχη
            smaller=NULL;     ////Αρα δεν εχει μικροτερο
        }
        
        assert(set_remove(set,value)==true);      ////Το αφαιρουμε

    }

    return smaller;           ////Επιστρεφουμε το στοιχειο που βρηκαμε ή NULL αν δεν βρηκαμε τιποτα
}