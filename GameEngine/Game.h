#pragma once

#include <iostream>
#include <ctime>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL_ttf/SDL_ttf.h>
#include <SDL_image/SDL_image.h>
#include <sstream>

using namespace std;

class Game;
class Window;
class GameComponent;
class Keyboard;
class Mouse;
class Timer;
class Color;
class Rect;

typedef void (*func)(shared_ptr<Game>);

namespace GameExceptions {
	class BaseGameException {
	public:
		BaseGameException(string str) { this->err = str.c_str(); }
		string what() {
			return this->err;
		}
		virtual const string type() const {
			return "BaseGameException";
		}
	protected:
		string err;
	};

	class GameOverException : public BaseGameException {
	public:
		GameOverException(string str) : BaseGameException(str) {}
		virtual const string type() const {
			return "GameOverException";
		}
	};
	class CloseGameException : public BaseGameException {
	public:
		CloseGameException(string str) : BaseGameException(str) {}
		virtual const string type() const {
			return "CloseGameException";
		}
	};
	class GameInitializationError : public BaseGameException {
	public:
		GameInitializationError(string str) : BaseGameException(str) {}
		virtual const string type() const {
			return "GameInitializationError";
		}
	};
	class ErrorInNotify : public BaseGameException {
	public:
		ErrorInNotify(string str) : BaseGameException(str) {}
		virtual const string type() const {
			return "ErrorInNotify";
		}
	};
};

class Window {
public:
	Window(string name, Rect rect, Uint32 flags);
	Window(Window&) = delete;
	void operator=(const Window&) = delete;

	void clear();
	void update();
	SDL_Renderer* getRenderer() const {
		return this->renderer;
	}

	shared_ptr<Rect> rect;
	typedef enum {
		FULLSCREEN = 0x00000001,
		OPENGL = 0x00000002,
		SHOWN = 0x00000004,
		HIDDEN = 0x00000008,
		BORDERLESS = 0x00000010,
		RESIZABLE = 0x00000020,
		MINIMIZED = 0x00000040,
		MAXIMIZED = 0x00000080,
		FULLSCREEN_DESKTOP = (FULLSCREEN | 0x00001000),
		FOREIGN = 0x00000800,
		ALLOW_HIGHDPI = 0x00002000,
		MOUSE_CAPTURE = 0x00004000,
		ALWAYS_ON_TOP = 0x00008000,
		SKIP_TASKBAR = 0x00010000,
		UTILITY = 0x00020000,
		TOOLTIP = 0x00040000,
		POPUP_MENU = 0x00080000,
		VULKAN = 0x10000000
	} Flags;
protected:
	SDL_Window* sdlwin;
	SDL_Renderer* renderer;
};

class Game {
public:
	Game();
	~Game();

	void init(string name);
	void init2(string name, int width, int height);

	void setup();
	virtual void run() = 0;
	void close();

	void draw();
	void update();

	void setExceptionsHandler(func callback);
	void throwException(GameExceptions::BaseGameException* exc);
	void setStatus(string status);
	void notify(GameComponent* sender, string to, string msg);
	void addComponent(GameComponent* component);

	string status;
	shared_ptr<Mouse> mouse;
	shared_ptr<Keyboard> keyboard;
	shared_ptr<Window> window;

	bool runing, closed;
	friend class Keyboard;
	friend class GameComponent;
protected:
	func exceptionHandler;

	vector<GameComponent*> components;
	GameExceptions::BaseGameException* exception;
};

class GameComponent {
public:
	GameComponent(shared_ptr<Game>& game);
	virtual void close() = 0;

	virtual void loadObjects() = 0;
	virtual bool canRender() = 0;
	virtual void onNotify(shared_ptr<GameComponent> sender, string msg) = 0;

	virtual void draw() = 0;
	virtual void update() = 0;

	virtual void onKeyUp(SDL_Keycode key) = 0;
	virtual void onKeyDown(SDL_Keycode key) = 0;
	virtual void onMouseUp(int x, int y) = 0;
	virtual void onMouseDown(int x, int y) = 0;
	virtual void onMouseMotion(int x, int y) = 0;

	int getId();

	string componentName;
	shared_ptr<Game> game;
private:
	int id;
	static int componentsCount;
};
class Keyboard {
public:
	Keyboard();

	bool isKeyPressed(string keyName);
	friend class Game;
protected:
	void onKeyUp(SDL_Keycode key);
	void onKeyDown(SDL_Keycode key);

	int getKeyPos(string keyName);

	int keysCount;
	string* keyNames;
	bool* pressedkeys;
};
class Mouse {
public:
	Mouse();
	void onMouseUp(SDL_Event e);
	void onMouseDown(SDL_Event e);
	void onMouseMotion(SDL_Event e);

	int x, y;
	bool leftButtonPressed;
	bool rightButtonPressed;
	bool middleButtonPressed;
};
class Timer {
public:
	Timer(int delay);

	bool isReady();
	void reset();

	double timeLeft;
protected:
	int delayTime;
	int startTime;
};
class Color {
public:
	Color();
	Color(int r, int g, int b, int a = 255);

	int r, g, b, a;
};
class Rect {
public:
	Rect();
	Rect(int x, int y, int w = 0, int h = 0);

	shared_ptr<Rect> copy();

	int x, y, w, h;
};
