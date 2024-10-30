#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "include/raylib.h"
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
typedef struct Room{
	int x,y;
	int width,height;
}Room;

typedef struct MovementList{
	int x, y;
	int speed;
	struct MovementList* next;
} MovementList;

typedef struct tile{
	int x, y;
}tile;

typedef struct Loot{
	int x,y;
	bool take;
}Loot;

int ChooseTile(){
	int out = rand()%101;
	if(out<50){
		return 1;
	}else if(out < 70){
		return 2;
	}else if(out < 99){
		return 3;
	}
	return 4;
}

typedef struct AI{
	int speed;
	int vision;
	int x,y;
	int mode;
	int lastx,lasty;
	int endx,endy;
	int Damage;
	int health;
	int frame;
	int dead;
}AI;

int** GenerateTerrain(int maxrooms, int minx, int miny, int lenx, int leny){
	int** map;
	map = malloc(sizeof(int) *100);
	for(int i =0;i<100;i++){
		map[i] = malloc(sizeof(int)*100);
	}
	for(int i =0;i<100;i++){
		for(int j =0;j<100; j++){
			map[i][j] = 0;
		}
	}
	
	for(int i =0;i<100;i++){
		for(int j =0;j<100; j++){
			map[i][j] = 0;
		}
	}
	Room rooms[maxrooms];
	int generated = 0;
	while(generated < maxrooms){
		Room temp;
		temp.width = rand()%lenx +minx;
		temp.height = rand()%leny +miny;
		
		temp.x = rand() % (100 - temp.width -1);
		temp.y = rand() % (100 - temp.height -1);
		bool cool = true;
		for(int i = 0; i<generated;i++){
			if(temp.x < rooms[i].x + rooms[i].width
			&& temp.x +temp.width > rooms[i].x
			&& temp.y < rooms[i].y + rooms[i].height
			&& temp.y +temp.height > rooms[i].y){
				cool = false;
			}
			
		}
		if(cool){
			rooms[generated] = temp;
			generated++; 
		}
	}
	
	for(int i = 0;i<generated;i++){
		Room temp = rooms[i];
		for(int j = 0; j< temp.height;j++){
			for(int k =0;k< temp.width;k++){
				map[temp.y + j][temp.x + k] = ChooseTile();
			}
		}
	}
	int player, exit;
	player = rand()%20;
	exit = player;
	while(player == exit){
		exit = rand()%20;
	}
	
	
	
	for(int i = 0;i<generated -1;i++){
		int startx, starty, endx, endy;
		startx = rooms[i].x + rooms[i].width/2;
		starty = rooms[i].y + rooms[i].height/2;
		endx = rooms[i+1].x + rooms[i+1].width/2;
		endy = rooms[i+1].y + rooms[i+1].height/2;
		
		int x = startx < endx ? startx : endx;
		int y = starty < endy ? starty : endy;
		endx = startx > endx ? startx : endx;
		endy = starty > endy ? starty : endy;
		
		
		
		if(rand()%2 == 0){
			while(x <= endx){
				map[y][x] = ChooseTile();
				x++;
			}
			while(y <= endy){
				map[y][x] = ChooseTile();
				y++;
			}

		}else{
			while(y <= endy){
				map[y][x] = ChooseTile();
				y++;
			}
			while(x <= endx){
				map[y][x] = ChooseTile();
				x++;
			}
			
		}
	}
	
	map[rand()%rooms[player].height +rooms[player].y][rand()%rooms[player].width +rooms[player].x] = 10;
	map[rand()%rooms[exit].height +rooms[exit].y][rand()%rooms[exit].width +rooms[exit].x] = 11;
	
/*	for(int i =0;i<100;i++){
		for(int j =0;j<100; j++){
			printf("%d",map[i][j]);
		}
		printf("\n");
	}*/
/*	FILE* f = fopen("map.txt", "w");
	for(int i =0;i<100;i++){
		for(int j =0;j<100; j++){
			fprintf(f,"%d",map[i][j]);
		}
		fprintf(f,"\n");
	}
	fclose(f);*/
	return map;
}

void DrawTile(int x, int y, int sx, int sy, Texture2D land){
	Rectangle r;
	r.height = 16;
	r.width = 16;
	r.x = sx*16;
	r.y = sy*16;
					
	Rectangle d;
	d.height = 64;
	d.width = 64;
	d.x = x*64;
	d.y = y*64;
					
	DrawTexturePro(land,r,d,(Vector2){0,0},0,WHITE);
}

void DrawEnemy(AI enemies[30], Texture2D Spider, bool update){
	for(int i=0;i<30;i++){		
	Rectangle r;
	r.height = 16;
	r.width = 16;
	if(enemies[i].dead>0){
		r.x =0;
		r.y =4*16;
	}else{
		r.x = enemies[i].frame*16;
		r.y = 16;
	}
					
	Rectangle d;
	d.height = 64;
	d.width = 64;
	d.x = enemies[i].x*64;
	d.y = enemies[i].y*64;
	if(update)
		enemies[i].frame++;
	
	if(enemies[i].frame == 4){
		enemies[i].frame = 0;
	}
	
	DrawTexturePro(Spider,r,d,(Vector2){0,0},0,WHITE);
	}
}

void DrawLoot(Loot loot[50], Texture2D cheese, int frame){
	for(int i=0;i<50;i++){		
	if(loot[i].take)
		continue;
	Rectangle r;
	r.height = 32;
	r.width = 32;
	r.x =0;
	r.y =0;
	float add;
	if(frame%33 >16){		
		add = -frame%33* 5/33;
	}else{
		add = +frame%33* 5/33 -5;
	}
	
	Rectangle d;
	d.height = 64;
	d.width = 64;
	d.x = loot[i].x*64 ;
	d.y = loot[i].y*64 + add;

	
	DrawEllipse(d.x+35,d.y+45-add,20,5,(Color){0,0,0,100});
	DrawTexturePro(cheese,r,d,(Vector2){0,0},0,WHITE);
	}
}

void DrawPlayer(int x, int y, int frame, Texture2D Player){
	Rectangle r;
	r.height = 16;
	r.width = 16;
	r.x = frame*16;
	r.y = 16;
					
	Rectangle d;
	d.height = 64;
	d.width = 64;
	d.x = x*64;
	d.y = y*64;
	
	DrawTexturePro(Player,r,d,(Vector2){0,0},0,WHITE);
}

bool checkmovementlist(MovementList* list, int x, int y){
	while(list){
		if(list->x == x && list->y == y){
			return false;
		}
		list = list->next;
	}
	return true;
}

MovementList* ResolveMovement(int map[100][100], int x, int y, int speed){
	MovementList* list = NULL;
	MovementList* stack = NULL;
	MovementList* temp = NULL;

	stack = malloc(sizeof(MovementList));
	stack->x = x;
	stack->y = y;
	stack->speed = speed;
	stack->next = NULL;
	if(speed != 0){
		while(stack){
		x = stack->x;
		y = stack->y;
		speed = stack->speed;
		temp = stack;
		stack = stack->next;
		free(temp);
			if(y<99 && speed >0){	
			if(map[y+1][x] == 1){
				if(checkmovementlist(list,x,y+1)){
					temp = malloc(sizeof(MovementList));
					temp->x = x;
					temp->y = y+1;
					temp->next = list;
					temp->speed = speed-1;
					list = temp;
					temp = malloc(sizeof(MovementList));
					temp->x = x;
					temp->y = y+1;
					temp->speed = speed-1;
					temp->next = stack;
					stack = temp;
				}
			}
			}
			if(y>0 && speed >0){	
			if(map[y-1][x] == 1){
				if(checkmovementlist(list,x,y-1)){
					temp = malloc(sizeof(MovementList));
					temp->x = x;
					temp->y = y-1;
					temp->next = list;
					temp->speed = speed-1;
					list = temp;
					temp = malloc(sizeof(MovementList));
					temp->x = x;
					temp->y = y-1;
					temp->speed = speed-1;
					temp->next = stack;
					stack = temp;
				}
			}
			}
			if(x<99 && speed >0){	
			if(map[y][x+1] == 1){
				if(checkmovementlist(list,x+1,y)){
					temp = malloc(sizeof(MovementList));
					temp->x = x+1;
					temp->y = y;
					temp->speed = speed-1;
					temp->next = list;
					list = temp;
					temp = malloc(sizeof(MovementList));
					temp->x = x+1;
					temp->y = y;
					temp->speed = speed-1;
					temp->next = stack;
					stack = temp;
				}
			}
			}
			if(x>0 && speed >0){	
			if(map[y][x-1] == 1){
				if(checkmovementlist(list,x-1,y)){
					temp = malloc(sizeof(MovementList));
					temp->x = x-1;
					temp->y = y;
					temp->speed = speed-1;
					temp->next = list;
					list = temp;
					temp = malloc(sizeof(MovementList));
					temp->x = x-1;
					temp->y = y;
					temp->speed = speed-1;
					temp->next = stack;
					stack = temp;
				}
			}
			}
		}
		speed =0;
	}
	
	return list;
}

tile gettile(Camera2D cam){
	Vector2 position = GetMousePosition();
	tile t;
	t.x = (int)position.x/64;
	t.y = (int)position.y/64;
	t.x += -cam.offset.x/64;
	t.y += -cam.offset.y/64;
	
	t.x += cam.target.x/64;
	t.y += cam.target.y/64;
	
	
	return t;
}

void shaveoff(MovementList** list, int x, int y, AI enemies[30]){
	MovementList* temp = *list;
	bool change = true;
	while(change){
	change = false;
	 
	if(temp->x == x && temp->y == y){
		*list = (*list)->next;
		free(temp);
		temp = *list;
		change = true;
		//printf("here");
	}else{
		for(int i =0;i<30;i++){
			if(enemies[i].x == temp->x && enemies[i].y == temp->y){
				*list = (*list)->next;
				free(temp);
				temp = *list;
				change = true;
				break;
				//printf("here");
			}
		}
	}
	}

	MovementList* head = *list;
	if(head){
		temp = (*list)->next;
		while(temp){
			if(temp->x == x && temp->y == y){
				head->next = temp->next;
				free(temp);
				//printf("here");
				
		}else{
			for(int i =0;i<30;i++){
				if(enemies[i].x == temp->x && enemies[i].y == temp->y){
					head->next = temp->next;
					free(temp);
					break;
					//printf("here");
				}
			}
		}
			head = head->next;
			if(head){
				temp = head->next;
			}else{
				temp = NULL;
			}
		}
	}	
}

void UpdateAi(AI enemy[30] ,int x , int y,int map[100][100],bool* turn){
	//AI* enemy = *enemies;
	for(int i =0;i<30;i++){
	if(enemy[i].dead > 0 && *turn){
		enemy[i].dead--;
		continue;	
	}
	MovementList* list = ResolveMovement(map,enemy[i].x,enemy[i].y,enemy[i].vision);
	MovementList* move = NULL;
	if(*turn){
		move = ResolveMovement(map,enemy[i].x,enemy[i].y,enemy[i].speed);
		shaveoff(&move,0,0,enemy);
	}
	if(checkmovementlist(list,x,y)){
		enemy[i].mode = 0;
		enemy[i].vision = 5;
	}else{
		enemy[i].vision = 7;
		enemy[i].mode = 1;
		enemy[i].lastx = x;
		enemy[i].lasty = y;
	}
	if(enemy[i].mode == 0 && *turn){
		int rando = 0;
		MovementList* temp = move;
		if(enemy[i].lastx == -1){
			while(temp){
				rando++;
				temp = temp->next;
			}
			int mover = rand()%rando;
			temp = move;
			for(int i =0;i<mover;i++){
				temp = temp->next;
			}
			enemy[i].endx= temp->x;
			enemy[i].endy= temp->y;
		}else{
			int xc = enemy[i].x,yc = enemy[i].y;
			MovementList* temp = move;
			while(temp){
				if(pow(enemy[i].lastx-xc,2)+pow(enemy[i].lasty-yc,2)>pow(enemy[i].lastx-temp->x,2)+pow(enemy[i].lasty-temp->y,2)){
					xc = temp->x;
					yc = temp->y;
				}
				temp = temp->next;
			}
			enemy[i].endx = xc;
			enemy[i].endy = yc;
			if(enemy[i].lastx == xc && enemy[i].lasty == yc){
				enemy[i].lastx = -1;
				enemy[i].lasty = -1;
			}
		}
		
	}else if(enemy[i].mode == 1 && *turn){
		int xc = enemy[i].x,yc = enemy[i].y;
		MovementList* temp = move;
		while(temp){
			if(pow(enemy[i].lastx-xc,2)+pow(enemy[i].lasty-yc,2)>pow(enemy[i].lastx-temp->x,2)+pow(enemy[i].lasty-temp->y,2)){
				xc = temp->x;
				yc = temp->y;
			}
			temp = temp->next;
		}
		enemy[i].endx = xc;
		enemy[i].endy = yc;
		enemy[i].lastx = x;
		enemy[i].lastx = y;
	}
	
	while(list){
			//DrawRectangle(PlayerMove->x*64,PlayerMove->y*64,60,60,WHITE);
			MovementList* temp = list;
			list = list->next;
			free(temp);
	}
	while(move && *turn){
			MovementList* temp = move;
			if(IsKeyDown(KEY_TAB))
				DrawRectangle(temp->x*64,temp->y*64,60,60,RED);
			move = move->next;
			free(temp);
	}
	}
	if(*turn)
		*turn = false;
	
}

bool AnimateAi(AI enemy[30]){
	bool result = false;
	for(int i =0; i<30;i++){
		if(enemy[i].x>enemy[i].endx){
			result = true;
			enemy[i].x--;
		}else if(enemy[i].x<enemy[i].endx){
			result = true;
			enemy[i].x++;
		}else if(enemy[i].y>enemy[i].endy){
			result = true;
			enemy[i].y--;
		}else if(enemy[i].y<enemy[i].endy){
			result = true;
			enemy[i].y++;
		}
	}
	return result;
}

int main(int argc, char *argv[]) {
	srand(time(NULL));
	SetTraceLogLevel(LOG_NONE);
    InitWindow(768,512,"Cheese Agnet");
    SetTargetFPS(60);
    InitAudioDevice();
    
    Wave wave = LoadWave("data/explore.wav");
    Wave wave2 = LoadWave("data/explosion.wav");
    Wave wave3 = LoadWave("data/pickupCoin.wav");
    Wave wave4 = LoadWave("data/synth.wav");
    Wave wave5 = LoadWave("data/powerUp.wav");
    Wave wave6 = LoadWave("data/blipSelect.wav");
//    Music music = LoadMusicStreamFromMemory("wav",wave.data,wave.sampleSize);
//    Music music = LoadMusicStream("explore.wav");
//    PlayMusicStream(music);
	Sound sound =  LoadSoundFromWave(wave);
	Sound explosion =  LoadSoundFromWave(wave2);
	Sound coin =  LoadSoundFromWave(wave3);
	Sound death =  LoadSoundFromWave(wave4);
	Sound done =  LoadSoundFromWave(wave5);
	Sound load =  LoadSoundFromWave(wave6);
    PlaySound(sound);
gate:	int** map;
	map = GenerateTerrain(30,5,5,15,15);
	int tacticalmap[100][100];
	bool PlayerTurn = false;
	bool enemymoving = false;
	bool movecooldown = false;
	bool gamerunning = true;
	bool win = true;
	bool tutorial = true;
	int score =0;
	int guncooldown = 0;
	
    
    Vector2 Player;
    Vector2 End;
    AI enemies[30];
    Loot loot[50];
    enemies[0].mode = 0;
    
    Shader shader = LoadShader(0, "data/Shader.fs");
    int timerUniform = GetShaderLocation(shader,"timer");
    RenderTexture screen = LoadRenderTexture(768, 512);
    
    
    for(int i =0;i<100;i++){
		for(int j =0;j<100; j++){
			if(map[i][j] == 10){
				Player.x = (float)j;
				Player.y = (float)i;
			//	printf("here %d::%d\n",i,j);
			}
			if(map[i][j] == 11){
				End.x = (float)j;
				End.y = (float)i;
			}
		}
	}
	
	for(int i =0;i<100;i++){
		for(int j =0;j<100; j++){
			if(map[i][j] != 0){
				tacticalmap[i][j] = 1;
			}else{
				tacticalmap[i][j] = 0;
			}
		}
	}
    
	Camera2D cam;
    cam.offset = (Vector2){(float)Player.x*-64, (float)Player.y*-64};
  /*  enemies[0].x = Player.x;
    enemies[0].y = Player.y;
    enemies[0].vision = 3;
    enemies[0].speed = 2;*/
     for(int i =0; i<50;i++){
    	int x,y;
    	do{
			x = rand()%100;
    		y = rand()%100;
		}while(tacticalmap[y][x] == 0);
    	
    	loot[i].x = x;
   		loot[i].y = y;
   		loot[i].take = false;
	}
	
    for(int i =0; i<30;i++){
    	int x,y;
    	do{
			x = rand()%100;
    		y = rand()%100;
		}while(tacticalmap[y][x] == 0);
    	
    	enemies[i].x = x;
   		enemies[i].y = y;
   		enemies[i].endx = x;
   		enemies[i].endy = y;
   		enemies[i].vision = 5;
    	enemies[i].speed = 2;
    	enemies[i].frame = 0;
    	enemies[i].lastx = -1;
    	enemies[i].lasty = -1;
    	enemies[i].mode = 0;
    	enemies[i].dead = 0;
	}
    cam.target = (Vector2){-384,-256};
    cam.zoom = 1;
    //printf("%f::%f\n",Player.x,Player.y);
    //printf("%f::%f",End.x,End.y);
   // printf("%f::%f\n",cam.offset.x,cam.offset.y);
    Texture2D land =LoadTexture("data/CosmicLilac_Tiles.png");
    Texture2D Spider =LoadTexture("data/Spider.png");
    Texture2D Pl =LoadTexture("data/RadioOperator.png");
    Texture2D Kanbei =LoadTexture("data/Kanbei.png");
    Texture2D Cheese =LoadTexture("data/I_cheese.png");
    Texture2D LMG =LoadTexture("data/LMG.png");
    MovementList* PlayerMove;
    int counter = 0;
    
    
    while (!WindowShouldClose()) {
    	if(gamerunning){
		counter++;
    	PlayerMove = ResolveMovement(tacticalmap,Player.x,Player.y,3);
		tile t = gettile(cam);
		if(!IsSoundPlaying(sound)){
			PlaySound(sound);
		}
		
		if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !PlayerTurn && !enemymoving && !tutorial && guncooldown==0){
    		if(!checkmovementlist(PlayerMove,t.x,t.y)){
    			for(int i =0; i<30;i++){
    				if(enemies[i].x == t.x && enemies[i].y == t.y){
    					enemies[i].dead = 3;
    					guncooldown = 7;
    					PlaySound(explosion);
    					break;
					}
				}
			}
		}
    	shaveoff(&PlayerMove,Player.x,Player.y,enemies);
    	if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !PlayerTurn && !enemymoving && !tutorial){
    		if(!checkmovementlist(PlayerMove,t.x,t.y) && !PlayerTurn && !enemymoving){
    			Player.x = t.x;
    			Player.y = t.y;
    			PlayerTurn = true;
    			//printf("%f::%f\n",Player.x,Player.y);
				if(guncooldown >0){
					guncooldown--;
					if(guncooldown ==0){
						PlaySound(load);
					}
				}
			}
		}
		
    	
    	
		cam.offset = (Vector2){(float)Player.x*-64, (float)Player.y*-64};
    	
		BeginDrawing();
    	BeginTextureMode(screen);
		ClearBackground(BLACK);
    	
		
		
		BeginMode2D(cam);
		if(IsKeyDown(KEY_A)){
			cam.offset.x++;
		}
		if(IsKeyDown(KEY_D)){
			cam.offset.x--;
		}
		if(IsKeyDown(KEY_W)){
			cam.offset.y++;
		}
		if(IsKeyDown(KEY_S)){
			cam.offset.y--;
		}
		//printf("%f::%f\n",cam.offset.x,cam.offset.y);
		for(int i =0;i<100;i++){
			for(int j =0;j<100; j++){
				if(map[i][j] == 1){
				/*	DrawRectangle(j*64,i*64,60,60,RED);	
					Rectangle r;
					r.height = 16;
					r.width = 16;
					r.x = 16*2;
					r.y = 16*2;
					
					Rectangle d;
					d.height = 64;
					d.width = 64;
					d.x = j*64;
					d.y = i*64;
					
					DrawTexturePro(land,r,d,(Vector2){0,0},0,WHITE);*/
					DrawTile(j,i,2,2,land);
				}
				if(map[i][j] == 2){
					DrawTile(j,i,7,3,land);
				}
				if(map[i][j] == 3){
					DrawTile(j,i,8,3,land);
				}
				if(map[i][j] == 4){
					DrawTile(j,i,3,4,land);
				}
				
				if(map[i][j] == 10){
					DrawTile(j,i,7,3,land);
				}
				
				if(map[i][j] == 11){
					DrawRectangle(j*64,i*64,60,60,YELLOW);	
				}
			}
		}
		
		
		
		//DrawRectangle(Player.x*64,Player.y*64,60,60,BLUE);
		for(int i =0; i<50;i++){
			if(loot[i].x == Player.x && loot[i].y == Player.y && !loot[i].take){
				PlaySound(coin);
				score++;
				loot[i].take = true;
			}
		}
		DrawLoot(loot,Cheese,counter);
		DrawRectangle(t.x*64,t.y*64,60,60,RED);
		/*for(int i =0;i<30;i++){
			DrawRectangle(enemies[i].x*64,enemies[i].y*64,60,60,RED);
		}*/
		if(counter%16 < 8)
			DrawPlayer(Player.x,Player.y,0,Pl);
		if(counter%16 >= 8)
			DrawPlayer(Player.x,Player.y,1,Pl);
		if(counter%4 == 0){			
			DrawEnemy(enemies,Spider,true);
		}else{
			DrawEnemy(enemies,Spider,false);
		}
			
		int smth = 0;
			
    //	if(IsKeyPressed(KEY_X)){
    	//	UpdateAi(enemies,Player.x,Player.y,tacticalmap);
    	if(enemymoving || PlayerTurn){
    		
			UpdateAi(enemies,Player.x,Player.y,tacticalmap,&PlayerTurn);
			if(movecooldown){
				enemymoving = AnimateAi(enemies);
				movecooldown = false;
			}
			
			
		}
		if(counter%16 == 0 && !movecooldown){
			movecooldown = true;
		}
	//	}
		
		while(PlayerMove){
			if(IsKeyDown(KEY_TAB))
				DrawRectangle(PlayerMove->x*64,PlayerMove->y*64,60,60,WHITE);
		//	printf("%d::%d::%d\n",smth++,PlayerMove->x,PlayerMove->y);
			MovementList* temp = PlayerMove;
			PlayerMove = PlayerMove->next;
			free(temp);
		}
		//printf("-------------\n");
		
		EndMode2D();
		if(Player.x== End.x && Player.y == End.y){
			StopSound(sound);
			PlaySound(done);
			gamerunning = false;
			win = true;
		}
		
		for(int i = 0;i<30;i++){
			if(enemies[i].x == Player.x && enemies[i].y == Player.y){
				StopSound(sound);
				PlaySound(death);
				gamerunning = false;
				win = false;
			}
		}
		
		if(tutorial){
    		DrawRectangle(0,325,768,200,LIGHTGRAY);
    		DrawRectangleLinesEx((Rectangle){00,325,768,200},5,GRAY);
    		DrawText("Wake up Soldier,",150,330,20,BLACK);
    		DrawText("your mission is avoid the mechs and collect 50 Cheese",150,355,20,BLACK);
    		DrawText("when you want out go to the golden square",150,380,20,BLACK);
    		DrawText("Left click to move, tab to see your movement range",150,405,20,BLACK);
    		DrawText("right click to attack",150,430,20,BLACK);
    		DrawText("press space to start",150,490,20,BLACK);
			DrawTexture(Kanbei,5,350,WHITE);
    	
			if(IsKeyPressed(KEY_SPACE))
				tutorial = false;	
		}
		
		
		}else{
			BeginDrawing();
			BeginTextureMode(screen);
			ClearBackground(BLACK);
			if(win){
				if(score ==50){
					DrawText("MISSION PASSED",40,200,80,GREEN);				
				}else{
					DrawText("YOU SURVIVED",40,200,80,GREEN);
				}
				DrawText(TextFormat("YOUR SCORE %d",score),200,285,40,GREEN);
				DrawText("press space to restart",250,490,20,WHITE);
			
			}else{
				DrawText("MISSION FAILED",50,200,80,RED);
				DrawText(TextFormat("YOUR SCORE %d",score),200,285,40,RED);
				DrawText("press space to restart",250,490,20,WHITE);
			//	DrawText("MISSION FAILED",50,200,80,RED);
			}
			if(IsKeyPressed(KEY_SPACE)){
				goto gate;
			}

		} 
		EndTextureMode();
		float timer = counter%1000 /1000.0;
		SetShaderValue(shader,timerUniform,&timer,SHADER_ATTRIB_FLOAT);
		ClearBackground(BLACK);
		BeginShaderMode(shader);

		DrawTexturePro(screen.texture,(Rectangle){0,0,screen.texture.width,-screen.texture.height},(Rectangle){0,0,screen.texture.width,screen.texture.height},(Vector2){0,0},0,WHITE);
		
		EndShaderMode();
		if(gamerunning){
	//	DrawRectangle(0,0,100,110,LIGHTGRAY);
		if(guncooldown >0){
			DrawTexturePro(LMG,(Rectangle){0,0,132,132},(Rectangle){-80,-85,264,264},(Vector2){0,0},0,BLACK);			
		}else{
			DrawTexturePro(LMG,(Rectangle){0,0,132,132},(Rectangle){-80,-85,264,264},(Vector2){0,0},0,WHITE);			
		}
			DrawTexturePro(Cheese,(Rectangle){0,0,32,32},(Rectangle){0,50,64,64},(Vector2){0,0},0,WHITE);
			DrawText(TextFormat("%d",score),70,66,40,RED);
		}
		EndDrawing();
	}
	
	CloseWindow();
	return 0;
}
