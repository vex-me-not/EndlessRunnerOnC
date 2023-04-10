#include "raylib.h"



#include "state.h"
#include "interface.h"
#include "ADTVector.h"

#define FLOOR 76


struct state {
	Vector objects;			// περιέχει στοιχεία Object (Εμπόδια / Εχθροί / Πύλες)
	List portal_pairs;		// περιέχει PortaPair (ζευγάρια πυλών, είσοδος/έξοδος)

	struct state_info info;
};

Texture char_img;
Texture portal_img;
Texture enemy_img;
Texture obst_img;




void interface_init(){
	////Αρχικοποιηση του παραθυρου
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Trial 'n' Terror");
	SetTargetFPS(60);
    //InitAudioDevice();

    ////Φορτωση εικονων και ηχου
	char_img = LoadTextureFromImage(LoadImage("assets/character.png"));
	enemy_img = LoadTextureFromImage(LoadImage("assets/enemy.png"));
	portal_img = LoadTextureFromImage(LoadImage("assets/portal.png"));
	obst_img = LoadTextureFromImage(LoadImage("assets/obstacle.png"));
}




void interface_close(){
	//CloseAudioDevice();
	CloseWindow();
}


void interface_draw_frame(State state){
	
	BeginDrawing();
	
	ClearBackground(BEIGE);
	
	int char_x=(SCREEN_WIDTH/2)-(state->info.character->rect.width );
	int char_y=SCREEN_HEIGHT-FLOOR-1*(state->info.character->rect.height);	
	
	DrawTexture(char_img, char_x ,char_y, WHITE); ///GG
	DrawLine(0,SCREEN_HEIGHT-FLOOR,SCREEN_WIDTH,SCREEN_HEIGHT-FLOOR,RED);

	List to_draw=state_objects(state,state->info.character->rect.x-char_x,state->info.character->rect.x+char_x);
	for(ListNode lnode=list_first(to_draw);lnode!=LIST_EOF;lnode=list_next(to_draw,lnode)){
		Object ob=list_node_value(to_draw,lnode);
		int ob_x= (SCREEN_WIDTH/2) + 2*(ob->rect.width);
		int ob_y=SCREEN_HEIGHT-FLOOR-2*(ob->rect.height);
		if(ob->type==OBSTACLE){
			DrawTexture(obst_img,ob_x,ob_y,WHITE);
		}else if(ob->type==ENEMY){
			DrawTexture(enemy_img,ob_x,ob_y,WHITE);
		}else{
			DrawTexture(portal_img,ob_x,ob_y,WHITE);
		}
	}
	
	if(state->info.paused==true){    ////Εχουμε PAUSE
		DrawText(
			"GAME PAUSED. PRESS [P] TO CONTINUE",
			GetScreenWidth() / 2 - MeasureText("GAME PAUSED. PRESS [P] TO CONTINUE", 20) / 2,
			GetScreenHeight() / 2 - 50, 20, DARKPURPLE
			);
	}
	
	
	if (state->info.playing==false) {       /////Eχουμε GAME OVER
		DrawText(
			"PRESS [ENTER] TO PLAY AGAIN",
			 GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, DARKPURPLE
		);
	}    
	
	
	
	// Σχεδιάζουμε το σκορ,τις πυλες και το FPS counter
	DrawText(TextFormat("%03i/100", state->info.current_portal), 20, 20, 30, DARKGRAY);
	DrawText(TextFormat("\n%03i", state->info.wins), 20, 20, 30, DARKGRAY);
	DrawFPS(SCREEN_WIDTH - 80, 0);	
	
	
	EndDrawing();
}