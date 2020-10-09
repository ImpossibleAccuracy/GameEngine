#include "Game.h"

Window::Window(string name, Rect rect, Uint32 flags) : rect(new Rect) {
	this->sdlwin = SDL_CreateWindow(
		name.c_str(),
		rect.x, rect.y, rect.w, rect.h,
		flags);
	this->renderer = SDL_CreateRenderer(sdlwin, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (sdlwin == nullptr or renderer == nullptr) {
		throw GameExceptions::GameInitializationError(SDL_GetError()); }

	this->rect->x = rect.x;
	this->rect->y = rect.y;
	SDL_GetWindowSize(sdlwin, &this->rect->w, &this->rect->h);
}
void Window::clear() {
	SDL_RenderClear(renderer);
}
void Window::update() {
	SDL_RenderPresent(renderer);
	SDL_UpdateWindowSurface(sdlwin);
}

Game::Game() {
    this->status = "";
    this->mouse = nullptr;
    this->keyboard = nullptr;
    this->window = nullptr;
    this->runing = false;
    this->closed = false;
	this->exception = nullptr;
	this->components = {};
	//this->exceptionHandler = boost::python::object([](GameExceptions::BaseGameException&) -> void {});


	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		throw GameExceptions::GameInitializationError(SDL_GetError()); }
	if (TTF_Init() != 0) {
		throw GameExceptions::GameInitializationError(TTF_GetError()); }
	int flags = IMG_INIT_PNG;
	if (!(IMG_Init(flags) & flags)) {
		throw GameExceptions::GameInitializationError(IMG_GetError()); }
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
}
Game::~Game() {
    this->close();
}

void Game::init(string name) {
    this->window = boost::shared_ptr<Window>(new Window(
        name,
        Rect(),
        Window::FULLSCREEN | Window::SHOWN | Window::OPENGL));

    this->mouse = boost::shared_ptr<Mouse>(new Mouse);
    this->keyboard = boost::shared_ptr<Keyboard>(new Keyboard);
}
void Game::init2(string name, int width, int height) {
	this->window = boost::shared_ptr<Window>(new Window(
		name,
		Rect(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height),
		Window::SHOWN | Window::OPENGL));

	this->mouse = boost::shared_ptr<Mouse>(new Mouse);
	this->keyboard = boost::shared_ptr<Keyboard>(new Keyboard);
}
void Game::close() {
    if (!this->closed) {
		this->closed = true;
		this->runing = false;
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();

    }
}
void Game::setup() {
	for (int i = 0; i < this->components.size(); i++) {
		this->components[i]->loadObjects();
	}
    this->closed = false;
    this->runing = true;
}

void Game::draw() {
	for (int i = 0; i < this->components.size(); i++) {
		if (this->components[i]->canRender() && this->exception == nullptr) {
			this->components[i]->draw();
		}
	}
}
void Game::update(){
	SDL_Event e;
	ostringstream param;
	while (SDL_PollEvent(&e) != NULL) {
		switch (e.type) {
		case SDL_QUIT:
			throw GameExceptions::CloseGameException("User close game!");
			break;

		case SDL_KEYUP:
			this->keyboard->onKeyUp(e.key.keysym.sym);

			for (int i = 0; i < this->components.size(); i++) {
				if (this->components[i]->canRender()) {
					this->components[i]->onKeyUp(e.key.keysym.sym);
				}
			}
			break;
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_ESCAPE)
				this->throwException(new GameExceptions::CloseGameException("User close game with escape key!"));

			this->keyboard->onKeyDown(e.key.keysym.sym);

			for (int i = 0; i < this->components.size(); i++) {
				if (this->components[i]->canRender()) {
					this->components[i]->onKeyDown(e.key.keysym.sym);
				}
			}
			break;

		case SDL_MOUSEBUTTONUP:
			this->mouse->onMouseUp(e);

			for (int i = 0; i < this->components.size(); i++) {
				if (this->components[i]->canRender()) {
					this->components[i]->onMouseUp(e.motion.x, e.motion.y);
				}
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			this->mouse->onMouseDown(e);

			for (int i = 0; i < this->components.size(); i++) {
				if (this->components[i]->canRender()) {
					this->components[i]->onMouseDown(e.motion.x, e.motion.y);
				}
			}
			break;
		case SDL_MOUSEMOTION:
			this->mouse->onMouseMotion(e);

			for (int i = 0; i < this->components.size(); i++) {
				if (this->components[i]->canRender()) {
					this->components[i]->onMouseMotion(e.motion.x, e.motion.y);
				}
			}
			break;
		}
	}

	for (int i = 0; i < this->components.size(); i++) {
		if (this->components[i]->canRender() && this->exception == nullptr) {
			this->components[i]->update();
		}
	}
}

void Game::setExceptionsHandler(func callback) {
	this->exceptionHandler = callback;
}
void Game::throwException(GameExceptions::BaseGameException* exc) {
	using namespace GameExceptions;
	if (exc->type() == "CloseGameException") {
		this->exception = new CloseGameException(exc->what());
	}
	else if (exc->type() == "GameOverException") {
		this->exception = new GameOverException(exc->what());
	}
	else {
		this->exception = new BaseGameException(exc->what());
	}
}
void Game::setStatus(string status) {
	this->status = status;
}
void Game::notify(GameComponent* sender, string to, string msg) {
	try {
		if (to == "all") {
			for (int i = 0; i < this->components.size(); i++) {
				GameComponent* component = this->components[i];
				if (sender == nullptr || component->getId() != sender->getId()) {
					component->onNotify(boost::shared_ptr<GameComponent>(sender), msg);
				}
			}
		}
		else {
			for (int i = 0; i < this->components.size(); i++) {
				GameComponent* component = this->components[i];
				if (component->componentName == to) {
					component->onNotify(boost::shared_ptr<GameComponent>(sender), msg);
				}
			}
		}
	}
	catch (const std::exception& err) {
		throw GameExceptions::ErrorInNotify(err.what());
	}
}
void Game::addComponent(GameComponent* component) {
	this->components.push_back(component);
}

GameComponent::GameComponent(boost::shared_ptr<Game>& game) {
    this->game = game;
	this->id = GameComponent::componentsCount;
	GameComponent::componentsCount++;
}
int GameComponent::getId() {
	return this->id;
}
int GameComponent::componentsCount = 0;


Keyboard::Keyboard() {
    this->keysCount = 115;
    this->keyNames = new string[this->keysCount];
    this->pressedkeys = new bool[this->keysCount];
    for (int i = 0; i < keysCount; i++)
        this->pressedkeys[i] = false;

    this->keyNames[0] = "backspace";
    this->keyNames[1] = "tab";
    this->keyNames[2] = "clear";
    this->keyNames[3] = "return";
    this->keyNames[4] = "pause";
    this->keyNames[5] = "escape";
    this->keyNames[6] = "space";
    this->keyNames[7] = "exclaim";
    this->keyNames[8] = "quotedbl";
    this->keyNames[9] = "hash";
    this->keyNames[10] = "dollar";
    this->keyNames[11] = "ampersand";
    this->keyNames[12] = "quote";
    this->keyNames[13] = "left parenthesis";
    this->keyNames[14] = "right parenthesis";
    this->keyNames[15] = "asterisk";
    this->keyNames[16] = "plus sign";
    this->keyNames[17] = "comma";
    this->keyNames[18] = "minus sign";
    this->keyNames[19] = "period";
    this->keyNames[20] = "forward slash";
    this->keyNames[21] = "0";
    this->keyNames[22] = "1";
    this->keyNames[23] = "2";
    this->keyNames[24] = "3";
    this->keyNames[25] = "4";
    this->keyNames[26] = "5";
    this->keyNames[27] = "6";
    this->keyNames[28] = "7";
    this->keyNames[29] = "8";
    this->keyNames[30] = "9";
    this->keyNames[31] = "colon";
    this->keyNames[32] = "semicolon";
    this->keyNames[33] = "less-than sign";
    this->keyNames[34] = "equals sign";
    this->keyNames[35] = "greater-than sign";
    this->keyNames[36] = "question mark";
    this->keyNames[37] = "at";
    this->keyNames[38] = "left bracket";
    this->keyNames[39] = "backslash";
    this->keyNames[40] = "right bracket";
    this->keyNames[41] = "caret";
    this->keyNames[42] = "underscore";
    this->keyNames[43] = "grave";
    this->keyNames[44] = "a";
    this->keyNames[45] = "b";
    this->keyNames[46] = "c";
    this->keyNames[47] = "d";
    this->keyNames[48] = "e";
    this->keyNames[49] = "f";
    this->keyNames[50] = "g";
    this->keyNames[51] = "h";
    this->keyNames[52] = "i";
    this->keyNames[53] = "j";
    this->keyNames[54] = "k";
    this->keyNames[55] = "l";
    this->keyNames[56] = "m";
    this->keyNames[57] = "n";
    this->keyNames[58] = "o";
    this->keyNames[59] = "p";
    this->keyNames[60] = "q";
    this->keyNames[61] = "r";
    this->keyNames[62] = "s";
    this->keyNames[63] = "t";
    this->keyNames[64] = "u";
    this->keyNames[65] = "v";
    this->keyNames[66] = "w";
    this->keyNames[67] = "x";
    this->keyNames[68] = "y";
    this->keyNames[69] = "z";
    this->keyNames[70] = "delete";
    this->keyNames[71] = "keypad period";
    this->keyNames[72] = "keypad divide";
    this->keyNames[73] = "keypad multiply";
    this->keyNames[74] = "keypad minus";
    this->keyNames[75] = "keypad plus";
    this->keyNames[76] = "keypad enter";
    this->keyNames[77] = "keypad equals";
    this->keyNames[78] = "up arrow";
    this->keyNames[79] = "down arrow";
    this->keyNames[80] = "right arrow";
    this->keyNames[81] = "left arrow";
    this->keyNames[82] = "insert";
    this->keyNames[83] = "home";
    this->keyNames[84] = "end";
    this->keyNames[85] = "page up";
    this->keyNames[86] = "page down";
    this->keyNames[87] = "F1";
    this->keyNames[88] = "F2";
    this->keyNames[89] = "F3";
    this->keyNames[90] = "F4";
    this->keyNames[91] = "F5";
    this->keyNames[92] = "F6";
    this->keyNames[93] = "F7";
    this->keyNames[94] = "F8";
    this->keyNames[95] = "F9";
    this->keyNames[96] = "F10";
    this->keyNames[97] = "F11";
    this->keyNames[98] = "F12";
    this->keyNames[99] = "F13";
    this->keyNames[100] = "F14";
    this->keyNames[101] = "F15";
    this->keyNames[102] = "capslock";
    this->keyNames[103] = "right shift";
    this->keyNames[104] = "left shift";
    this->keyNames[105] = "right ctrl";
    this->keyNames[106] = "left ctrl";
    this->keyNames[107] = "ight alt";
    this->keyNames[108] = "left alt";
    this->keyNames[109] = "mode shift";
    this->keyNames[110] = "help";
    this->keyNames[111] = "SysRq";
    this->keyNames[112] = "menu";
    this->keyNames[113] = "power";
    this->keyNames[114] = "num";
}
bool Keyboard::isKeyPressed(string keyName) {
    int pos = this->getKeyPos(keyName);
    if (pos == -1)
        throw exception(string("Key " + keyName + " not exist!").c_str());
    else
        return this->pressedkeys[pos];
}

void Keyboard::onKeyUp(SDL_Keycode key) {
	switch (key) {
	case SDLK_BACKSPACE:
		this->pressedkeys[0] = false;
		break;
	case SDLK_TAB:
		this->pressedkeys[1] = false;
		break;
	case SDLK_CLEAR:
		this->pressedkeys[2] = false;
		break;
	case SDLK_RETURN:
		this->pressedkeys[3] = false;
		break;
	case SDLK_PAUSE:
		this->pressedkeys[4] = false;
		break;
	case SDLK_ESCAPE:
		this->pressedkeys[5] = false;
		break;
	case SDLK_SPACE:
		this->pressedkeys[6] = false;
		break;
	case SDLK_EXCLAIM:
		this->pressedkeys[7] = false;
		break;
	case SDLK_QUOTEDBL:
		this->pressedkeys[8] = false;
		break;
	case SDLK_HASH:
		this->pressedkeys[9] = false;
		break;
	case SDLK_DOLLAR:
		this->pressedkeys[10] = false;
		break;
	case SDLK_AMPERSAND:
		this->pressedkeys[11] = false;
		break;
	case SDLK_QUOTE:
		this->pressedkeys[12] = false;
		break;
	case SDLK_LEFTPAREN:
		this->pressedkeys[13] = false;
		break;
	case SDLK_RIGHTPAREN:
		this->pressedkeys[14] = false;
		break;
	case SDLK_ASTERISK:
		this->pressedkeys[15] = false;
		break;
	case SDLK_PLUS:
		this->pressedkeys[16] = false;
		break;
	case SDLK_COMMA:
		this->pressedkeys[17] = false;
		break;
	case SDLK_MINUS:
		this->pressedkeys[18] = false;
		break;
	case SDLK_PERIOD:
		this->pressedkeys[19] = false;
		break;
	case SDLK_SLASH:
		this->pressedkeys[20] = false;
		break;
	case SDLK_0:
		this->pressedkeys[21] = false;
		break;
	case SDLK_1:
		this->pressedkeys[22] = false;
		break;
	case SDLK_2:
		this->pressedkeys[23] = false;
		break;
	case SDLK_3:
		this->pressedkeys[24] = false;
		break;
	case SDLK_4:
		this->pressedkeys[25] = false;
		break;
	case SDLK_5:
		this->pressedkeys[26] = false;
		break;
	case SDLK_6:
		this->pressedkeys[27] = false;
		break;
	case SDLK_7:
		this->pressedkeys[28] = false;
		break;
	case SDLK_8:
		this->pressedkeys[29] = false;
		break;
	case SDLK_9:
		this->pressedkeys[30] = false;
		break;
	case SDLK_COLON:
		this->pressedkeys[31] = false;
		break;
	case SDLK_SEMICOLON:
		this->pressedkeys[32] = false;
		break;
	case SDLK_LESS:
		this->pressedkeys[33] = false;
		break;
	case SDLK_EQUALS:
		this->pressedkeys[34] = false;
		break;
	case SDLK_GREATER:
		this->pressedkeys[35] = false;
		break;
	case SDLK_QUESTION:
		this->pressedkeys[36] = false;
		break;
	case SDLK_AT:
		this->pressedkeys[37] = false;
		break;
	case SDLK_LEFTBRACKET:
		this->pressedkeys[38] = false;
		break;
	case SDLK_BACKSLASH:
		this->pressedkeys[39] = false;
		break;
	case SDLK_RIGHTBRACKET:
		this->pressedkeys[40] = false;
		break;
	case SDLK_CARET:
		this->pressedkeys[41] = false;
		break;
	case SDLK_UNDERSCORE:
		this->pressedkeys[42] = false;
		break;
	case SDLK_BACKQUOTE:
		this->pressedkeys[43] = false;
		break;
	case SDLK_a:
		this->pressedkeys[44] = false;
		break;
	case SDLK_b:
		this->pressedkeys[45] = false;
		break;
	case SDLK_c:
		this->pressedkeys[46] = false;
		break;
	case SDLK_d:
		this->pressedkeys[47] = false;
		break;
	case SDLK_e:
		this->pressedkeys[48] = false;
		break;
	case SDLK_f:
		this->pressedkeys[49] = false;
		break;
	case SDLK_g:
		this->pressedkeys[50] = false;
		break;
	case SDLK_h:
		this->pressedkeys[51] = false;
		break;
	case SDLK_i:
		this->pressedkeys[52] = false;
		break;
	case SDLK_j:
		this->pressedkeys[53] = false;
		break;
	case SDLK_k:
		this->pressedkeys[54] = false;
		break;
	case SDLK_l:
		this->pressedkeys[55] = false;
		break;
	case SDLK_m:
		this->pressedkeys[56] = false;
		break;
	case SDLK_n:
		this->pressedkeys[57] = false;
		break;
	case SDLK_o:
		this->pressedkeys[58] = false;
		break;
	case SDLK_p:
		this->pressedkeys[59] = false;
		break;
	case SDLK_q:
		this->pressedkeys[60] = false;
		break;
	case SDLK_r:
		this->pressedkeys[61] = false;
		break;
	case SDLK_s:
		this->pressedkeys[62] = false;
		break;
	case SDLK_t:
		this->pressedkeys[63] = false;
		break;
	case SDLK_u:
		this->pressedkeys[64] = false;
		break;
	case SDLK_v:
		this->pressedkeys[65] = false;
		break;
	case SDLK_w:
		this->pressedkeys[66] = false;
		break;
	case SDLK_x:
		this->pressedkeys[67] = false;
		break;
	case SDLK_y:
		this->pressedkeys[68] = false;
		break;
	case SDLK_z:
		this->pressedkeys[69] = false;
		break;
	case SDLK_DELETE:
		this->pressedkeys[70] = false;
		break;
	case SDLK_KP_PERIOD:
		this->pressedkeys[71] = false;
		break;
	case SDLK_KP_DIVIDE:
		this->pressedkeys[72] = false;
		break;
	case SDLK_KP_MULTIPLY:
		this->pressedkeys[73] = false;
		break;
	case SDLK_KP_MINUS:
		this->pressedkeys[74] = false;
		break;
	case SDLK_KP_PLUS:
		this->pressedkeys[75] = false;
		break;
	case SDLK_KP_ENTER:
		this->pressedkeys[76] = false;
		break;
	case SDLK_KP_EQUALS:
		this->pressedkeys[77] = false;
		break;
	case SDLK_UP:
		this->pressedkeys[78] = false;
		break;
	case SDLK_DOWN:
		this->pressedkeys[79] = false;
		break;
	case SDLK_RIGHT:
		this->pressedkeys[80] = false;
		break;
	case SDLK_LEFT:
		this->pressedkeys[81] = false;
		break;
	case SDLK_INSERT:
		this->pressedkeys[82] = false;
		break;
	case SDLK_HOME:
		this->pressedkeys[83] = false;
		break;
	case SDLK_END:
		this->pressedkeys[84] = false;
		break;
	case SDLK_PAGEUP:
		this->pressedkeys[85] = false;
		break;
	case SDLK_PAGEDOWN:
		this->pressedkeys[86] = false;
		break;
	case SDLK_F1:
		this->pressedkeys[87] = false;
		break;
	case SDLK_F2:
		this->pressedkeys[88] = false;
		break;
	case SDLK_F3:
		this->pressedkeys[89] = false;
		break;
	case SDLK_F4:
		this->pressedkeys[90] = false;
		break;
	case SDLK_F5:
		this->pressedkeys[91] = false;
		break;
	case SDLK_F6:
		this->pressedkeys[92] = false;
		break;
	case SDLK_F7:
		this->pressedkeys[93] = false;
		break;
	case SDLK_F8:
		this->pressedkeys[94] = false;
		break;
	case SDLK_F9:
		this->pressedkeys[95] = false;
		break;
	case SDLK_F10:
		this->pressedkeys[96] = false;
		break;
	case SDLK_F11:
		this->pressedkeys[97] = false;
		break;
	case SDLK_F12:
		this->pressedkeys[98] = false;
		break;
	case SDLK_F13:
		this->pressedkeys[99] = false;
		break;
	case SDLK_F14:
		this->pressedkeys[100] = false;
		break;
	case SDLK_F15:
		this->pressedkeys[101] = false;
		break;
	case SDLK_CAPSLOCK:
		this->pressedkeys[102] = false;
		break;
	case SDLK_RSHIFT:
		this->pressedkeys[103] = false;
		break;
	case SDLK_LSHIFT:
		this->pressedkeys[104] = false;
		break;
	case SDLK_RCTRL:
		this->pressedkeys[105] = false;
		break;
	case SDLK_LCTRL:
		this->pressedkeys[106] = false;
		break;
	case SDLK_RALT:
		this->pressedkeys[107] = false;
		break;
	case SDLK_LALT:
		this->pressedkeys[108] = false;
		break;
	case SDLK_MODE:
		this->pressedkeys[109] = false;
		break;
	case SDLK_HELP:
		this->pressedkeys[110] = false;
		break;
	case SDLK_SYSREQ:
		this->pressedkeys[111] = false;
		break;
	case SDLK_MENU:
		this->pressedkeys[112] = false;
		break;
	case SDLK_POWER:
		this->pressedkeys[113] = false;
		break;
	case KMOD_NUM:
		this->pressedkeys[114] = false;
		break;
	}
}
void Keyboard::onKeyDown(SDL_Keycode key) {
	switch (key) {
	case SDLK_BACKSPACE:
		this->pressedkeys[0] = true;
		break;
	case SDLK_TAB:
		this->pressedkeys[1] = true;
		break;
	case SDLK_CLEAR:
		this->pressedkeys[2] = true;
		break;
	case SDLK_RETURN:
		this->pressedkeys[3] = true;
		break;
	case SDLK_PAUSE:
		this->pressedkeys[4] = true;
		break;
	case SDLK_ESCAPE:
		this->pressedkeys[5] = true;
		break;
	case SDLK_SPACE:
		this->pressedkeys[6] = true;
		break;
	case SDLK_EXCLAIM:
		this->pressedkeys[7] = true;
		break;
	case SDLK_QUOTEDBL:
		this->pressedkeys[8] = true;
		break;
	case SDLK_HASH:
		this->pressedkeys[9] = true;
		break;
	case SDLK_DOLLAR:
		this->pressedkeys[10] = true;
		break;
	case SDLK_AMPERSAND:
		this->pressedkeys[11] = true;
		break;
	case SDLK_QUOTE:
		this->pressedkeys[12] = true;
		break;
	case SDLK_LEFTPAREN:
		this->pressedkeys[13] = true;
		break;
	case SDLK_RIGHTPAREN:
		this->pressedkeys[14] = true;
		break;
	case SDLK_ASTERISK:
		this->pressedkeys[15] = true;
		break;
	case SDLK_PLUS:
		this->pressedkeys[16] = true;
		break;
	case SDLK_COMMA:
		this->pressedkeys[17] = true;
		break;
	case SDLK_MINUS:
		this->pressedkeys[18] = true;
		break;
	case SDLK_PERIOD:
		this->pressedkeys[19] = true;
		break;
	case SDLK_SLASH:
		this->pressedkeys[20] = true;
		break;
	case SDLK_0:
		this->pressedkeys[21] = true;
		break;
	case SDLK_1:
		this->pressedkeys[22] = true;
		break;
	case SDLK_2:
		this->pressedkeys[23] = true;
		break;
	case SDLK_3:
		this->pressedkeys[24] = true;
		break;
	case SDLK_4:
		this->pressedkeys[25] = true;
		break;
	case SDLK_5:
		this->pressedkeys[26] = true;
		break;
	case SDLK_6:
		this->pressedkeys[27] = true;
		break;
	case SDLK_7:
		this->pressedkeys[28] = true;
		break;
	case SDLK_8:
		this->pressedkeys[29] = true;
		break;
	case SDLK_9:
		this->pressedkeys[30] = true;
		break;
	case SDLK_COLON:
		this->pressedkeys[31] = true;
		break;
	case SDLK_SEMICOLON:
		this->pressedkeys[32] = true;
		break;
	case SDLK_LESS:
		this->pressedkeys[33] = true;
		break;
	case SDLK_EQUALS:
		this->pressedkeys[34] = true;
		break;
	case SDLK_GREATER:
		this->pressedkeys[35] = true;
		break;
	case SDLK_QUESTION:
		this->pressedkeys[36] = true;
		break;
	case SDLK_AT:
		this->pressedkeys[37] = true;
		break;
	case SDLK_LEFTBRACKET:
		this->pressedkeys[38] = true;
		break;
	case SDLK_BACKSLASH:
		this->pressedkeys[39] = true;
		break;
	case SDLK_RIGHTBRACKET:
		this->pressedkeys[40] = true;
		break;
	case SDLK_CARET:
		this->pressedkeys[41] = true;
		break;
	case SDLK_UNDERSCORE:
		this->pressedkeys[42] = true;
		break;
	case SDLK_BACKQUOTE:
		this->pressedkeys[43] = true;
		break;
	case SDLK_a:
		this->pressedkeys[44] = true;
		break;
	case SDLK_b:
		this->pressedkeys[45] = true;
		break;
	case SDLK_c:
		this->pressedkeys[46] = true;
		break;
	case SDLK_d:
		this->pressedkeys[47] = true;
		break;
	case SDLK_e:
		this->pressedkeys[48] = true;
		break;
	case SDLK_f:
		this->pressedkeys[49] = true;
		break;
	case SDLK_g:
		this->pressedkeys[50] = true;
		break;
	case SDLK_h:
		this->pressedkeys[51] = true;
		break;
	case SDLK_i:
		this->pressedkeys[52] = true;
		break;
	case SDLK_j:
		this->pressedkeys[53] = true;
		break;
	case SDLK_k:
		this->pressedkeys[54] = true;
		break;
	case SDLK_l:
		this->pressedkeys[55] = true;
		break;
	case SDLK_m:
		this->pressedkeys[56] = true;
		break;
	case SDLK_n:
		this->pressedkeys[57] = true;
		break;
	case SDLK_o:
		this->pressedkeys[58] = true;
		break;
	case SDLK_p:
		this->pressedkeys[59] = true;
		break;
	case SDLK_q:
		this->pressedkeys[60] = true;
		break;
	case SDLK_r:
		this->pressedkeys[61] = true;
		break;
	case SDLK_s:
		this->pressedkeys[62] = true;
		break;
	case SDLK_t:
		this->pressedkeys[63] = true;
		break;
	case SDLK_u:
		this->pressedkeys[64] = true;
		break;
	case SDLK_v:
		this->pressedkeys[65] = true;
		break;
	case SDLK_w:
		this->pressedkeys[66] = true;
		break;
	case SDLK_x:
		this->pressedkeys[67] = true;
		break;
	case SDLK_y:
		this->pressedkeys[68] = true;
		break;
	case SDLK_z:
		this->pressedkeys[69] = true;
		break;
	case SDLK_DELETE:
		this->pressedkeys[70] = true;
		break;
	case SDLK_KP_PERIOD:
		this->pressedkeys[71] = true;
		break;
	case SDLK_KP_DIVIDE:
		this->pressedkeys[72] = true;
		break;
	case SDLK_KP_MULTIPLY:
		this->pressedkeys[73] = true;
		break;
	case SDLK_KP_MINUS:
		this->pressedkeys[74] = true;
		break;
	case SDLK_KP_PLUS:
		this->pressedkeys[75] = true;
		break;
	case SDLK_KP_ENTER:
		this->pressedkeys[76] = true;
		break;
	case SDLK_KP_EQUALS:
		this->pressedkeys[77] = true;
		break;
	case SDLK_UP:
		this->pressedkeys[78] = true;
		break;
	case SDLK_DOWN:
		this->pressedkeys[79] = true;
		break;
	case SDLK_RIGHT:
		this->pressedkeys[80] = true;
		break;
	case SDLK_LEFT:
		this->pressedkeys[81] = true;
		break;
	case SDLK_INSERT:
		this->pressedkeys[82] = true;
		break;
	case SDLK_HOME:
		this->pressedkeys[83] = true;
		break;
	case SDLK_END:
		this->pressedkeys[84] = true;
		break;
	case SDLK_PAGEUP:
		this->pressedkeys[85] = true;
		break;
	case SDLK_PAGEDOWN:
		this->pressedkeys[86] = true;
		break;
	case SDLK_F1:
		this->pressedkeys[87] = true;
		break;
	case SDLK_F2:
		this->pressedkeys[88] = true;
		break;
	case SDLK_F3:
		this->pressedkeys[89] = true;
		break;
	case SDLK_F4:
		this->pressedkeys[90] = true;
		break;
	case SDLK_F5:
		this->pressedkeys[91] = true;
		break;
	case SDLK_F6:
		this->pressedkeys[92] = true;
		break;
	case SDLK_F7:
		this->pressedkeys[93] = true;
		break;
	case SDLK_F8:
		this->pressedkeys[94] = true;
		break;
	case SDLK_F9:
		this->pressedkeys[95] = true;
		break;
	case SDLK_F10:
		this->pressedkeys[96] = true;
		break;
	case SDLK_F11:
		this->pressedkeys[97] = true;
		break;
	case SDLK_F12:
		this->pressedkeys[98] = true;
		break;
	case SDLK_F13:
		this->pressedkeys[99] = true;
		break;
	case SDLK_F14:
		this->pressedkeys[100] = true;
		break;
	case SDLK_F15:
		this->pressedkeys[101] = true;
		break;
	case SDLK_CAPSLOCK:
		this->pressedkeys[102] = true;
		break;
	case SDLK_RSHIFT:
		this->pressedkeys[103] = true;
		break;
	case SDLK_LSHIFT:
		this->pressedkeys[104] = true;
		break;
	case SDLK_RCTRL:
		this->pressedkeys[105] = true;
		break;
	case SDLK_LCTRL:
		this->pressedkeys[106] = true;
		break;
	case SDLK_RALT:
		this->pressedkeys[107] = true;
		break;
	case SDLK_LALT:
		this->pressedkeys[108] = true;
		break;
	case SDLK_MODE:
		this->pressedkeys[109] = true;
		break;
	case SDLK_HELP:
		this->pressedkeys[110] = true;
		break;
	case SDLK_SYSREQ:
		this->pressedkeys[111] = true;
		break;
	case SDLK_MENU:
		this->pressedkeys[112] = true;
		break;
	case SDLK_POWER:
		this->pressedkeys[113] = true;
		break;
	case KMOD_NUM:
		this->pressedkeys[114] = true;
		break;
	}
}
int Keyboard::getKeyPos(string keyName) {
	int pos = -1;
	for (int i = 0; i < this->keysCount; i++)
		if (this->keyNames[i] == keyName)
			pos = i;
	return pos;
}


Mouse::Mouse() {
	this->x = 0;
	this->y = 0;
	this->leftButtonPressed = false;
	this->rightButtonPressed = false;
	this->middleButtonPressed = false;
}
void Mouse::onMouseUp(SDL_Event e) {
	if (e.button.button == SDL_BUTTON_LEFT)
		this->leftButtonPressed = false;
	if (e.button.button == SDL_BUTTON_RIGHT)
		this->rightButtonPressed = false;
	if (e.button.button == SDL_BUTTON_MIDDLE)
		this->middleButtonPressed = false;
}
void Mouse::onMouseDown(SDL_Event e) {
	if (e.button.button == SDL_BUTTON_LEFT)
		this->leftButtonPressed = true;
	if (e.button.button == SDL_BUTTON_RIGHT)
		this->rightButtonPressed = true;
	if (e.button.button == SDL_BUTTON_MIDDLE)
		this->middleButtonPressed = true;
}
void Mouse::onMouseMotion(SDL_Event e) {
	this->x = e.motion.x;
	this->y = e.motion.y;
}


Timer::Timer(int delay) {
	this->timeLeft = delay;
	this->delayTime = delay;
	this->startTime = (int)clock();
}
bool Timer::isReady() {
	this->timeLeft = delayTime;
	this->timeLeft += this->startTime;
	this->timeLeft -= clock();

	bool result = this->timeLeft <= 0;
	this->timeLeft /= 1000;
	return result;
}
void Timer::reset() {
	this->startTime = (int)clock();
}

Color::Color() {
	this->r = 0;
	this->g = 0;
	this->b = 0;
	this->a = 0;
}
Color::Color(int r, int g, int b, int a) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

Rect::Rect() {
	this->x = 0;
	this->y = 0;
	this->w = 0;
	this->h = 0;
}
Rect::Rect(int x, int y, int w, int h) {
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
}

boost::shared_ptr<Rect> Rect::copy() {
	return boost::shared_ptr<Rect>(new Rect(this->x, this->y, this->w, this->h));
}

