#pragma once
#include "Game.h"
#include <boost/python.hpp>
#include "UserIntarface.h"
using namespace UserIntarface;
using namespace boost::python;

namespace Wrappers {
	class GameComponent_PyWrapper : public GameComponent, public wrapper<GameComponent> {
	public:
		GameComponent_PyWrapper(boost::shared_ptr<Game> game) : GameComponent(game) {}
		void close() {
			if (boost::python::override f = this->get_override("__del__")) {
				f();
			}
		}

		void loadObjects() {
			if (boost::python::override f = this->get_override("loadObjects")) {
				f();
			}
		}
		bool canRender() {
			if (boost::python::override f = this->get_override("canRender")) {
				return f();
			}
			return false;
		}
		void onNotify(boost::shared_ptr<GameComponent> sender, string msg) {
			if (boost::python::override f = this->get_override("onNotify")) {
				f(sender, msg);
			}
		}

		void draw() {
			if (boost::python::override f = this->get_override("draw")) {
				f();
			}
		}
		void update() {
			if (boost::python::override f = this->get_override("update")) {
				f();
			}
		}

		void onKeyUp(SDL_Keycode key) {
			if (boost::python::override f = this->get_override("onKeyUp")) {
				f(key);
			}
		}
		void onKeyDown(SDL_Keycode key) {
			if (boost::python::override f = this->get_override("onKeyDown")) {
				f(key);
			}
		}
		void onMouseUp(int x, int y) {
			if (boost::python::override f = this->get_override("onMouseUp")) {
				f(x, y);
			}
		}
		void onMouseDown(int x, int y) {
			if (boost::python::override f = this->get_override("onMouseDown")) {
				f(x, y);
			}
		}
		void onMouseMotion(int x, int y) {
			if (boost::python::override f = this->get_override("onMouseMotion")) {
				f(x, y);
			}
		}
	};

	class Sprite_PyWrapper : public Sprite, wrapper<Sprite> {
	public:
		Sprite_PyWrapper(boost::shared_ptr<Game> game, boost::shared_ptr<Rect> rect, string path) : Sprite(game, rect, path) {}
		Sprite_PyWrapper(boost::shared_ptr<Game> game, string path) : Sprite(game, path) {}

		void draw() {
			if (boost::python::override f = this->get_override("draw")) {
				f();
			}
			else {
				Sprite::draw();
			}
		}
		void update() {
			if (boost::python::override f = this->get_override("update")) {
				f();
			}
			else {
				Sprite::update();
			}
		}

		void move(int x, int y) {
			if (boost::python::override f = this->get_override("move")) {
				f(x, y);
			}
			else {
				Sprite::move(x, y);
			}
		}
		void resize(int w, int h) {
			if (boost::python::override f = this->get_override("resize")) {
				f(w, h);
			}
			else {
				Sprite::resize(w, h);
			}
		}
	};
	/*class Text_PyWrapper : public Text, wrapper<Text> {
	public:
		Text_PyWrapper(boost::shared_ptr<Game> game, boost::shared_ptr<Rect> rect, string path) : Text(game, rect, path) {}
		Text_PyWrapper(boost::shared_ptr<Game> game, string path) : Text(game, path) {}

		void draw() {
			if (boost::python::override f = this->get_override("draw")) {
				f();
			}
			else {
				Text::draw();
			}
		}
		void update() {
			if (boost::python::override f = this->get_override("update")) {
				f();
			}
			else {
				Text::update();
			}
		}

		void move(int x, int y) {
			if (boost::python::override f = this->get_override("move")) {
				f(x, y);
			}
			else {
				Text::move(x, y);
			}
		}
		void resize(int w, int h) {
			if (boost::python::override f = this->get_override("resize")) {
				f(w, h);
			}
			else {
				Text::resize(w, h);
			}
		}
	};*/

	class Game_PyWrapper : public Game, public wrapper<Game> {
	public:
		Game_PyWrapper() : Game() {}

		void run() {
			this->window->clear();
			this->notify(nullptr, "all", "start rendering");
			while (this->runing) {

				try {
					this->update();
					this->draw();

					// throw exception
					if (this->exception != nullptr) {
						using namespace GameExceptions;
						if (this->exception->type() == "CloseGameException") {
							throw CloseGameException(this->exception->what());
						}
						else if (this->exception->type() == "GameOverException") {
							throw GameOverException(this->exception->what());
						}
						else {
							throw (*this->exception);
						}
					}
				}
				catch (GameExceptions::GameOverException err) {
					this->notify(nullptr, "all", "gameOver");
					this->status = "gameOver";
				}
				catch (GameExceptions::CloseGameException err) {
					cout << err.what() << endl;
					this->runing = false;
				}

				if (this->exception) {
					delete this->exception;
					this->exception = nullptr;
				}
				this->window->update();
			}
		}

		void close() {
			if (boost::python::override f = this->get_override("close")) {
				f();
			}
			else {
				Game::close();
			}
		}
		void draw() {
			if (boost::python::override f = this->get_override("draw")) {
				f();
			}
			else {
				Game::draw();
			}
		}
		void update() {
			if (boost::python::override f = this->get_override("update")) {
				f();
			}
			else {
				Game::update();
			}
		}
	};
};
