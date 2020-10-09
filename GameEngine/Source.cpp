#define BOOST_PYTHON_STATIC_LIB
#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#pragma comment(lib, "libs/SDL2.lib")
#pragma comment(lib, "libs/SDL2_ttf.lib")
#pragma comment(lib, "libs/SDL2_image.lib")

#include <iostream>
#include <boost/python.hpp>
#include <boost/bind/bind.hpp>

#include "Game.h"
#include "UserIntarface.h"
#include "Wrappers.h"

using namespace Wrappers;
using namespace boost::python;
using namespace boost::placeholders;

void translate(GameExceptions::BaseGameException e) {
	PyErr_SetString(PyExc_RuntimeError, e.what().c_str());
}
void translate2(GameExceptions::GameOverException e) {
	PyErr_SetString(PyExc_RuntimeError, e.what().c_str());
}
void translate3(GameExceptions::CloseGameException e) {
	PyErr_SetString(PyExc_RuntimeError, e.what().c_str());
}
void translate4(GameExceptions::GameInitializationError e) {
	PyErr_SetString(PyExc_RuntimeError, e.what().c_str());
}
void translate5(GameExceptions::ErrorInNotify e) {
	PyErr_SetString(PyExc_RuntimeError, e.what().c_str());
}

void translate6(UserIntarface::Exceptions::FontNotFoundException e) {
	PyErr_SetString(PyExc_RuntimeError, e.what().c_str());
}
void translate7(UserIntarface::Exceptions::ImageNotFoundException e) {
	PyErr_SetString(PyExc_RuntimeError, e.what().c_str());
}

void export_Root() {
	class_<Game_PyWrapper, boost::noncopyable>("Game", init<>())
		.def("__del__", &Game::close)
		.def("init", &Game::init, args("name"))
		.def("init", &Game::init2, args("name", "width", "height"))
		.def("setup", &Game::setup)
		.def("close", &Game::close)
		.def("run", &Game::run)
		.def("draw", &Game::draw)
		.def("update", &Game::update)
		.def("throwException", &Game::throwException)
		.def("setStatus", &Game::setStatus, args("status"))
		.def("notify", &Game::notify, args("sender", "to", "msg"))
		.def("addComponent", &Game::addComponent, args("component"))
		.def_readonly("window", &Game::window)
		.def_readonly("mouse", &Game::mouse)
		.def_readonly("closed", &Game::closed)
		.def_readonly("keyboard", &Game::keyboard)
		.def_readwrite("status", &Game::status)
		;
	class_<Window, boost::noncopyable>("Window", init<string, Rect, Uint32>())
		.def("clear", &Window::clear)
		.def("update", &Window::update)
		.def_readonly("rect", &Window::rect);
	enum_<Window::Flags>("Flags")
		.value("FULLSCREEN", Window::FULLSCREEN)
		.value("OPENGL", Window::OPENGL)
		.value("SHOWN", Window::SHOWN)
		.value("HIDDEN", Window::HIDDEN)
		.value("BORDERLESS", Window::BORDERLESS)
		.value("RESIZABLE", Window::RESIZABLE)
		.value("MINIMIZED", Window::MINIMIZED)
		.value("MAXIMIZED", Window::MAXIMIZED)
		.value("FULLSCREEN_DESKTOP", Window::FULLSCREEN_DESKTOP)
		.value("FOREIGN", Window::FOREIGN)
		.value("ALLOW_HIGHDPI", Window::ALLOW_HIGHDPI)
		.value("MOUSE_CAPTURE", Window::MOUSE_CAPTURE)
		.value("ALWAYS_ON_TOP", Window::ALWAYS_ON_TOP)
		.value("SKIP_TASKBAR", Window::SKIP_TASKBAR)
		.value("UTILITY", Window::UTILITY)
		.value("TOOLTIP", Window::TOOLTIP)
		.value("POPUP_MENU", Window::POPUP_MENU)
		.value("VULKAN", Window::VULKAN);
	class_<Keyboard, boost::noncopyable>("Keyboard", init<>())
		.def("isPressed", &Keyboard::isKeyPressed);
	class_<Mouse, boost::noncopyable>("Mouse", init<>())
		.def_readonly("x", &Mouse::x)
		.def_readonly("y", &Mouse::y)
		.def_readonly("leftButtonPressed", &Mouse::leftButtonPressed)
		.def_readonly("rightButtonPressed", &Mouse::rightButtonPressed)
		.def_readonly("middleButtonPressed", &Mouse::middleButtonPressed)
		;


	class_<GameComponent_PyWrapper, boost::noncopyable>("GameComponent", init<boost::shared_ptr<Game>>())
		.def("__del__", &GameComponent::close)
		.def("loadObjects", &GameComponent::loadObjects)
		.def("canRender", pure_virtual(&GameComponent::canRender))
		.def("onNotify", &GameComponent::onNotify, args("sender", "msg"))
		.def("draw", pure_virtual(&GameComponent::draw))
		.def("update", pure_virtual(&GameComponent::update))
		.def("onKeyUp", &GameComponent::onKeyUp)
		.def("onKeyDown", &GameComponent::onKeyDown)
		.def("onMouseUp", &GameComponent::onMouseUp)
		.def("onMouseDown", &GameComponent::onMouseDown)
		.def("onMouseMotion", &GameComponent::onMouseMotion)
		.def("getId", &GameComponent::getId)
		.def_readwrite("game", &GameComponent::game)
		.def_readwrite("componentName", &GameComponent::componentName)
		;

	class_<Rect>("Rect", init<>())
		.def(init<int, int>())
		.def(init<float, float>())
		.def(init<int, int, int, int>())
		.def(init<int, int, float, float>())
		.def(init<float, float, int, int>())
		.def("copy", &Rect::copy)
		.def_readwrite("x", &Rect::x)
		.def_readwrite("y", &Rect::y)
		.def_readwrite("w", &Rect::w)
		.def_readwrite("h", &Rect::h)
		;
	class_<Color>("Color", init<>())
		.def(init<int, int, int>())
		.def(init<int, int, int, int>())
		.def_readwrite("r", &Color::r)
		.def_readwrite("g", &Color::g)
		.def_readwrite("b", &Color::b)
		.def_readwrite("a", &Color::a)
		;
}
void export_Exceptions() {
	object module(handle<>(borrowed(PyImport_AddModule("GameEngine.exceptions"))));
	scope().attr("exceptions") = module;
	scope scope = module;
	using namespace GameExceptions;
	using namespace UserIntarface::Exceptions;

	boost::python::register_exception_translator<BaseGameException>(translate);
	boost::python::register_exception_translator<GameOverException>(translate2);
	boost::python::register_exception_translator<CloseGameException>(translate3);
	boost::python::register_exception_translator<GameInitializationError>(translate4);
	boost::python::register_exception_translator<ErrorInNotify>(translate5);

	boost::python::register_exception_translator<FontNotFoundException>(translate6);
	boost::python::register_exception_translator<ImageNotFoundException>(translate7);

	class_<BaseGameException, boost::noncopyable>("BaseGameException", init<string>())
		.def("what", &GameOverException::what);
	class_<GameOverException, bases<BaseGameException>, boost::noncopyable>("GameOverException", init<string>())
		.def("what", &GameOverException::what);
	class_<CloseGameException, bases<BaseGameException>, boost::noncopyable>("CloseGameException", init<string>())
		.def("what", &CloseGameException::what);
	class_<GameInitializationError, bases<BaseGameException>, boost::noncopyable>("GameInitializationError", init<string>())
		.def("what", &GameInitializationError::what);
	class_<ErrorInNotify, bases<BaseGameException>, boost::noncopyable>("ErrorInNotify", init<string>())
		.def("what", &ErrorInNotify::what);

	class_<FontNotFoundException, bases<BaseGameException>, boost::noncopyable>("FontNotFoundException", init<string>())
		.def("what", &FontNotFoundException::what);
	class_<ImageNotFoundException, bases<BaseGameException>, boost::noncopyable>("ImageNotFoundException", init<string>())
		.def("what", &ImageNotFoundException::what);
}
void export_UserInterface() {
	object module(handle<>(borrowed(PyImport_AddModule("GameEngine.UserIntarface"))));
	scope().attr("UserIntarface") = module;
	scope scope = module;
	using namespace UserIntarface;
	class_<UserIntarface::Object>("Object", init<boost::shared_ptr<Game>>())
		.def("draw", pure_virtual(&Object::draw))
		.def("update", pure_virtual(&Object::update))
		.def("checkColision", &Object::checkColision)
		.def("checkColision", &Object::checkColision2)
		.def("move", &Object::move)
		.def("resize", &Object::resize)
		.def_readonly("id", &Object::id)
		.def_readwrite("rect", &Object::rect)
		.def_readwrite("game", &Object::game)
		.def_readwrite("change_x", &Object::change_x)
		.def_readwrite("change_y", &Object::change_y)
		;
	class_<UserIntarface::Sprite, bases<Object>>("Sprite", init<
		boost::shared_ptr<Game>,
		boost::shared_ptr<Rect>,
		string>())
		.def(init<boost::shared_ptr<Game>, string>())
		.def("draw", &Sprite::draw)
		.def("update", &Sprite::update)
		.def("checkColision", &Sprite::checkColision)
		.def("checkColision", &Object::checkColision2)
		.def("move", &Sprite::move)
		.def("resize", &Sprite::resize)
		.def("loadTexture", &Sprite::loadTexture)
		.def_readonly("id", &Sprite::id)
		.def_readwrite("rect", &Sprite::rect)
		.def_readwrite("game", &Sprite::game)
		.def_readwrite("change_x", &Sprite::change_x)
		.def_readwrite("change_y", &Sprite::change_y)
		;
	class_<UserIntarface::Text, bases<Object>>("Text", init<
		boost::shared_ptr<Game>,
		string,
		boost::shared_ptr<Color>,
		boost::shared_ptr<Rect>,
		string, int,
		bool>())
		.def(init<
			boost::shared_ptr<Game>,
			string,
			boost::shared_ptr<Color>,
			boost::shared_ptr<Rect>,
			string, int>())
		.def("center", &Text::center)
		.def("setFont", &Text::setFont)
		.def("setColor", &Text::setColor)
		.def("setSource", &Text::setSource)
		.def("draw", &Text::draw)
		.def("update", &Text::update)
		.def("checkColision", &Text::checkColision)
		.def("checkColision", &Object::checkColision2)
		.def("move", &Text::move)
		.def("resize", &Text::resize)
		.def_readonly("id", &Text::id)
		.def_readwrite("rect", &Text::rect)
		.def_readwrite("game", &Text::game)
		.def_readwrite("change_x", &Text::change_x)
		.def_readwrite("change_y", &Text::change_y)
		;

	class_<UserIntarface::Button, bases<Object>>("Button", init<
		boost::shared_ptr<Game>,
		func,
		boost::shared_ptr<Rect>,
		string>())
		.def(init<boost::shared_ptr<Game>, func, string>())
		.def("draw", &Button::draw)
		.def("update", &Button::update)
		.def("checkColision", &Button::checkColision)
		.def("checkColision", &Object::checkColision2)
		.def("move", &Button::move)
		.def("resize", &Button::resize)
		.def("loadTexture", &Button::loadTexture)
		.def("setText", &Button::setText)
		.def("onMouseUp", &Button::onMouseUp)
		.def("onMouseDown", &Button::onMouseDown)
		.def("onMouseMotion", &Button::onMouseMotion)
		.def_readonly("id", &Button::id)
		.def_readwrite("onpress", &Button::onpress)
		.def_readwrite("rect", &Button::rect)
		.def_readwrite("game", &Button::game)
		.def_readwrite("change_x", &Button::change_x)
		.def_readwrite("change_y", &Button::change_y)
		;
}
void export_Time() {
	object module(handle<>(borrowed(PyImport_AddModule("GameEngine.time"))));
	scope().attr("time") = module;
	scope scope = module;

	class_<Timer, boost::noncopyable>("Timer", init<int>())
		.def(init<float>())
		.def("isReady", &Timer::isReady)
		.def("reset", &Timer::reset)
		.def_readonly("timeLeft", &Timer::timeLeft)
		;
}
void export_Draw() {
	object module(handle<>(borrowed(PyImport_AddModule("GameEngine.draw"))));
	scope().attr("draw") = module;
	scope scope = module;
}
void export_ptrs() {
	register_ptr_to_python< boost::shared_ptr<Window> >();
	register_ptr_to_python< boost::shared_ptr<Rect> >();
	register_ptr_to_python< boost::shared_ptr<Color> >();
	register_ptr_to_python< boost::shared_ptr<Game> >();
	register_ptr_to_python< boost::shared_ptr<Mouse> >();
	register_ptr_to_python< boost::shared_ptr<Keyboard> >();
	register_ptr_to_python< boost::shared_ptr<GameComponent> >();
}

BOOST_PYTHON_MODULE(GameEngine) {
	object package = scope();
	package.attr("__path__") = "GameEngine";

	export_Root();
	export_Exceptions();
	export_UserInterface();
	export_Time();
	export_Draw();
	export_ptrs();
}
