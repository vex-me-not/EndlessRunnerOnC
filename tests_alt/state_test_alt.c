


#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing
#include "state.h"


void test_state_create_alt(){
	State state = state_create();
	TEST_ASSERT(state != NULL);


	StateInfo info = state_info(state);
	TEST_ASSERT(info != NULL);

	TEST_ASSERT(info->current_portal == 0);
	TEST_ASSERT(info->wins == 0);
	TEST_ASSERT(info->playing);
	TEST_ASSERT(!info->paused);
	
	// Προσθέστε επιπλέον ελέγχους
	TEST_ASSERT(info->character->rect.x==0 && info->character->rect.y==-(info->character->rect.height));    //// Ελεχγουμε αν ο χαρακτηρας ειναι στην αρχη και στο δαπεδο
	
	TEST_ASSERT(state_objects(state,0.0,4.0*PORTAL_NUM*SPACING) != NULL);                                       ////// Test για να βεβαιωθουμε οτι δουλευει η state_objects. Θα πρεπει να εχουμε βρει 2 Objects
	TEST_ASSERT(list_size(state_objects(state,0.0,(4.0*PORTAL_NUM )*SPACING)) == 4*PORTAL_NUM);                        ///// Βεβαιωνομαστε οτι οντως υπαρχουν 4*PORTAL_NUM Objects
	

	TEST_ASSERT(list_size(state_objects(state,0.0,1400.0)) == 2);
	TEST_ASSERT(list_size(state_objects(state,-4.0*PORTAL_NUM*SPACING,-0.1))==0);                        /////Βεβαιωνομαστε οτι δεν υπαρχουν στοιχεια "αριστερα" της αφετηριας 
	
	////////Ελεγχουμε οτι ολα τα αντικειμενα βρισκονται στο πατωμα 
	ListNode ln = list_first(state_objects(state,0.0,(4.0*PORTAL_NUM)*SPACING));
	while(ln != LIST_EOF){                                                                      //////Διατρεχουμε ολα τα στοιχεια της state
		Object help=(Object)list_node_value(state_objects(state,0.0,(4.0*PORTAL_NUM)*SPACING),ln);
		TEST_ASSERT(help->rect.y==-help->rect.height);                                          ////Το στοιχειο ακουμπαει στ πατωμα
		ln=list_next(state_objects(state,0.0,(4.0*PORTAL_NUM)*SPACING),ln);                       //////επομενο στοιχεια της λιστας
	}
	free(ln);
	state_destroy(state);


}



void test_state_update_alt(){
	State state = state_create();
	TEST_ASSERT(state != NULL && state_info(state) != NULL);

	// Πληροφορίες για τα πλήκτρα (αρχικά κανένα δεν είναι πατημένο)
	struct key_state keys = { false, false, false, false, false, false };
	
	// Χωρίς κανένα πλήκτρο, ο χαρακτήρας μετακινείται 7 pixels μπροστά
	Rectangle old_rect = state_info(state)->character->rect;
	state_update(state, &keys);
	Rectangle new_rect = state_info(state)->character->rect;

	TEST_ASSERT( new_rect.x == old_rect.x + 7 && new_rect.y == old_rect.y );

	// Με πατημένο το δεξί βέλος, ο χαρακτήρας μετακινείται 12 pixes μπροστά
	keys.right = true;
	old_rect = state_info(state)->character->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->character->rect;

	TEST_ASSERT( new_rect.x == old_rect.x + 12 && new_rect.y == old_rect.y );

	// Προσθέστε επιπλέον ελέγχους
	
	keys.right=false;
	old_rect=new_rect;
	state_update(state,&keys);                                                           /////Το right arrow δεν ειναι πατημενο αρα αναμενουμε να κινηθουμε 7 pixels προς τα δεξια 
	new_rect=state_info(state)->character->rect;
	TEST_ASSERT(new_rect.x == old_rect.x + 7.0);
	
	keys.up=true;                                                                      ////Ελεγχουμε για το up arrow ενω το right arrow ειναι πατημενο με τον χαρακτηρα να πηγαινει προς τα δεξια
	old_rect=new_rect;
	state_update(state,&keys);
	new_rect=state_info(state)->character->rect;
	TEST_ASSERT(new_rect.x == old_rect.x + 7.0 && new_rect.y== old_rect.y -15.0);      ////-15 γιατι παει προς τα πανω
	
	keys.p=true;                                                                      ////Test το pause
	old_rect=new_rect;
	state_update(state,&keys);                                                      ////Ειμαστε σε pause
	new_rect=state_info(state)->character->rect;
	TEST_ASSERT(new_rect.x == old_rect.x && new_rect.y == old_rect.y);
	
	keys.p=true;                                                                   ////Δεν ειμαστε σε pause
	keys.enter=true;                                                              ////Test το enter
	old_rect=new_rect;
	state_info(state)->playing=false;
	state_update(state,&keys);
	new_rect=state_info(state)->character->rect;
	TEST_ASSERT(state_info(state)->playing==true);

	keys.up=false;
	keys.right=false;
	keys.p=false;
	keys.left=true;                                                        ////Τεστ το left arrow
	old_rect=new_rect;
	state_update(state,&keys);
	new_rect=state_info(state)->character->rect;
	TEST_ASSERT(state_info(state)->character->forward==false);                  ////Αλλαζουμε κατευθυνση
	TEST_ASSERT(new_rect.x == old_rect.x && new_rect.y == old_rect.y -15.0);    ////Ανεβαινουμε κατα 15 pixels

	old_rect=new_rect;
	state_update(state,&keys);                                                 /////Το left πατημενο ενω κινουμαστε προς τα αριστερα
	new_rect=state_info(state)->character->rect;
	TEST_ASSERT(new_rect.x == old_rect.x - 12.0);                             /////Πρεπει να εχουμε κινηθει 12 pixels προς τα αριστερα

	keys.left=false;                                                         /////Το left ΔΕΝ ειναι πατημενο ενω κινουμαστε προς τα αριστερα
	old_rect=new_rect;
	state_update(state,&keys);
	new_rect=state_info(state)->character->rect;
	TEST_ASSERT(new_rect.x==old_rect.x - 7.0);                                                          /////Αναμενουμε να εχουμε κινηθει 7 pixels προς τα αριστερα

	keys.p=true;                                                                      ////Μπαινουμε σε pause mode
	old_rect=new_rect;
	state_update(state,&keys);
	new_rect=state_info(state)->character->rect;
	TEST_ASSERT(new_rect.x==old_rect.x && new_rect.y==old_rect.y);                                 /////Αρχικα βεβαιωνομαστε οτι το pause δουλευει

	keys.p=false;                                  /////Δεν ξαναπαταμε το p γιατι θελουμε να μεινουμε σε pause 
	keys.n=true;                                   ////Παταμε το n αρα αναμενουμε το state να ενημερωθει για ενα μονο frame
	old_rect=new_rect;
	state_update(state,&keys);                ////Χωρις να εχουμε πατημενο κανενα βελος και ενω κινουμαστε προς τα αριστερα περιμενουμε να εχουμε μετακινηθει 7 pixels αριστερα
	new_rect=state_info(state)->character->rect;
	TEST_ASSERT(new_rect.x == old_rect.x - 7.0);
	TEST_ASSERT(new_rect.y == old_rect.y - 15.0);       /////Επισης αναμενουμε να εχουμε μετακινηθει και 15 pixels προς τα πανω

	state_destroy(state);


}



TEST_LIST = {
	{ "test_state_create_alt", test_state_create_alt },
	{ "test_state_update", test_state_update_alt },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};