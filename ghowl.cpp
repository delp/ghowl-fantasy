#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>

//TODO maybe ultimately the spritesheet should have some file with metadata about its layout...
struct spriteSheet {
    SDL_Texture* texture;
    int width = 0;
    int height = 0;
    int numberOfSprites;
    SDL_Rect* frames;
};

struct entity {
    spriteSheet* sheet;
    float x;
    float y;
    float dx;
    float dy;
    int animFrame;
};

//=====CONSTANTS=====

//FAMICOM screen width
const int FAM_W = 256;
const int FAM_H = 240;
const int SCALE_FACTOR = 3;
//metroid tiles
const int TILE_W = 16;
const int TILE_H = 16;
//so a metroid style tiled area would be 16 tiles wide and 15 tiles tall

//TODO So actually this window dimension is going to be
//FAM_W * 3
//FAM_H * 3
const int SCREEN_WIDTH = FAM_W * SCALE_FACTOR; 
const int SCREEN_HEIGHT = FAM_H * SCALE_FACTOR;
const int SPRITE = 80;

const int GHOWL_SPRITE_WIDTH = 16;
const int NUM_GHOWL_SPRITES = 4;

const int TILE_SPRITE_WIDTH = 16;
const int NUM_TILE_SPRITES = 4;

const int WRAITH_SPRITE_WIDTH = 16;
const int NUM_WRAITH_SPRITES = 5;

const int NUM_GREENERY_SPRITES = 6;
const int GREENERY_SPRITE = 16;

const float DELTA_V = 1.40;

const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

const int LEVEL_WIDTH = 16;
const int LEVEL_HEIGHT = 15;
const int tileMap[LEVEL_HEIGHT][LEVEL_WIDTH] = 
                {{ 0, 1, -1, -1, -1, 0, 2, 3, 1, 1, 1, 1, 1, 1, 1, 1},
                { 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3},
                { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0},
                { -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, -1, -1, -1, -1, -1, 2},
                { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1},
                { -1, -1, -1, 1, 1, 1, -1, -1, -1, -1, -1, -1, 1, -1, -1, 2},
                { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3},
                { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2},
                { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 2},
                { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 1},
                { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 1},
                { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 1},
                { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1},
                { -1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                { 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 1}};


//=====GLOBALS=====

//The window to render to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//The spritesheets
//TODO honestly this is going to have to be done in a less confusing way in the future
SDL_Rect ghowlFrames[] = { 
        SDL_Rect{0, 0, 
            GHOWL_SPRITE_WIDTH, GHOWL_SPRITE_WIDTH }, 
        SDL_Rect{GHOWL_SPRITE_WIDTH, 0, 
            GHOWL_SPRITE_WIDTH, GHOWL_SPRITE_WIDTH }, 
        SDL_Rect{GHOWL_SPRITE_WIDTH * 2, 0,    
            GHOWL_SPRITE_WIDTH, GHOWL_SPRITE_WIDTH }, 
        SDL_Rect{GHOWL_SPRITE_WIDTH * 3, 0, 
            GHOWL_SPRITE_WIDTH, GHOWL_SPRITE_WIDTH }, 
                       } ;

SDL_Rect wraithFrames[] = { 
        SDL_Rect{0, 0, 
            WRAITH_SPRITE_WIDTH, WRAITH_SPRITE_WIDTH }, 
        SDL_Rect{WRAITH_SPRITE_WIDTH, 0, 
            WRAITH_SPRITE_WIDTH, WRAITH_SPRITE_WIDTH }, 
        SDL_Rect{WRAITH_SPRITE_WIDTH * 2, 0,    
            WRAITH_SPRITE_WIDTH, WRAITH_SPRITE_WIDTH }, 
        SDL_Rect{WRAITH_SPRITE_WIDTH * 3, 0, 
            WRAITH_SPRITE_WIDTH, WRAITH_SPRITE_WIDTH }, 
        SDL_Rect{WRAITH_SPRITE_WIDTH * 4, 0, 
            WRAITH_SPRITE_WIDTH, WRAITH_SPRITE_WIDTH }, 
                       } ;

SDL_Rect greeneryFrames[] = {
    SDL_Rect{0, 0,
        GREENERY_SPRITE, GREENERY_SPRITE},
    SDL_Rect{GREENERY_SPRITE, 0,
        GREENERY_SPRITE, GREENERY_SPRITE}, 
    SDL_Rect{GREENERY_SPRITE * 2, 0,
        GREENERY_SPRITE, GREENERY_SPRITE},
    SDL_Rect{GREENERY_SPRITE * 3, 0,
        GREENERY_SPRITE, GREENERY_SPRITE},
    SDL_Rect{GREENERY_SPRITE * 3, 0,
        GREENERY_SPRITE, GREENERY_SPRITE}, 
    SDL_Rect{GREENERY_SPRITE * 3, 0,
        GREENERY_SPRITE, GREENERY_SPRITE} };

spriteSheet blocks = { NULL, 0, 0, NUM_TILE_SPRITES } ;
spriteSheet ghowlSheet = { NULL, 0, 0, NUM_GHOWL_SPRITES, &ghowlFrames[0] } ; //TODO lol this is dumb as hell
spriteSheet wraithSheet = { NULL, 0, 0, NUM_WRAITH_SPRITES, &wraithFrames[0] } ;  //....or is it?
spriteSheet greenerySheet = {NULL, 0, 0, NUM_GREENERY_SPRITES, &greeneryFrames[0] }; 

entity ghowlEntity = {&ghowlSheet, 50, 64, 0.0, 0.0, 0} ;
entity wraithEntity = {&wraithSheet, 50, 42, 0.0, 0.0, 0} ;

//=====FUNCTION DEFS=====

bool init();
SDL_Texture* loadSpriteSheetTexture();
void freeTextures(spriteSheet* s);
void render(spriteSheet* s, int x, int y, SDL_Rect* clip);
void renderEntity(entity* ent);
int getRand(int max);
void initRand();
void collisionDetection();
void getTileAt(int x, int y);

bool checkCollision(entity* ent) {
    bool result = false;

    //get the position of the entity
    int x = ent->x;
    int y = ent->y;
    //get its size
    //TODO fix this, add a width and height to entity
    int width = GHOWL_SPRITE_WIDTH;
    int height = GHOWL_SPRITE_WIDTH;

    //get tile size
    int tileSize = TILE_SPRITE_WIDTH;



    //TODO move this intot he nentity
    //top
    int top = ent->y;
    //bottom
    int bottom = ent->y + GHOWL_SPRITE_WIDTH;
    //left
    int left = ent->x;
    //right
    int right = ent->x + GHOWL_SPRITE_WIDTH;
    
    int leftTile = left / tileSize;
    int rightTile = right / tileSize;
    int topTile = top / tileSize;
    int bottomTile = bottom / tileSize;


    if(leftTile < 0) 
        leftTile = 0;
    if(rightTile > tileSize) 
        rightTile = tileSize;
    if(topTile < 0) 
        topTile = 0;
    if(bottomTile > tileSize)
        bottomTile = tileSize;

    for(int i = leftTile; i<=rightTile; i++) {
        for(int j = topTile; j<=bottomTile; j++) {
            int tileVal = tileMap[j][i];
            if(tileVal > -1) {
                printf("collision at %d, %d\n", j, i);
                result = true;
            }
        }
    }

    return result;
}

void initRand() {
    srand(time(NULL));
}

int getRand(int max) {
    return rand() % max;
}

void render(spriteSheet* s, int x, int y, SDL_Rect* clip) {
    SDL_Rect renderQuad = { x, y, s->width, s->height };

    if( clip != NULL ) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to screen
    SDL_RenderCopy( gRenderer, s->texture, clip, &renderQuad );
}

//TODO REMOVE THIS TEST CODE
void updateFrames(entity* ent) {
    ent->animFrame++;
    if(ent->animFrame >= ent->sheet->numberOfSprites) {
        ent->animFrame = 0;
    }
}

void updateEntity(entity* ent) {


    //TODO if you cast the ent->dx to (int) here, the diagnoal jitter disappears
    //you need to figure out why that is


    float oldX = ent->x;
    ent->x += ent->dx;
    if(checkCollision(ent)) {
        ent->x = oldX;
    }

    float oldY = ent->y;
    ent->y += ent->dy;
    if(checkCollision(ent)) {
        ent->y = oldY;
    }

}

void renderEntity(entity* ent) {
    //TODO add a null check
    //TODO this is also hella dumb
    SDL_Rect* clip = &ent->sheet->frames[ent->animFrame];
 
    // TODO why is the quad this?
    SDL_Rect renderQuad = { (int) ent->x, (int) ent->y, ent->sheet->width, ent->sheet->height };
  
    if( clip != NULL ) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to screen
    SDL_RenderCopy( gRenderer, ent->sheet->texture, clip, &renderQuad );
}




void freeTextures(spriteSheet* s) {
    if( s->texture != NULL) {
        SDL_DestroyTexture(s->texture);
        s->texture = NULL;
        s->width = 0;
        s->height = 0;
    }
}

bool init() { 
    bool success = true;

    //init SDl
    if( SDL_Init( SDL_INIT_VIDEO) < 0 ) {
        printf("Failed to Initialize SDL: %s\n", SDL_GetError() );
    } else {
        //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" ) ) {
            printf( "Failed to enable linear texture filtering...");
        }

        //Create the window
        gWindow = SDL_CreateWindow( "GHOWL-ARC: GENESYS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        //Error check
        if( gWindow == NULL ) {
            printf( "Couldn't create window: %s\n", SDL_GetError() );
        } else {

            //create renderer for window
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED);

            //error check
            if(gRenderer == NULL) {
                printf( "Couldn't create Renderer: %s\n", SDL_GetError() );
            } else {

                //init renderer color
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

                //if( !SDL_RenderSetLogicalSize(gRenderer, FAM_W, FAM_H)) {
                //    printf( "Failed to enable logical size thingy...");
                //}

                SDL_RenderSetScale(gRenderer, SCALE_FACTOR, SCALE_FACTOR);

                //init PNG loading
                int imgFlags = IMG_INIT_PNG;

                //TODO: wtf is up with this bitwise and?
                if( !(IMG_Init( imgFlags) & imgFlags ) ) {
                    printf( "SDL_image could not initialize: %s\n", IMG_GetError() );
                    success = false;
                }
            }
        }
    }
    return success;
}

bool loadSpriteSheetTexture(spriteSheet* s, std::string path) {

    bool success = true;

    //Get rid of preexisting texture
    freeTextures(s);

    //This is the final texture
    SDL_Texture* newTexture = NULL;

    //load the specified image
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL ) {
        success = false;
        printf( "Unable to load image %s    SDL_Image Error: %s\n", path.c_str(), IMG_GetError());
    } else {
        //Create the color key for the image/surface
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
        if( newTexture == NULL) {
            printf( "Unable to create texture from  %s    SDL Error: %s\n", path.c_str(), SDL_GetError());
            success = false;
        } else {
            s->width = loadedSurface->w;
            s->height = loadedSurface->h;
        }

        
        //get rid of old loaded Surface
        SDL_FreeSurface( loadedSurface);
    }

    s->texture = newTexture;
    return success;

}

bool loadMedia() { 
    bool success = true;

    //load the spritesheet texture
    if(!loadSpriteSheetTexture(&blocks, "res/tiles.png")) {
        success = false;
    } else {
        //set the sprites up, config them
        //TODO  ????????????? create some SDL_Rect to hold the sprites...
        //then in the main function you use the gRenderer to render those sprites from the rects....
    }

    //TODO refactor this
    if(!loadSpriteSheetTexture(&ghowlSheet, "res/ghowl.png")) {
        success = false;
    } else {
        //set the sprites up, config them
        //TODO  ????????????? create some SDL_Rect to hold the sprites...
        //then in the main function you use the gRenderer to render those sprites from the rects....
    }

     //TODO refactor this
    if(!loadSpriteSheetTexture(&wraithSheet, "res/wraith.png")) {
        success = false;
    } else {
        //set the sprites up, config them
        //TODO  ????????????? create some SDL_Rect to hold the sprites...
        //then in the main function you use the gRenderer to render those sprites from the rects....
    }
    
    //TODO refactor this
    if(!loadSpriteSheetTexture(&greenerySheet, "res/Greenery.png")) {
        success = false;
    } else {
        //set the sprites up, config them
        //TODO  ????????????? create some SDL_Rect to hold the sprites...
        //then in the main function you use the gRenderer to render those sprites from the rects....
    }

    return success;
}

//TODO implement this
bool close() { return true; }

int main(int argc, char* args[]) {
    //Start up SDL and create window
    if( !init() ) {

        printf("Failed to initialize\n");

    } else {
        
        //load media
        if( !loadMedia() ) {

            printf("Failed to load media...\n");

        } else {
            bool quit = false;

            //Event handling
            SDL_Event e;

            //tales ov da frame counter
            int countedFrames = 0;
            
            while(!quit) {

                //Need to know the start time to calculate frame length
                int frame_start = SDL_GetTicks();

                //handle all Q'd events 
                while (SDL_PollEvent( &e ) != 0) {
                    if (e.type == SDL_QUIT) {
                        quit = true;
                    } else if( e.type == SDL_KEYDOWN && e.key.repeat == 0 ) {
                        switch( e.key.keysym.sym) {
                            case SDLK_ESCAPE:
                            quit = true;
                            break;



                            case SDLK_w:
                            //
                            wraithEntity.dy -= DELTA_V;
                            break;
                            
                            case SDLK_s:
                            //
                            wraithEntity.dy += DELTA_V;
                            break;
                            
                            case SDLK_a:
                            //
                            wraithEntity.dx -= DELTA_V;
                            break;
                            
                            case SDLK_d:
                            //
                            wraithEntity.dx += DELTA_V;
                            break;

                            case SDLK_UP:
                            ghowlEntity.dy -= DELTA_V;
                            break;
                            
                            case SDLK_DOWN:
                            ghowlEntity.dy += DELTA_V;
                            break;

                            case SDLK_LEFT:
                            ghowlEntity.dx -= DELTA_V;
                            break;

                            case SDLK_RIGHT:
                            ghowlEntity.dx += DELTA_V;
                            break;

                        }
                    } else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
                        switch( e.key.keysym.sym) {

                            case SDLK_w:
                            //
                            wraithEntity.dy += DELTA_V;
                            break;
                            
                            case SDLK_s:
                            //
                            wraithEntity.dy -= DELTA_V;
                            break;
                            
                            case SDLK_a:
                            //
                            wraithEntity.dx += DELTA_V;
                            break;
                            
                            case SDLK_d:
                            //
                            wraithEntity.dx -= DELTA_V;
                            break;
                            
                            case SDLK_UP:
                            //
                            ghowlEntity.dy += DELTA_V;
                            break;
                            
                            case SDLK_DOWN:
                            //
                            ghowlEntity.dy -= DELTA_V;
                            break;

                            case SDLK_LEFT:
                            ghowlEntity.dx += DELTA_V;
                            break;

                            case SDLK_RIGHT:
                            ghowlEntity.dx -= DELTA_V;
                            break;

                        }
                    }
                }

                //Clear the screen/draw background
                SDL_SetRenderDrawColor( gRenderer, 0x11, 0x11, 0x00, 0xFF); 
                SDL_RenderClear( gRenderer );
              
                for(int y = 0; y < LEVEL_HEIGHT; y++) {
                    for(int x = 0; x < LEVEL_WIDTH; x++) {

                        int tileNum = tileMap[y][x];
                        if(tileNum >= -1) {

                            //Draw things (Render things to gRenderer)
                            SDL_Rect spriteClip;
                            spriteClip.x = (tileNum * TILE_SPRITE_WIDTH );
                            spriteClip.y = 0;
                            spriteClip.w = TILE_SPRITE_WIDTH;
                            spriteClip.h = TILE_SPRITE_WIDTH;
                              
                            render(&blocks, x * TILE_SPRITE_WIDTH, y * TILE_SPRITE_WIDTH, &spriteClip);
                        }
                    }
                } 

                SDL_Rect greenClip;
                greenClip.x = GREENERY_SPRITE * 3;
                greenClip.y = 0;
                greenClip.w = GREENERY_SPRITE;
                greenClip.h = GREENERY_SPRITE * 2;

                render(&greenerySheet, 192, 48, &greenClip);


                //TODO time for code reuse
                SDL_Rect greenClip2;
                greenClip2.x = GREENERY_SPRITE * 4;
                greenClip2.y = 0;
                greenClip2.w = GREENERY_SPRITE;
                greenClip2.h = GREENERY_SPRITE * 2;

                render(&greenerySheet, 128, 176, &greenClip2);


                SDL_Rect greenClip3;
                greenClip3.x = GREENERY_SPRITE * 5;
                greenClip3.y = 0;
                greenClip3.w = GREENERY_SPRITE;
                greenClip3.h = GREENERY_SPRITE * 2;

                render(&greenerySheet, 224, 96, &greenClip3);


                //UPDATE DUDES
                updateEntity(&ghowlEntity);
                updateEntity(&wraithEntity);

                //CHECK COLLISION and handle
                checkCollision(&ghowlEntity);

                //DRAW DUDES
                renderEntity(&ghowlEntity);
                renderEntity(&wraithEntity);

                int time_ms = SDL_GetTicks();
                float avgFPS = countedFrames / ( time_ms / 1000.f );
                if( avgFPS > 2000000 ) {
                    avgFPS = 0;  
                }

                //Update screen
                SDL_RenderPresent( gRenderer );

                //TODO this seems to work but diagonally movement seems to be choppy?
                int frame_end = SDL_GetTicks();
                int frame_length = frame_end - frame_start;
                //printf("frame start: %d\n", frame_start);
                //printf("frame end: %d\n", frame_end);
                //printf("frame len: %d\n", frame_length);
                //printf("stpf - frame len: %d\n", SCREEN_TICKS_PER_FRAME - frame_length);
                if( frame_length < SCREEN_TICKS_PER_FRAME ) {
                    SDL_Delay(SCREEN_TICKS_PER_FRAME - frame_length);
                }
                //printf("frame ticks: %d\n", SDL_GetTicks());

                countedFrames++;
                
            }
        }
    }

    //Free resources, close SDL
    close();

    return 0; //GREAT SUCCESS!
}
