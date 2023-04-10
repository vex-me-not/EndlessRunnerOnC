



#include "raylib.h"
#include "interface.h"
#include "state.h"


State state;

void update_and_draw() {

	struct key_state keys = { 
	
	.left=IsKeyDown(KEY_LEFT), 
	.right=IsKeyDown(KEY_RIGHT), 
	.up=IsKeyDown(KEY_UP), 
	.n=IsKeyDown(KEY_N), 
	.p=IsKeyPressed(KEY_P), 
	.enter=IsKeyPressed(KEY_ENTER) };

	state_update(state,&keys);

	interface_draw_frame(state);
}

int main(){
    state = state_create();
	interface_init();

	// Η κλήση αυτή καλεί συνεχόμενα την update_and_draw μέχρι ο χρήστης να κλείσει το παράθυρο
	start_main_loop(update_and_draw);

	interface_close();


	return 0;
}