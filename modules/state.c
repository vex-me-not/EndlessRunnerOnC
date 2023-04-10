
#include <stdlib.h>

#include "ADTVector.h"
#include "ADTList.h"
#include "state.h"



// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	Vector objects;			// περιέχει στοιχεία Object (Εμπόδια / Εχθροί / Πύλες)
	List portal_pairs;		// περιέχει PortaPair (ζευγάρια πυλών, είσοδος/έξοδος)

	struct state_info info;
};

// Ζευγάρια πυλών

typedef struct portal_pair {
	Object entrance;		// η πύλη entrance
	Object exit;			// οδηγεί στην exit
}* PortalPair;


// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.current_portal = 0;			// Δεν έχουμε περάσει καμία πύλη
	state->info.wins = 0;					// Δεν έχουμε νίκες ακόμα
	state->info.playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	state->info.paused = false;				// Χωρίς να είναι paused.

	// Πληροφορίες για το χαρακτήρα.
	Object character = state->info.character = malloc(sizeof(*character));
	character->type = CHARACTER;
	character->forward = true;
	character->jumping = false;

    // Ο χαρακτήρας (όπως και όλα τα αντικείμενα) έχουν συντεταγμένες x,y σε ένα
    // καρτεσιανό επίπεδο.
	// - Στο άξονα x το 0 είναι η αρχή στης πίστας και οι συντεταγμένες
	//   μεγαλώνουν προς τα δεξιά.
	// - Στον άξονα y το 0 είναι το "δάπεδο" της πίστας, και οι
	//   συντεταγμένες μεγαλώνουν προς τα _κάτω_.
	// Πέρα από τις συντεταγμένες, αποθηκεύουμε και τις διαστάσεις width,height
	// κάθε αντικειμένου. Τα x,y,width,height ορίζουν ένα παραλληλόγραμμο, οπότε
	// μπορούν να αποθηκευτούν όλα μαζί στο obj->rect τύπου Rectangle (ορίζεται
	// στο include/raylib.h).
	// 
	// Προσοχή: τα x,y αναφέρονται στην πάνω-αριστερά γωνία του Rectangle, και
	// τα y μεγαλώνουν προς τα κάτω, οπότε πχ ο χαρακτήρας που έχει height=38,
	// αν θέλουμε να "κάθεται" πάνω στο δάπεδο, θα πρέπει να έχει y=-38.

	character->rect.width = 70;
	character->rect.height = 38;
	character->rect.x = 0;
	character->rect.y = - character->rect.height;

	// Δημιουργία των objects (πύλες / εμπόδια / εχθροί) και προσθήκη στο vector
	// state->objects. Η πίστα περιέχει συνολικά 4*PORTAL_NUM αντικείμενα, από
	// τα οποία τα PORTAL_NUM είναι πύλες, και τα υπόλοια εμπόδια και εχθροί.

	state->objects = vector_create(0, free);		// Δημιουργία του vector

	for (int i = 0; i < 4*PORTAL_NUM; i++) {
		// Δημιουργία του Object και προσθήκη στο vector
		Object obj = malloc(sizeof(*obj));
		vector_insert_last(state->objects, obj);

		// Κάθε 4 αντικείμενα υπάρχει μια πύλη. Τα υπόλοιπα αντικείμενα
		// επιλέγονται τυχαία.

		if(i % 4 == 3) {							// Το 4ο, 8ο, 12ο κλπ αντικείμενο
			obj->type = PORTAL;						// είναι πύλη.
			obj->rect.width = 100;
			obj->rect.height = 5;

		} else if(rand() % 2 == 0) {				// Για τα υπόλοιπα, με πιθανότητα 50%
			obj->type = OBSTACLE;					// επιλέγουμε εμπόδιο.
			obj->rect.width = 10;
			obj->rect.height = 80;

		} else {
			obj->type = ENEMY;						// Και τα υπόλοιπα είναι εχθροί.
			obj->rect.width = 30;
			obj->rect.height = 30;
			obj->forward = false;					// Οι εχθροί αρχικά κινούνται προς τα αριστερά.
		}
		
		if(i==0){
			obj->type = OBSTACLE;                 /////Προστεθηκε για να συμβαδιζει με την state_alt
		}

		// Τα αντικείμενα είναι ομοιόμορφα τοποθετημένα σε απόσταση SPACING
		// μεταξύ τους, και "κάθονται" πάνω στο δάπεδο.

		obj->rect.x = (i+1) * SPACING;
		obj->rect.y = - obj->rect.height;
	}

	// TODO: αρχικοποίηση της λίστας obj->portal_pairs
	
	state->portal_pairs=list_create(free);              ////Δημιουργια της Λιστας
	Vector exits_found=vector_create(0,NULL);           ////Εδω θα φυλαμε ολες τις εξοδους που χρησιμοποιησαμε
	
	for(int i = 3;i<vector_size(state->objects);i=i+4){  ////Διατρεχουμε ολες τις πυλες(η πρωτη πυλη βρισκεται για i =3)
		////Δημιουργια του Pair και εισαγωγη στη λιστα
		PortalPair pair=malloc(sizeof(*pair));                  ////Δημιουργουμε το ζευγος
		pair->entrance=NULL;                                    ////Το αρχικοποιουμε σε NULL γιατι why not?
		pair->exit=NULL;
		list_insert_next(state->portal_pairs,list_last(state->portal_pairs),pair);      ////Εισαγουμε το ζευγος στη λιστα με τα ζευγη
		
		
		Object ob=vector_get_at(state->objects,i);                        ////Επειδη ξεκινησαμε απο πυλη και διασχιζουμε τα στοιχεια με βημα4 ξερουμε οτι το ob ειναι πυλη
		pair->entrance=ob;                                               ////Αρα το αναθετουμε ως entrance
		
		int exit_num=(rand()%vector_size(state->objects));
		while(exit_num==i){
			exit_num=(rand()%vector_size(state->objects));
		}
		
		ob=vector_get_at(state->objects,exit_num);
		
		while(ob->type != PORTAL){                 ////Βεβαιωνομαστε οτι παιρνουμε PORTAL
			
			if(exit_num + 1 < vector_size(state->objects)){
				exit_num++;
			}else{
				exit_num=3;
			}
			ob=vector_get_at(state->objects,exit_num);        /////Παιρνουμε ενα νεο στοιχειο για να ξαναδοκιμασουμε
		}
		
		for(int j = 0; j<vector_size(exits_found);j++){           /////Ελεγχουμε αν το εχουμε ηδη βρει αυτο το στοιχειο,δηλαδη αν βρισκεται στο exits_found
			while( ob == (Object)(vector_get_at(exits_found,j)) ){
				if(exit_num<vector_size(state->objects)-4){
					exit_num=exit_num +4;
				}else{
					exit_num=3;
				}
				ob=vector_get_at(state->objects,exit_num);
			}
		
		}
		vector_insert_last(exits_found,ob);           /////Βαζουμε το ob στην λιστα με τις πυλες που βρηκαμε/χρησιμοποιησαμε σαν εξοδους
		pair->exit=ob;                       /////Αναθετουμε την exit στο ζευγαρι

		

	}
	vector_destroy(exits_found);
	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {
	// Προς υλοποίηση
	if(state!=NULL){
		return &(state->info);
	}
	return NULL;
}





// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη x είναι ανάμεσα στο x_from και x_to.

List state_objects(State state, float x_from, float x_to) {
	// Προς υλοποίηση
	
	if(state != NULL && state->objects!=NULL){
		List obj_list=list_create(NULL);                          ////Δημιουργουμε μια νεα λιστα  
		for (int i = 0; i < vector_size(state->objects); i++){    //// Διατρεχουμε το vector state->objects
                            
			if( ((Object)vector_get_at(state->objects,i))->rect.x>=x_from && ((Object)vector_get_at(state->objects,i))->rect.x<=x_to){               ////Αν το στοιχειο πληροι τα κριτηρια που θεσαμε βαλτο στη λιστα
				list_insert_next(obj_list,list_last(obj_list),vector_get_at(state->objects,i));
			}
		}
		return obj_list ;                                       ////Επιστρεφουμε την τελικη λιστα
	}else{
		return NULL;
	}	                                                          //// Αν η κατασταση δεν εχει οριστει επιστρεφει NULL

}


// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys) {
	// Προς υλοποίηση
	
	//////////////////Αυξανουμε τον μετρητη current portal////////////////////
	List objects_behind=state_objects(state,0.0,state->info.character->rect.x);
	int portals_behind=0; 
	for(ListNode lnode=list_first(objects_behind);lnode != LIST_EOF;lnode=list_next(objects_behind,lnode)){
		if( ((Object)list_node_value(objects_behind,lnode))->type==PORTAL ){
			portals_behind++;
		}
	}
	state->info.current_portal=portals_behind;
	list_destroy(objects_behind);

	if(keys->p==true){                 //// Εχουμε πατησει το pause button
		state->info.paused=!(state->info.paused);       //// Το παιχνιδι μπαινει σε pause mode

	}

	if(keys->enter==true && state->info.playing==false && state->info.paused==false ){        ////Εχουμε πατησει το enter ενω βρισκομαστε σε GAME OVER και οχι σε pause
		state->info.playing=true;                              ////Ξεκιναμε να παιζουμε
		state->info.character->rect.x=0.0;                      /////O χαρακτηρας επιστρεφει στην αρχη
	}


	if(keys->up==true){
		state->info.character->jumping=true;
	}

	if( (state->info.playing==true && state->info.paused==false )|| (keys->n==true && state->info.paused==true) ){
		
			////////////////Κινηση Χαρακτηρα κατα y////////////////////
		if(state->info.character->jumping==true){
			state->info.character->rect.y-=15.0;         /////Μετακινουμαστε στον y προς τα πανω
			if(state->info.character->rect.y< -state->info.character->rect.height -220.0){      //////Ξεπερασμε το μεγιστο οριο
				state->info.character->jumping=false;                                        /////Δεν κανουμε αλμα πια,φτασεμε στα 220 pixels απο το εδαφος
				state->info.character->rect.y=-state->info.character->rect.height -220.0;    /////
			}
		

		}else{
			if(state->info.character->rect.y == -state->info.character->rect.height -220){  /////Ειμαστε στον αερα
				state->info.character->rect.y+=15.0;
			
				if(state->info.character->rect.y>-state->info.character->rect.height){         /////Ειμαστε κατω απο το εδαφος
					state->info.character->rect.y=-state->info.character->rect.height;
				}
			
			}
		}
			///////Τελος Κινησης Χαρακτηρα κατα y///////

				//////////Κινηση Χαρακτηρα κατα x////////////
		if(keys->right==true && state->info.character->forward==true){       ////Εχουμε πατησει το right arrow και ο χαρακτηρας κινειται προς τα δεξια 
			state->info.character->rect.x+=12.0;
		}else if(keys->right==true && state->info.character->forward==false){   ////Εχουμε πατησει το right arrow και ο χαρακτηρας κινειται προς τα αριστερα
			state->info.character->forward=!(state->info.character->forward);                                ////Ο χαρακτηρας αλλαζει φορα κινησης(κινειται προς τα δεξια τωρα)
		}else if(keys->left==true && state->info.character->forward==true){     ////Εχουμε πατησει το left arrow και ο χαρακτηρας κινειται προς τα δεξια
			state->info.character->forward=!(state->info.character->forward);                               ////Ο χαρακτηρας αλλαζει φορα κινησης(κινειται προς τα αριστερα τωρα)
		}else if(keys->left==true && state->info.character->forward==false){     ////Εχουμε πατησει το left arrow και ο χαρακτηρας κινειται προς τα αριστερα
			state->info.character->rect.x-=12.0;
		}else{                                                                  ////Δεν εχουμε πατησει κανενα απο τα left και right arrows
			if(state->info.character->forward==true){                           ////Αν ο χαρακτηρας κινειται προς τα δεξια
				state->info.character->rect.x+=7.0;
			}else{                                                             ////Αν ο χαρακτηρας κινειται προς τα αριστερα
				state->info.character->rect.x-=7.0;
			}
		}
			//////////Τελος κινησης Χαρακτηρα κατα x/////////


			//////Κινηση των εχθρων κατα 5 pixels στον αξονα των x κατα τη φορα που κινουνται
		for(int i = 0;i<vector_size(state->objects);i++){
		
			if(((Object)vector_get_at(state->objects,i))->type==ENEMY){            /////Βρηκαμε εχθρο
				if(((Object)vector_get_at(state->objects,i))->forward==true){      ////Ο εχθρος κινειται προς τα δεξια
					((Object)vector_get_at(state->objects,i))->rect.x+=5.0;        ////Το x του εχθρου αυξανεται κατα 5
				}else{                                                             ////Ο εχθρος κινειται προς τα αριστερα
					((Object)vector_get_at(state->objects,i))->rect.x-=5.0;       ////Το χ του εχθρου μειωνεται κατα 5
				}
			}
		}
		////////Τελος κινησης των εχθρων///////

		//////Ελεγχος για Συγκρουσεις Χαρκτηρα////////	
	
		for(int i = 0;i<vector_size(state->objects);i++){

			if(CheckCollisionRecs(state->info.character->rect,((Object)vector_get_at(state->objects,i))->rect)==true){                      /////O χαρακτηρας συγκρουστηκε
			
				if(((Object)vector_get_at(state->objects,i))->type==ENEMY || ((Object)vector_get_at(state->objects,i))->type==OBSTACLE){     ////Συγκρουστηκε με ENEMY ή OBSTACLE
					state->info.playing=false;                                          ////GAME OVER
					i=vector_size(state->objects);                                    /////Δεν υπαρχει λογος να διατρεξουμε το υπολοιπο της state->objects
				}else{                                                               /////Συγκρουστηκε με PORTAL
					ListNode ln=list_first(state->portal_pairs);

					while(ln != LIST_EOF ){
						PortalPair pphelp=list_node_value(state->portal_pairs,ln);
					
						if( state->info.character->forward==true &&  pphelp->entrance==vector_get_at(state->objects,i) ){        /////Ο χαρακτηρας κινειται προς τα δεξια αρα συγκρουστηκε με entr
							state->info.character->rect.x=pphelp->exit->rect.x;                                                 /////Αρα θα βγει στο αντιστοιχο exit
							ln=LIST_EOF;                                                                                       /////Μιας και καθε ζευγος ειναι μοναδικο δεν χρειαζεται να ψαχνουμε αλλο
						}else if( state->info.character->forward==false && pphelp->exit==vector_get_at(state->objects,i)){     /////Ο χαρακτηρας κεινιται προς τα αριστερα αρα συγκρουστηκε με exit
							state->info.character->rect.x=pphelp->entrance->rect.x;                                         /////Αρα θα βγει στο αντιστοιχο entr
							ln=LIST_EOF;

						}

						if(ln != LIST_EOF){                                            /////Δεν εχουμε βρει ακομη το στοιχειο που ψαχνουμε
							ln=list_next(state->portal_pairs,ln);                      ////To επομενο στοιχειο της λιστας
						}
					}
				}
			}
		}
		/////Τελος Ελεγχου για Συγκρουσεις Χαρακτηρα////////

		//////Ελεγχος για Συγκρουσεις Εχθρων/////////

		for(int i = 0;i<vector_size(state->objects);i++){
			if( ((Object)vector_get_at(state->objects,i))->type==ENEMY ){                                /////Εχουμε εχθρο
				for(int j = 0;j<vector_size(state->objects);j++){
					if( (CheckCollisionRecs( ((Object)vector_get_at(state->objects,i))->rect , ((Object)vector_get_at(state->objects,j))->rect ) ==true ) && ( j != i ) ){     ////O εχθρος συγκρουστηκε 
						if( ((Object)vector_get_at(state->objects,j))->type==OBSTACLE ){                                      /////Ο εχθρος συγκρουστηκε με OBSTACLE
							((Object)vector_get_at(state->objects,i))->forward=!(((Object)vector_get_at(state->objects,i))->forward);         /////Ο Εχθρος αλλαζει κατευθυνση
						}else if( ((Object)vector_get_at(state->objects,j))->type==PORTAL ){                             /////Ο εχθρος συγκρουστηκε με PORTAL
						
							ListNode ln=list_first(state->portal_pairs);

							while(ln != LIST_EOF ){
								 PortalPair pphelp=list_node_value(state->portal_pairs,ln);

								if(((Object)vector_get_at(state->objects,i))->forward==true && pphelp->entrance==((Object)vector_get_at(state->objects,j)) ){      ////O εχθρος κινειται προς τα δεξια
									((Object)vector_get_at(state->objects,i))->rect.x=pphelp->exit->rect.x;
									ln=LIST_EOF;
								}else if(((Object)vector_get_at(state->objects,i))->forward==false && pphelp->exit==((Object)vector_get_at(state->objects,j)) ){
									((Object)vector_get_at(state->objects,i))->rect.x=pphelp->entrance->rect.x;
									ln=LIST_EOF;
								}
							
								if(ln != LIST_EOF){
									ln=list_next(state->portal_pairs,ln);                      ////To επομενο στοιχειο της λιστας
								}
							}
						}
					}
				}
			}	
		}
		//////Tελος ελεγχου για συγκρουσεις Εχθρου


		//////Ελεγχος για νικη/////////
		PortalPair last_gate=list_node_value(state->portal_pairs,list_last(state->portal_pairs));
		if(state->info.character->rect.x>=last_gate->entrance->rect.x){
			//printf("We did it!\n");
			state->info.wins++;
			state->info.character->rect.x=0.0;
		}
		//////Τελος ελεγχου για νικη/////
	
	}
		
}        
		
				
	
		


// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	// Προς υλοποίηση
	list_destroy(state->portal_pairs);
	vector_destroy(state->objects);
	free(state);         ////Η state_create δεσμευει χωρο για την state με malloc αρα καλουμε free(state)
}