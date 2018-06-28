#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//The window to render to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

bool init() { return true; }
bool loadMedia() { return true; }
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
                    }
                }

                //Clear the screen/draw background
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF); 
                SDL_RenderClear( gRenderer );
                
                //Draw things (Render things to gRenderer)

                //Update screen
                SDL_RenderPresent( gRenderer );

            }
        }
    }

    //Free resources, close SDL
    close();

    return 0; //GREAT SUCCESS!
}
