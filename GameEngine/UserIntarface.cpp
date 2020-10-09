#include "UserIntarface.h"

using namespace UserIntarface;

Object::Object(boost::shared_ptr<Game> game) {
	this->game = game;
	this->id = this->objectsCount;
	this->objectsCount++;
	this->change_x = 0;
	this->change_y = 0;
	this->rect = nullptr;
	this->texture = nullptr;
}
void Object::move(int x, int y) {
	this->rect->x = x;
	this->rect->y = y;
}
void Object::resize(int w, int h) {
	this->rect->w = w;
	this->rect->h = h;
}
bool Object::checkColision(boost::shared_ptr<Object> _object) {
	for (int x = this->rect->x; x <= this->rect->x + this->rect->w; x++) {
		for (int y = this->rect->y; y <= this->rect->y + this->rect->h; y++) {
			if ((_object->rect->x <= x && (x <= _object->rect->x + _object->rect->w)) &&
				_object->rect->y <= y && (y <= _object->rect->y + _object->rect->h))
				return true;
		}
	}
	return false;
}
bool Object::checkColision2(boost::shared_ptr<Rect> _rect) {
	for (int x = this->rect->x; x <= this->rect->x + this->rect->w; x++) {
		for (int y = this->rect->y; y <= this->rect->y + this->rect->h; y++) {
			if ((_rect->x <= x && (x <= _rect->x + _rect->w)) &&
				_rect->y <= y && (y <= _rect->y + _rect->h))
				return true;
		}
	}
	return false;
}
int Object::objectsCount = 0;


Sprite::Sprite(boost::shared_ptr<Game> _game, boost::shared_ptr<Rect> _rect, string _path) : Object(_game) {
	this->rect = _rect;
	this->loadTexture(_path);
}
Sprite::Sprite(boost::shared_ptr<Game> _game, string _path) : Object(_game) {
	this->rect = boost::shared_ptr<Rect>(new Rect);
	this->rect->x = 0;
	this->rect->y = 0;
	this->rect->w = 100;
	this->rect->h = 100;
	this->loadTexture(_path);
}
Sprite::~Sprite() {
	SDL_DestroyTexture(this->texture);

	this->rect = nullptr;
	this->texture = nullptr;
}
void Sprite::loadTexture(string _path) {
	this->path = "textures/";
	this->path += _path;
	SDL_Surface* img = IMG_Load(this->path.c_str());
	this->texture = SDL_CreateTextureFromSurface(this->game->window->getRenderer(), img);
	if (this->texture == nullptr) {
		string msg;
		msg += "\"";
		msg += this->path;
		msg += "\" doesn't exist!";
		cout << msg << endl;
		throw Exceptions::ImageNotFoundException(msg);
	}
}
void Sprite::draw() {
	SDL_Rect _rect = { (int)this->rect->x, (int)this->rect->y, (int)this->rect->w, (int)this->rect->h };
	SDL_RenderCopy(this->game->window->getRenderer(), this->texture, NULL, &_rect);
}
void Sprite::update() {
	this->rect->x += this->change_x;
	this->rect->y += this->change_y;
	this->change_x = 0;
	this->change_y = 0;
}


Text::Text(boost::shared_ptr<Game> _game,
		string _source,
		boost::shared_ptr<Color> _color,
		boost::shared_ptr<Rect> _rect,
		string fontName, int fontSize,
		bool _center) : Object(_game) {
	this->rect = _rect;
	this->color = _color;
	this->source = _source;
	this->font = nullptr;
	this->setFont(fontName, fontSize);
	if (_center)
		this->center();
}
Text::~Text() {
	TTF_CloseFont(this->font);
	SDL_DestroyTexture(this->texture);

	this->rect = nullptr;
	this->font = nullptr;
	this->texture = nullptr;
}
void Text::center() {
	this->rect->x -= this->rect->w / 2;
	this->rect->y -= this->rect->h / 2;
}
void Text::setFont(string fontName, int fontSize) {
	if (this->font)
		TTF_CloseFont(this->font);
	this->font = TTF_OpenFont(fontName.c_str(), fontSize);
	if (this->font == nullptr) {
		string msg;
		msg += "\"";
		msg += fontName;
		msg += "\" doesn't exist!";
		throw Exceptions::FontNotFoundException(msg);
	}
	this->setSource(this->source);
}
void Text::setColor(boost::shared_ptr<Color> _color) {
	this->color = _color;
	this->setSource(this->source);
}
void Text::setSource(string _source) {
	this->source = _source;
	SDL_Color _color = { this->color->r, this->color->g, this->color->b, this->color->a };
	SDL_Surface* img = TTF_RenderText_Blended(this->font, this->source.c_str(), _color);
	this->texture = SDL_CreateTextureFromSurface(this->game->window->getRenderer(), img);
	SDL_FreeSurface(img);

	SDL_QueryTexture(this->texture, NULL, NULL, &this->rect->w, &this->rect->h);
}
void Text::draw() {
	SDL_Rect _rect = { (int)this->rect->x, (int)this->rect->y, (int)this->rect->w, (int)this->rect->h };
	SDL_RenderCopy(this->game->window->getRenderer(), this->texture, NULL, &_rect);
}
void Text::update() {
	this->rect->x += this->change_x;
	this->rect->y += this->change_y;
	this->change_x = 0;
	this->change_y = 0;
}


Button::Button(boost::shared_ptr<Game> _game, func _onpress, boost::shared_ptr<Rect> _rect, string _path) : Sprite(_game, _rect, _path) {
	this->onpress = _onpress;
	this->text = nullptr;
	this->is_pressed = false;
}
Button::Button(boost::shared_ptr<Game> _game, func _onpress, string _path) : Sprite(_game, _path) {
	this->onpress = _onpress;
	this->text = nullptr;
	this->is_pressed = false;
}
Button::~Button() {
	SDL_DestroyTexture(this->texture);
	this->rect = nullptr;
	this->texture = nullptr;
}
void Button::onMouseUp(int x, int y) {
	if (this->is_pressed &&
		(this->rect->x <= x && (x <= this->rect->x + this->rect->w)) &&
		(this->rect->y <= y && (y <= this->rect->y + this->rect->h))) {
		this->onpress(this->game);
	}
	this->is_pressed = false;
}
void Button::onMouseDown(int x, int y) {
	if ((this->rect->x <= x && (x <= this->rect->x + this->rect->w)) &&
		(this->rect->y <= y && (y <= this->rect->y + this->rect->h))) {
		this->is_pressed = true;
	}
}
void Button::onMouseMotion(int x, int y) {}
void Button::draw() {
	Sprite::draw();
	if (this->text)
		this->text->draw();
}
void Button::update() {
	Sprite::update();
	if (this->text)
		this->text->update();
}
void Button::setText(boost::shared_ptr<Text> _text) {
	this->text = _text;
}
