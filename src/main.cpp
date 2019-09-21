/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

//Analog joystick dead zone
const int JOYSTICK_DEAD_ZONE = 8000;

int x_mouse_pos = 0;
int y_mouse_pos = 0;

const int Button_position_X = 55;
const int Button_position_Y = 75;

const int Text_position_X = 100;
const int Text_position_Y = 100;

bool invert_button = false;

int ButtonPressedCount = 0;

#define SDL_TTF_H
#define _FONT    "assets/OpenSans-Bold.ttf"

SDL_Color BlackColour = {0, 0, 0};  // Black colour
SDL_Color WhiteColour = {255, 255, 255};  // Black colour

//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadCursor( std::string path );
		bool loadButton( std::string path );

		#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
		//Creates image from font string
		bool LoadButtonText( std::string textureText, SDL_Color textColor );
		bool LoadCounterText( std::string textureText, SDL_Color textColor );
		#endif

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );
		
		//Renders texture at given point
		void render(SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Gets image dimensions
		int CursorgetWidth();
		int CursorgetHeight();
		int ButtongetWidth();
		int ButtongetHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;
		SDL_Texture* fTexture;
		SDL_Texture* fTexture_count;
		SDL_Texture* bTexture;

		//Cursor dimensions
		int mWidth;
		int mHeight;

		//Button dimensions
		int bWidth;
		int bHeight;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture gArrowTexture;
LTexture gTextTexture;
LTexture gTextTexture_count;
LTexture gButtonTexture;
LTexture gButtonInvertedTexture;

//Game Controller 1 handler
SDL_Joystick* gGameController = NULL;

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	bTexture = NULL;
	mWidth = 0;
	mHeight = 0;
	bWidth = 0;
	bHeight = 0;

	#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
	fTexture = NULL;
	fTexture_count = NULL;
	#endif
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadCursor( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	
	return mTexture != NULL;
}

bool LTexture::loadButton( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			bWidth = loadedSurface->w;
			bHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	bTexture = newTexture;
	
	return bTexture != NULL;
}

#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
bool LTexture::LoadButtonText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();
	TTF_Font* font = TTF_OpenFont(_FONT, 20);
	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( font, textureText.c_str(), textColor );
	
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        fTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( fTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}
	//Return success
	return fTexture != NULL;
}
bool LTexture::LoadCounterText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();
	TTF_Font* font = TTF_OpenFont(_FONT, 20);
	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( font, textureText.c_str(), textColor );
	
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        fTexture_count = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( fTexture_count == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}
	//Return success
	return fTexture_count != NULL;
}
#endif

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
		bWidth = 0;
		bHeight = 0;
	}

	if( bTexture != NULL )
	{
		SDL_DestroyTexture( bTexture );
		bTexture = NULL;
	}

	#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
	if( fTexture != NULL )
	{
		SDL_DestroyTexture( fTexture );
		fTexture = NULL;
	}
	if( fTexture_count != NULL )
	{
		SDL_DestroyTexture( fTexture_count );
		fTexture_count = NULL;
	}
	#endif
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );

	#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
	SDL_SetTextureColorMod( fTexture, red, green, blue );
	SDL_SetTextureColorMod( fTexture_count, red, green, blue );
	#endif
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );

	#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
	SDL_SetTextureBlendMode( fTexture, blending );
	SDL_SetTextureBlendMode( fTexture_count, blending );
	#endif
}
		
void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );

	#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
	SDL_SetTextureAlphaMod( fTexture, alpha );
	SDL_SetTextureAlphaMod( fTexture_count, alpha );
	#endif
}

void LTexture::render(SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x_mouse_pos, y_mouse_pos, mWidth, mHeight };
	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	
	//Render Mouse to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
	
	#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
	SDL_Rect renderText = { Text_position_X, Text_position_Y, mWidth, mHeight };
	SDL_Rect renderText2 = { Text_position_X + 200, Text_position_Y, mWidth, mHeight };

	//Render Text to screen
	SDL_RenderCopyEx( gRenderer, fTexture, clip, &renderText, angle, center, flip );
	SDL_RenderCopyEx( gRenderer, fTexture_count, clip, &renderText2, angle, center, flip );
	#endif

	SDL_Rect renderButton = { Button_position_X, Button_position_Y, bWidth, bHeight };
	//Render Button to screen
	SDL_RenderCopyEx( gRenderer, bTexture, clip, &renderButton, angle, center, flip );

	
}

int LTexture::CursorgetWidth()
{
	return mWidth;
}

int LTexture::CursorgetHeight()
{
	return mHeight;
}

int LTexture::ButtongetWidth()
{
	return bWidth;
}

int LTexture::ButtongetHeight()
{
	return bHeight;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Check for joysticks
		if( SDL_NumJoysticks() < 1 )
		{
			printf( "Warning: No joysticks connected!\n" );
		}
		else
		{
			//Load joystick
			gGameController = SDL_JoystickOpen( 0 );
			if( gGameController == NULL )
			{
				printf( "Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError() );
			}
		}

		if (TTF_Init() != 0) 
		{
			printf( "TTF_Init Error");
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load arrow texture
	if( !gArrowTexture.loadCursor( "assets/arrow.png" ) )
	{
		printf( "Failed to load arrow texture!\n" );
		success = false;
	}

	#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
	if( !gTextTexture.LoadButtonText("Click Me",  WhiteColour) )
	{
		printf( "Failed to load font!\n" );
		success = false;
	}
	if( !gTextTexture_count.LoadCounterText("Button Pressed Count: " + std::to_string(ButtonPressedCount),  BlackColour) )
	{
		printf( "Failed to load font!\n" );
		success = false;
	}
	#endif

	if( !gButtonTexture.loadButton( "assets/button.png" ) )
	{
		printf( "Failed to load arrow texture!\n" );
		success = false;
	}

	if( !gButtonInvertedTexture.loadButton( "assets/button_invert.png" ) )
	{
		printf( "Failed to load arrow texture!\n" );
		success = false;
	}
	
	return success;
}

void close()
{
	//Free loaded images
	gArrowTexture.free();
	gTextTexture.free();
	gTextTexture_count.free();
	gButtonTexture.free();

	//Close game controller
	SDL_JoystickClose( gGameController );
	gGameController = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//Normalized direction
			int xDir = 0;
			int yDir = 0;
			x_mouse_pos = ( SCREEN_WIDTH - gArrowTexture.CursorgetWidth() ) / 2;
			y_mouse_pos = ( SCREEN_HEIGHT - gArrowTexture.CursorgetHeight() ) / 2;

			//While application is running
			while( !quit )
			{
				
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
					else if( e.type == SDL_JOYBUTTONDOWN )
					{
						if(e.jbutton.button == 0) // Key (A)
						{
							//Get mouse position over button positioon
							if(x_mouse_pos > (Button_position_X) 
							&& x_mouse_pos < ((SCREEN_WIDTH - gButtonTexture.ButtongetWidth() ) / 2.7) 
							&& y_mouse_pos > (Button_position_Y) 
							&& y_mouse_pos < ((SCREEN_HEIGHT - gButtonTexture.ButtongetHeight() ) / 3.7) )
							{
								invert_button = true;
								ButtonPressedCount++;
							}
							
							
						}
					}
					else if( e.type == SDL_JOYBUTTONUP )
					{
						if(e.jbutton.button == 0) // Key (A)
						{
							invert_button = false;
						}
					}
					else if( e.type == SDL_JOYAXISMOTION )
					{
						//Motion on controller 0
						if( e.jaxis.which == 0 )
						{						
							//X axis motion
							if( e.jaxis.axis == 0 )
							{
								//Left of dead zone
								if( e.jaxis.value < -JOYSTICK_DEAD_ZONE )
								{
									if (x_mouse_pos > 0)
									{
										x_mouse_pos -= 10;
									}	
								}
								//Right of dead zone
								else if( e.jaxis.value > JOYSTICK_DEAD_ZONE )
								{
									if (x_mouse_pos <= (SCREEN_WIDTH - 30))
									{
										x_mouse_pos += 10;
									}
								}
								else
								{
									//xDir = 0;
								}
							}
							//Y axis motion
							else if( e.jaxis.axis == 1 )
							{
								//Below of dead zone
								if( e.jaxis.value < -JOYSTICK_DEAD_ZONE )
								{
									if (y_mouse_pos > 0)
									{
										y_mouse_pos -= 10;
									}
									
									
								}
								//Above of dead zone
								else if( e.jaxis.value > JOYSTICK_DEAD_ZONE )
								{
									if (y_mouse_pos <= (SCREEN_HEIGHT - 30))
									{
										y_mouse_pos += 10;
									}
								}
								else
								{
									//yDir = 0;
								}
							}
						}
					}
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				//Calculate angle
				double joystickAngle = atan2( (double)yDir, (double)xDir ) * ( 180.0 / M_PI );
				
				//Correct angle
				if( xDir == 0 && yDir == 0 )
				{
					joystickAngle = 0;
				}

				//Render 

				if (!invert_button)
				{
					gButtonTexture.render(NULL, 0);
				}
				else
				{
					gButtonInvertedTexture.render(NULL, 0);
				}
				gTextTexture.render(NULL, 0);
				//gTextTexture_count.render(NULL, 0);
				gArrowTexture.render(NULL, joystickAngle );
				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
