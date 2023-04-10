

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>



#include "ADTVector.h"
#include "ADTList.h"
#include "ADTMap.h"
#include "ADTSet.h"
#include "state.h"
#include "sets_util.h"




int compare_objects_at_x(Pointer a,Pointer b){
    float a_x=((Object)a)->rect.x;
    float b_x=((Object)b)->rect.x;

    return (int)(a_x-b_x);    
}

int compare_objects(Pointer a ,Pointer b){

    assert(a!=NULL);
    assert(b!=NULL);
    
    float a_x=((Object)a)->rect.x;
    float b_x=((Object)b)->rect.x;
    float a_y=((Object)a)->rect.y;
    float b_y=((Object)b)->rect.y;

    if(a_x!=b_x){
        return (int) (a_x - b_x);    
    }else if(a_x==b_x && a_y!=b_y){
        return (int) (b_y-a_y);
    }else if(a_x==b_x && a_y==b_y){ 
        return a-b;
    }else{
        return (int) (a_x-b_x);
    }
}


struct state {
	Set objects;			////περιέχει στοιχεία Object (Εμπόδια / Εχθροί / Πύλες),σε Set διατεταγμενα κατα χ
    Map entr_to_ex;         ////Portals "entrances" που οδηγουν σε "exits"	
    Map ex_to_entr;         ////Exits που οδηγουν στις entrances τους

	struct state_info info;
};



State state_create() {
    ///////Δεσμευουμε μνημη για το state///////
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


    character->rect.width = 70;
	character->rect.height = 38;
	character->rect.x = 0;
	character->rect.y = - character->rect.height;           /////Για να ειναι στο πατωμα πρεπει να εχει y=-height

    state->objects = set_create(compare_objects,NULL);

    Vector all_portals=vector_create(0,NULL);
    
    for(int i = 0;i<4*PORTAL_NUM;i++){
        Object obj=malloc(sizeof(*obj));
        
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

		// Τα αντικείμενα είναι ομοιόμορφα τοποθετημένα σε απόσταση SPACING
		// μεταξύ τους, και "κάθονται" πάνω στο δάπεδο.

		obj->rect.x = (i+1) * SPACING;
		obj->rect.y = - obj->rect.height;
        if(i==0){                        ////Φροντιζω το 1ο στοιχειο της πιστας να ειναι εμποδιο για να κανω τη ζωη μου ευκολη
            obj->type = OBSTACLE;        ////Αν εμφανιζονται εχθροι πισω απο x==SPACING προκυπτουν θεματα στη state_objects που δεν μπορω να λυσω 
        }

        if(obj->type==PORTAL){
            Object duplicate=malloc(sizeof(*duplicate));
            duplicate=obj;
            vector_insert_last(all_portals,duplicate);
        }

        set_insert(state->objects,obj);
    }

    /////Αρχικοποιηση των "ζευγων" πυλων
    state->entr_to_ex=map_create(compare_objects_at_x,NULL,NULL);          /////Εισοδοι που οδηγουν στις αντιστοιχες εξοδους(κινηση προς τα αριστερα)
    state->ex_to_entr=map_create(compare_objects_at_x,NULL,NULL);         /////Εξοδοι που οδηγουν στις αντισοιχες εισοδους (κινηση προς το δεξια)
    

    bool seen[vector_size(all_portals)];
    for(int i =0;i<vector_size(all_portals);i++){
        seen[i]=false;
       // printf("%d\n",seen[i]);
    }
   
    for(int k=0;k<vector_size(all_portals);k++){                            /////Επειτα αντιστοιχουμε τις εισοδους με εξοδους
        

        int exit_num=rand() % vector_size(all_portals);                    /////Μια τυχαια πυλη βρισκεται στη θεση exit_num του all_portals
        
        while(exit_num==k){                                                  /////Βεβαιωνομαστε οτι η εξοδος ειναι διαφορετικη απο την εισοδο
            exit_num=rand() % vector_size(all_portals);
        }
        Object ob=vector_get_at(all_portals,exit_num);         ////Η υπ'αριθμον exit_num πυλη

        while(seen[exit_num]==true ){    ////Αν η εξοδος που πηραμε εχει ηδη ανατεθει σε μια εισοδο πρεπει να παρουμε αλλη
            if(exit_num + 1 <vector_size(all_portals)){                  ////Σε περιπτωση που  δεν πηραμε την τελευταια πυλη του all_portals παιρνουμε την επομενη
                exit_num++;
            }else{                           ////Αν πηραμε την τελευταια επιστρεφουμε παλι στην αρχη
                exit_num=0;
            }
            ob=vector_get_at(all_portals,exit_num);         
        }

        ////Η πυλη που πηραμε δεν εχει αντιστοιχιστει καπου αρα την αντιστοιχουμε με την υπ'αριθμον k εισοδο
        map_insert(state->entr_to_ex,vector_get_at(all_portals,k),ob);
        map_insert(state->ex_to_entr,ob,vector_get_at(all_portals,k));
        seen[exit_num]=true;
    }
    vector_destroy(all_portals);
   
    return state;

}



StateInfo state_info(State state){
    if(state !=NULL){
        return &(state->info);
    }else{
        return NULL;
    }

}

List state_objects(State state, float x_from, float x_to){
    assert(state!=NULL);
    assert(state->objects!=NULL);
   
    List obj_list=list_create(NULL);
   
    if( (x_from<0 && x_to<0) ||(x_to<SPACING) ){
        return obj_list;
    }

    Object ob_from=malloc(sizeof(*ob_from));
    ob_from->forward=false;
    ob_from->type=PORTAL;
    ob_from->jumping=false;
    ob_from->rect.height=10.0;
    ob_from->rect.width=10.0;
    ob_from->rect.y=-80.0;
    ob_from->rect.x=x_from;
    
    Object ob_to=malloc(sizeof(*ob_to));
    ob_to->forward=false;
    ob_to->type=PORTAL;
    ob_to->jumping=false;
    ob_to->rect.height=10.0;
    ob_to->rect.width=10.0;
    ob_to->rect.y=-80.0;
    ob_to->rect.x=x_to;
    
   
    SetNode node_from=set_find_node(state->objects,set_find_eq_or_greater(state->objects,ob_from));
    SetNode node_to=set_find_node(state->objects,set_find_eq_or_smaller(state->objects,ob_to));
    
    for(SetNode node=node_from;node!=node_to && node!=NULL;
    node=set_next(state->objects,node)){    
        list_insert_next(obj_list,list_last(obj_list),set_node_value(state->objects,node));
    }

    list_insert_next(obj_list,list_last(obj_list),set_node_value(state->objects,node_to));
    free(ob_to);
    free(ob_from);
    
    return obj_list;
}


void state_update(State state, KeyState keys){
    	
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
		float pixels_moved=0.0;
        if(keys->right==true && state->info.character->forward==true){       ////Εχουμε πατησει το right arrow και ο χαρακτηρας κινειται προς τα δεξια 
			pixels_moved=12.0;
		}else if(keys->right==true && state->info.character->forward==false){   ////Εχουμε πατησει το right arrow και ο χαρακτηρας κινειται προς τα αριστερα
			state->info.character->forward=!(state->info.character->forward);                                ////Ο χαρακτηρας αλλαζει φορα κινησης(κινειται προς τα δεξια τωρα)
		}else if(keys->left==true && state->info.character->forward==true){     ////Εχουμε πατησει το left arrow και ο χαρακτηρας κινειται προς τα δεξια
			state->info.character->forward=!(state->info.character->forward);                               ////Ο χαρακτηρας αλλαζει φορα κινησης(κινειται προς τα αριστερα τωρα)
		}else if(keys->left==true && state->info.character->forward==false){     ////Εχουμε πατησει το left arrow και ο χαρακτηρας κινειται προς τα αριστερα
			pixels_moved=-12.0;
		}else{                                                                  ////Δεν εχουμε πατησει κανενα απο τα left και right arrows
			if(state->info.character->forward==true){                           ////Αν ο χαρακτηρας κινειται προς τα δεξια
				pixels_moved=7.0;
			}else{                                                             ////Αν ο χαρακτηρας κινειται προς τα αριστερα
				pixels_moved=-7.0;
			}
		}
        state->info.character->rect.x+=pixels_moved;
			//////////Τελος κινησης Χαρακτηρα κατα x/////////
			
            //////Κινηση των εχθρων κατα 5 pixels στον αξονα των x κατα τη φορα που κινουνται
		for(SetNode node = set_first(state->objects);node != SET_EOF;node=set_next(state->objects,node)){
		
			if(((Object)set_node_value(state->objects,node))->type==ENEMY){            /////Βρηκαμε εχθρο
                Object dupl=malloc(sizeof(*dupl));
                dupl=set_node_value(state->objects,node);                         ////Αντιγραφουμε τον εχθρο γιατι θα χρειαστει να τον ματαβαλλουμε
                assert(set_remove(state->objects,set_node_value(state->objects,node))==true);  ////Τον αφαιρουμε γιατι δεν μπορουμε να τον πειραξουμε οσο ειναι στο set
                
                if(dupl->forward==true){     ////Αναλογα με το προς τα που κινειται ο εχθρος θα αλλαξουμε και το x του 
                    dupl->rect.x+=5.0;       ////Προχωραει 5 pixels προς τα δεξια
                    List en_pot_col=state_objects(state,dupl->rect.x-5.0,dupl->rect.x);
                    for(ListNode lnode=list_first(en_pot_col);lnode!=LIST_EOF && lnode!=NULL;lnode=list_next(en_pot_col,lnode)){
                        Object potential=list_node_value(en_pot_col,lnode);
                        if(potential != NULL){
                            if(CheckCollisionRecs(dupl->rect,potential->rect)==true){
                                if(potential->type==OBSTACLE){
                                    dupl->forward=!(dupl->forward);
                                }else if(potential->type==PORTAL){
                                    dupl->rect.x=((Object)(map_find(state->entr_to_ex,potential)))->rect.x;
                                }
                            }
                        }
                    }
                }else{
                    dupl->rect.x-=5.0;      ////Προχωραει 5 pixels προς τα αριστερα
                    /*List en_pot_col=state_objects(state,dupl->rect.x,dupl->rect.x+5.0);
                    for(ListNode lnode=list_first(en_pot_col);lnode!=LIST_EOF && lnode!=NULL;lnode=list_next(en_pot_col,lnode)){
                        Object potential=list_node_value(en_pot_col,lnode);
                        if(potential != NULL){
                            if(CheckCollisionRecs(dupl->rect,potential->rect)==true){
                                if(potential->type==OBSTACLE){
                                    dupl->forward=!(dupl->forward);
                                }else if(potential->type==PORTAL && map_find_node(state->ex_to_entr,potential)!=NULL){
                                    dupl->rect.x=((Object)(map_find_node(state->ex_to_entr,potential)))->rect.x;
                                }
                            }
                        }
                    }*/
                }
                set_insert(state->objects,dupl);     ////Επιστρεφουμε τον εχθρο πισω στο set
            }
        
        }
		////////Τελος κινησης των εχθρων/////// 
        
        ///////Ελεγχος γισ συγκρουσεις χαρακτηρα///////
        List pot_col=list_create(NULL);
        if(state->info.character->forward==true){
            pot_col=state_objects(state,state->info.character->rect.x-pixels_moved,state->info.character->rect.x);
        }else{
            pot_col=state_objects(state,state->info.character->rect.x,state->info.character->rect.x-pixels_moved);
        }
        for(ListNode lnode=list_first(pot_col);lnode!=LIST_EOF;lnode=list_next(pot_col,lnode)){
            Object potential=list_node_value(pot_col,lnode);
            if(potential != NULL){           /////O χαρακτηρας ειναι στη θεση x ενος αντικειμενου
                if(CheckCollisionRecs(state->info.character->rect,potential->rect)==true){    /////Αν συγκρουστηκε με το αντικειμενο πρεπει να δουμε με τον τυπο του αντικειμενου

                    if( (potential->type==ENEMY) || (potential->type==OBSTACLE)){                   ////Ο χαρακτηρας συγκρουστηκε με εχθρο ή εμποδιο
                        state->info.playing=false;                                                 ////Αρα GAME OVER
                    }else{                                                                        ////Ο Χαρακτηρας συγκρουστηκε με portal
                        if(state->info.character->forward==true){                                                      ////Ενω κινουταν προς τα δεξια
                            state->info.character->rect.x=((Object)map_find(state->entr_to_ex,potential))->rect.x;  ////Αρα συγκρουστηκε με entrance αρα θα βγει σε exit
                        }else{
                            state->info.character->rect.x=((Object)map_find(state->ex_to_entr,potential))->rect.x;
                        }
                    }
                }
            }
        }
        list_destroy(pot_col);
        //////Τελος συγκρουσεις χαρακτηρα///////////


	
    	//////Ελεγχος για νικη/////////
		if(state->info.character->rect.x>=4.0*PORTAL_NUM*SPACING){
			state->info.wins++;
			state->info.character->rect.x=0.0;
		}
		//////Τελος ελεγχου για νικη/////
    
    }/////Tελος της καταστασης παιχνιδιου



}/////////Τελος της state_update


void state_destroy(State state){
    
    assert(set_set_destroy_value(state->objects,free)==NULL);
    set_destroy(state->objects);
    map_destroy(state->entr_to_ex);
    map_destroy(state->ex_to_entr);
    free(state);
    

}