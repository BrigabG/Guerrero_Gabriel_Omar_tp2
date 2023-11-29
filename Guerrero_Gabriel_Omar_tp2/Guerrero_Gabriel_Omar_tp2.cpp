#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <string>



using namespace std;

const int WIDTH = 720;
const int HEIGHT = 720;
const int FONT_SIZE = 32;
const int BALL_SPEED = 16;
const int SPEED = 9;
const int SIZE = 16;
const double PI = 3.14159265358979323846;
const int GAME_DURATION = 120000;  // Duración máxima del juego en segundos

SDL_Renderer* renderer;
SDL_Window* window;
TTF_Font* font;
SDL_Color color;
SDL_Texture* texture_paleta;
SDL_Texture* texture_pelota;
bool running;
int frameCount, timerFPS, lastFrame, fps;
int totalTime = 0;  // Tiempo total del juego en segundos
int elapsedTime = 0;  // Tiempo transcurrido en segundos
int gameTime = 0;


SDL_Rect l_paddle, r_paddle, ball, score_board;
float velX, velY;
string score;
int l_s, r_s;
bool turn;

void serve() {
	l_paddle.y = r_paddle.y = (HEIGHT / 2) - (l_paddle.h / 2);
	if (turn) {
		ball.x = l_paddle.x + (l_paddle.w * 4);
		velX = BALL_SPEED / 2;
	}
	else {
		ball.x = r_paddle.x - (r_paddle.w * 4);
		velX = -BALL_SPEED / 2;
	}
	velY = 0;
	ball.y = HEIGHT / 2 - (SIZE / 2);
	turn = !turn;
}

void mainMenu() {
	// Limpiar la pantalla
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	// Configurar el color y la fuente del texto
	SDL_Color textColor = { 255, 255, 255 };
	TTF_Font* menuFont = TTF_OpenFont("Roboto-Regular.ttf", FONT_SIZE);

	// Configurar el mensaje del menú
	string message = "Presiona ESPACIO para comenzar.";

	// Configurar el mensaje
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(menuFont, message.c_str(), textColor);
	SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	// Configurar la posición del mensaje
	int textWidth, textHeight;
	SDL_QueryTexture(messageTexture, NULL, NULL, &textWidth, &textHeight);
	int x = (WIDTH - textWidth) / 2;
	int y = (HEIGHT - textHeight) / 2;

	// Dibujar el mensaje en la pantalla
	SDL_Rect messageRect = { x, y, textWidth, textHeight };
	SDL_RenderCopy(renderer, messageTexture, NULL, &messageRect);

	// Presentar en la pantalla
	SDL_RenderPresent(renderer);

	// Esperar a que el jugador presione ESPACIO
	SDL_Event event;
	while (SDL_WaitEvent(&event)) {
		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
			// Reiniciar variables del juego
			l_s = r_s = 0;
			serve();
			// Reiniciar el tiempo
			lastFrame = SDL_GetTicks();
			break;
		}
	}

	// Liberar recursos
	TTF_CloseFont(menuFont);
	SDL_DestroyTexture(messageTexture);
	SDL_FreeSurface(surfaceMessage);
}

void showResult() {
	// Limpiar la pantalla
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	// Configurar el color y la fuente del texto
	SDL_Color textColor = { 255, 255, 255 };
	TTF_Font* resultFont = TTF_OpenFont("Roboto-Regular.ttf", FONT_SIZE);

	// Determinar el mensaje según el resultado
	string message;
	if (l_s > r_s) {
		message = "¡Ganaste! Presiona ESPACIO para volver al menú.";
	}
	else if (r_s > l_s) {
		message = "Perdiste. Presiona ESPACIO para volver al menú.";
	}
	else {
		message = "Empate. Presiona ESPACIO para volver al menú.";
	}

	// Configurar el mensaje
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(resultFont, message.c_str(), textColor);
	SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	// Configurar la posición del mensaje
	int textWidth, textHeight;
	SDL_QueryTexture(messageTexture, NULL, NULL, &textWidth, &textHeight);
	int x = (WIDTH - textWidth) / 2;
	int y = (HEIGHT - textHeight) / 2;

	// Dibujar el mensaje en la pantalla
	SDL_Rect messageRect = { x, y, textWidth, textHeight };
	SDL_RenderCopy(renderer, messageTexture, NULL, &messageRect);

	// Presentar en la pantalla
	SDL_RenderPresent(renderer);

	// Liberar recursos
	TTF_CloseFont(resultFont);
	SDL_DestroyTexture(messageTexture);
	SDL_FreeSurface(surfaceMessage);

	// Reiniciar el tiempo
	lastFrame = SDL_GetTicks();
}



bool loadMedia() {


	// Cargar imágenes desde archivos PNG
	SDL_Surface* surface_paleta = IMG_Load("spr_paleta.png");
	SDL_Surface* surface_pelota = IMG_Load("spr_pelota.png");

	if (!surface_paleta || !surface_pelota) {
		// Manejar error al cargar imágenes
		return false;
	}

	// Crear texturas desde las superficies
	texture_paleta = SDL_CreateTextureFromSurface(renderer, surface_paleta);
	texture_pelota = SDL_CreateTextureFromSurface(renderer, surface_pelota);

	SDL_FreeSurface(surface_paleta);
	SDL_FreeSurface(surface_pelota);

	if (!texture_paleta || !texture_pelota) {
		// Manejar error al crear texturas
		return false;
	}
	
	// Establecer el tamaño de las paletas y la pelota según el tamaño real de las imágenes
	SDL_QueryTexture(texture_paleta, nullptr, nullptr, &l_paddle.w, &l_paddle.h);
	SDL_QueryTexture(texture_pelota, nullptr, nullptr, &ball.w, &ball.h);

	// Establecer la posición inicial de las paletas
	l_paddle.x = 32;
	l_paddle.y = (HEIGHT / 2) - (l_paddle.h / 2);
	r_paddle = l_paddle;
	r_paddle.x = WIDTH - r_paddle.w - 32;
	return true;
}



void write(string text, int x, int y) {
	SDL_Surface* surface;
	SDL_Texture* texture;
	const char* t = text.c_str();
	surface = TTF_RenderText_Solid(font, t, color);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	score_board.w = surface->w;
	score_board.h = surface->h;
	score_board.x = x - score_board.w;
	score_board.y = y - score_board.h;
	SDL_FreeSurface(surface);
	SDL_RenderCopy(renderer, texture, NULL, &score_board);
	SDL_DestroyTexture(texture);
}

void update() {
	if (SDL_HasIntersection(&ball, &r_paddle)) {
		double rel = (r_paddle.y + (r_paddle.h / 2)) - (ball.y + (SIZE / 2));
		double norm = rel / (r_paddle.h / 2);
		double bounce = norm * (5 * PI / 12);
		velX = -BALL_SPEED * cos(bounce);
		velY = BALL_SPEED * -sin(bounce);
	}
	if (SDL_HasIntersection(&ball, &l_paddle)) {
		double rel = (l_paddle.y + (l_paddle.h / 2)) - (ball.y + (SIZE / 2));
		double norm = rel / (l_paddle.h / 2);
		double bounce = norm * (5 * PI / 12);
		velX = BALL_SPEED * cos(bounce);
		velY = BALL_SPEED * -sin(bounce);
	}
	if (ball.y > r_paddle.y + (r_paddle.h / 2)) r_paddle.y += SPEED;
	if (ball.y < r_paddle.y + (r_paddle.h / 2)) r_paddle.y -= SPEED;
	if (ball.x <= 0) { r_s++; serve(); }
	if (ball.x + SIZE >= WIDTH) { l_s++; serve(); }
	if (ball.y <= 0 || ball.y + SIZE >= HEIGHT) velY = -velY;
	ball.x += velX;
	ball.y += velY;
	score = to_string(l_s) + "   " + to_string(r_s);
	if (l_paddle.y < 0)l_paddle.y = 0;
	if (l_paddle.y + l_paddle.h > HEIGHT)l_paddle.y = HEIGHT - l_paddle.h;
	if (r_paddle.y < 0)r_paddle.y = 0;
	if (r_paddle.y + r_paddle.h > HEIGHT)r_paddle.y = HEIGHT - r_paddle.h;

	
	// Actualizar el tiempo transcurrido
	if (elapsedTime < GAME_DURATION) {
		elapsedTime += 1000 / 60; // Asumiendo 60 cuadros por segundo
	}
	else {
		// Aquí puedes agregar lógica para manejar el final del juego
		// Por ejemplo, mostrar un mensaje de juego terminado, reiniciar, etc.
	}
}

void input() {
	SDL_Event e;
	const Uint8* keystates = SDL_GetKeyboardState(NULL);
	while (SDL_PollEvent(&e)) if (e.type == SDL_QUIT) running = false;
	if (keystates[SDL_SCANCODE_ESCAPE]) running = false;
	if (keystates[SDL_SCANCODE_UP]) l_paddle.y -= SPEED;
	if (keystates[SDL_SCANCODE_DOWN]) l_paddle.y += SPEED;
}

void render() {
	SDL_RenderClear(renderer);  // Limpiar la pantalla
	
	SDL_RenderCopy(renderer, texture_paleta, nullptr, &l_paddle);
	SDL_RenderCopy(renderer, texture_paleta, nullptr, &r_paddle);
	SDL_RenderCopy(renderer, texture_pelota, nullptr, &ball);

	// Renderizar el marcador
	write(score, WIDTH / 2 + FONT_SIZE, FONT_SIZE * 2);
	write("Time: " + to_string((GAME_DURATION - elapsedTime) / 1000) + "s", WIDTH / 2 - FONT_SIZE * 3, FONT_SIZE * 2);

	SDL_RenderPresent(renderer);
}

void close() {
	// Liberar recursos (texturas, fuentes, etc.)
	SDL_DestroyTexture(texture_paleta);
	SDL_DestroyTexture(texture_pelota);
	
	TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
	SDL_DestroyTexture(texture_paleta);
	SDL_DestroyTexture(texture_pelota);
	IMG_Quit();
}



int SDL_main(int argc, char* argv[]) {
	
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		cerr << "Failed to initialize SDL: " << SDL_GetError() << endl;
		return 1;
	}

	if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) {
		cerr << "Failed to create window and renderer: " << SDL_GetError() << endl;
		SDL_Quit();
		return 1;
	}

	TTF_Init();
	font = TTF_OpenFont("Roboto-Regular.ttf", FONT_SIZE);
	if (!font) {
		cerr << "Failed to load font: " << TTF_GetError() << endl;
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		TTF_Quit();  // Cerrar TTF antes de salir
		SDL_Quit();
		return 1;
	}
	
	// Llamada a loadMedia para cargar las imágenes y texturas
	if (!loadMedia()) {
		cerr << "Failed to load media." << endl;
		TTF_CloseFont(font);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
		return 1;
	}

	running = 1;
	static int lastTime = 0;

	color.r = color.g = color.b = 255;
	l_s = r_s = 0;
	l_paddle.x = 32; l_paddle.h = HEIGHT / 4;
	l_paddle.y = (HEIGHT / 2) - (l_paddle.h / 2);
	l_paddle.w = 12;
	r_paddle = l_paddle;
	r_paddle.x = WIDTH - r_paddle.w - 32;
	ball.w = ball.h = SIZE;

	serve();

	// Variables para controlar el tiempo
	int currentTime, deltaTime;
	
	// Mostrar la pantalla principal al inicio
	mainMenu();

	bool gameStarted = false;
	SDL_Event event;

	while (!gameStarted) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
				gameStarted = true;
			}
		}
	}


	while (running) {
		currentTime = SDL_GetTicks();
		deltaTime = currentTime - lastFrame;

		if (deltaTime < (1000 / 60)) {
			SDL_Delay((1000 / 60) - deltaTime);
			currentTime = SDL_GetTicks();
			deltaTime = currentTime - lastFrame;
		}

		lastFrame = currentTime;

		// Resto del bucle principal
		update();
		input();
		render();
		
		// Verificar si el juego ha terminado (120 segundos)
		if (currentTime >= 120000) {
			showResult();
			// Esperar a que el jugador presione ESPACIO
			SDL_Event event;
			while (SDL_WaitEvent(&event)) {
				if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
					
					// Reiniciar el juego
					l_s = r_s = 0;
					serve();
					break;
				}
			}
			// Volver al menú principal
			mainMenu();
		}
	}

	
		
	// Liberar recursos al salir del programa
	close();

	return 0;
}