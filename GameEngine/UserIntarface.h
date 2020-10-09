#pragma once

#include "Game.h"
#include <SDL2/SDL.h>
#include <SDL_ttf/SDL_ttf.h>
#include <SDL_image/SDL_image.h>
#include <boost/python.hpp>
#include <boost/function.hpp>

using func = boost::python::object;

namespace UserIntarface {
	namespace Exceptions {
		class FontNotFoundException : public GameExceptions::BaseGameException {
		public:
			FontNotFoundException(string str) : BaseGameException(str) {}
			virtual const string type() const {
				return "FontNotFoundException";
			}
		};
		class ImageNotFoundException : public GameExceptions::BaseGameException {
		public:
			ImageNotFoundException(string str) : BaseGameException(str) {}
			virtual const string type() const {
				return "ImageNotFoundException";
			}
		};
	};

	class Object {
	public:
		Object(boost::shared_ptr<Game> game);
		virtual void draw() {};
		virtual void update() {};

		virtual bool checkColision(boost::shared_ptr<Object> object);
		virtual bool checkColision2(boost::shared_ptr<Rect> rect);

		int change_x;
		int change_y;
		void move(int x, int y);
		void resize(int w, int h);

		int id;
		SDL_Texture* texture;
		boost::shared_ptr<Game> game;
		boost::shared_ptr<Rect> rect;
	private:
		static int objectsCount;
	};

	class Sprite : public Object {
	public:
		Sprite(boost::shared_ptr<Game> game, boost::shared_ptr<Rect> rect, string path);
		Sprite(boost::shared_ptr<Game> game, string path);
		~Sprite();

		void loadTexture(string path);

		virtual void draw();
		virtual void update();
	protected:
		string path;
	};

	class Text : public Object {
	public:
		Text(boost::shared_ptr<Game> game,
			string source,
			boost::shared_ptr<Color> color,
			boost::shared_ptr<Rect> rect,
			string fontName, int fontSize,
			bool center = false);
		~Text();

		void center();
		void setFont(string fontName, int fontSize);
		void setColor(boost::shared_ptr<Color> color);
		void setSource(string source);

		void draw();
		void update();

		string source;
		TTF_Font* font;
		boost::shared_ptr<Color> color;
	};

	class Button : public Sprite {
	public:
		Button(boost::shared_ptr<Game> game, func onpress, boost::shared_ptr<Rect> rect, string path);
		Button(boost::shared_ptr<Game> game, func onpress, string path);
		~Button();

		virtual void draw() override;
		virtual void update() override;

		void setText(boost::shared_ptr<Text> text);
		void onMouseUp(int x, int y);
		void onMouseDown(int x, int y);
		void onMouseMotion(int x, int y);

		boost::shared_ptr<Text> text;
		func onpress;
	protected:
		bool is_pressed;
	};
};
