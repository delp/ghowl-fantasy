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
    int x;
    int y;
    int animFrame;
};

//=====CONSTANTS=====

/*TODO can you use SDLRenderCopy to stretch the spritesheet texture after load?
int SDL_RenderCopy(SDL_Renderer*   renderer,
                   SDL_Texture*    texture,
                   const SDL_Rect* srcrect,
                   const SDL_Rect* dstrect)
                   */

//TODO fix the sprite exports make them 16!!!!
//FAMICOM screen width
const int FAM_W = 256;
const int FAM_H = 240;
//metroid tiles
const int TILE_W = 16;
const int TILE_H = 16;
//so a metroid style tiled area would be 16 tiles wide and 15 tiles tall

//TODO So actually this window dimension is going to be
//FAM_W * 3
//FAM_H * 3
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SPRITE = 80;

const int GHOWL_SPRITE_WIDTH = 75;
const int NUM_GHOWL_SPRITES = 4;

const int TILE_SPRITE_WIDTH = 80;
const int NUM_TILE_SPRITES = 4;

const int WRAITH_SPRITE_WIDTH = 80;
const int NUM_WRAITH_SPRITES = 5;

const int LEVEL_WIDTH = 10;
const int LEVEL_HEIGHT = 8;
const int tileMap[LEVEL_HEIGHT][LEVEL_WIDTH] = 
                {{ 0, 1, 0, 2, 3, 0, 2, 3, 1, 1,},
                { 1, -1, -1, -1, -1, -1, -1, -1, -1, 3},
                { -1, -1, -1, -1, -1, -1, -1, -1, -1, 1},
                { -1, -1, -1, -1, -1, -1, -1, -1, -1, 1},
                { -1, -1, -1, -1, -1 -1, -1, -1, -1, 3},
                { -1, -1, -1, -1, -1 -1, -1, -1, -1, 3},
                { -1, -1, -1, -1, 1, 0, 0, 0, 2, 1},
                { 3, 1, 0, 0, 0, 0, 2, 1, 2, 1}};

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




spriteSheet blocks = { NULL, 0, 0, NUM_TILE_SPRITES } ;
spriteSheet ghowlSheet = { NULL, 0, 0, NUM_GHOWL_SPRITES, &ghowlFrames[0] } ; //TODO lol this is dumb as hell
spriteSheet wraithSheet = { NULL, 0, 0, NUM_WRAITH_SPRITES, &wraithFrames[0] } ;

entity ghowlEntity = {&ghowlSheet, 170, 200, 0} ;
entity wraithEntity = {&wraithSheet, 270, 210, 0} ;

//=====FUNCTION DEFS=====

bool init();
SDL_Texture* loadSpriteSheetTexture();
void freeTextures(spriteSheet* s);
void render(spriteSheet* s, int x, int y, SDL_Rect* clip);
void renderEntity(entity* ent);
int getRand(int max);
void initRand();

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

void renderEntity(entity* ent) {
    //TODO add a null check
    //TODO this is also hella dumb
    SDL_Rect* clip = &ent->sheet->frames[ent->animFrame];
 
    // TODO why is the quad this?
    SDL_Rect renderQuad = { ent->x, ent->y, ent->sheet->width, ent->sheet->height };
  
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
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ) {
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
    if(!loadSpriteSheetTexture(&blocks, "res/dungeon-tiles-cpc.png")) {
        success = false;
    } else {
        //set the sprites up, config them
        //TODO  ????????????? create some SDL_Rect to hold the sprites...
        //then in the main function you use the gRenderer to render those sprites from the rects....
    }

    //TODO refactor this
    if(!loadSpriteSheetTexture(&ghowlSheet, "res/ghowl-sprites.png")) {
        success = false;
    } else {
        //set the sprites up, config them
        //TODO  ????????????? create some SDL_Rect to hold the sprites...
        //then in the main function you use the gRenderer to render those sprites from the rects....
    }

     //TODO refactor this
    if(!loadSpriteSheetTexture(&wraithSheet, "res/wraith-sprites.png")) {
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
            
            while(!quit) {

                //handle all Q'd events 
                while (SDL_PollEvent( &e ) != 0) {
                    if (e.type == SDL_QUIT) {
                        quit = true;
                    } else if( e.type == SDL_KEYDOWN ) {
                        switch( e.key.keysym.sym) {
                            case SDLK_ESCAPE:
                            quit = true;
                            break;

                            case SDLK_UP:
                            updateFrames(&ghowlEntity);
                            break;
                            
                            case SDLK_DOWN:
                            updateFrames(&wraithEntity);
                            updateFrames(&ghowlEntity);
                            break;

                            case SDLK_LEFT:
                            updateFrames(&wraithEntity);
                            break;

                            case SDLK_RIGHT:
                            //
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
                            spriteClip.x = (tileNum * SPRITE);
                            spriteClip.y = 0;
                            spriteClip.w = SPRITE;
                            spriteClip.h = SPRITE;
                              
                            render(&blocks, x * SPRITE, y * SPRITE, &spriteClip);
                        }
                    }
                } 

                renderEntity(&ghowlEntity);
                renderEntity(&wraithEntity);

                //Update screen
                SDL_RenderPresent( gRenderer );
            }
        }
    }

    //Free resources, close SDL
    close();

    return 0; //GREAT SUCCESS!
}
