#pragma once

#include "Game.h"
#include <SDL2/SDL.h>
#include <SDL_ttf/SDL_ttf.h>
#include <SDL_image/SDL_image.h>

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
		Object(shared_ptr<Game> game);
		virtual void draw() {};
		virtual void update() {};

		virtual bool checkColision(shared_ptr<Object> object);
		virtual bool checkColision2(shared_ptr<Rect> rect);

		int change_x;
		int change_y;
		void move(int x, int y);
		void resize(int w, int h);

		int id;
		SDL_Texture* texture;
		shared_ptr<Game> game;
		shared_ptr<Rect> rect;
	private:
		static int objectsCount;
	};

	class Sprite : public Object {
	public:
		Sprite(shared_ptr<Game> game, shared_ptr<Rect> rect, string path);
		Sprite(shared_ptr<Game> game, string path);
		~Sprite();

		void loadTexture(string path);

		virtual void draw();
		virtual void update();
	protected:
		string path;
	};

	class Text : public Object {
	public:
		Text(shared_ptr<Game> game,
			string source,
			shared_ptr<Color> color,
			shared_ptr<Rect> rect,
			string fontName, int fontSize,
			bool center = false);
		~Text();

		void center();
		void setFont(string fontName, int fontSize);
		void setColor(shared_ptr<Color> color);
		void setSource(string source);

		void draw();
		void update();

		string source;
		TTF_Font* font;
		shared_ptr<Color> color;
	};

	class Button : public Sprite {
	public:
		Button(shared_ptr<Game> game, func onpress, shared_ptr<Rect> rect, string path);
		Button(shared_ptr<Game> game, func onpress, string path);
		~Button();

		virtual void draw() override;
		virtual void update() override;

		void setText(shared_ptr<Text> text);
		void onMouseUp(int x, int y);
		void onMouseDown(int x, int y);
		void onMouseMotion(int x, int y);

		shared_ptr<Text> text;
		func onpress;
	protected:
		bool is_pressed;
	};
};
