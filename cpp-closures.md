---
layout: page
title: "(Mis)treating C++ like it was Javascript"
description: ""
category: 
tags: []
---
{% include JB/setup %}

C++11 Shared Pointers
---------------------
If you have gotten your hands dirty with the new features in C++11 you know that it allows you to
program differently than you used to. In addition to type inference through `auto` and *for each*
loops, you have things like `std::move()` and `std::shared_ptr`.

These two, especially the reference counting *shared_ptr*, makes it
much less necessary to use `new` or `delete`. You can almost always create objects on the stack and
pass by value without many of the problems and penalties you used to have.

One way to accomplish this is put your class members in an internal class, and only store
a shared_ptr to the internal class in the external class, like this;

{% highlight c++ %}
struct Bitmap {
	struct InternalBitmap {
		InternalBitmap(int w, int h) : w(w), h(h), pixels(w*h) {}
		int w;
		int h;
		std::vector<uint8_t> pixels;
	};

	Bitmap(int w, int h) : internalBitmap(std::make_shared<InternalBitmap>(w, h)) {}
	T& operator[](const int &i) { 
		return (*internalBitmap->pixels)[i];
	}
	int width() const { return internalBitmap->w; }
	int height() const { return internalBitmap->h; }

	std::shared_ptr<InternalBitmap> internalBitmap;
};
{% endhighlight %}
Now you can treat your objects like in java for instance; pass it around by value and all
places that store it will share the same bitmap - and when the last reference goes away
it will be destroyed, just like in a garbage collected language.

If you don't need the enitre class to be shared, you can accomplish this in a simpler way like this;
{% highlight c++ %}
struct Bitmap {
	Bitmap(int w, int h) :  : w(w), h(h), pixels(std::make_shared<std::vector<uint8_t>(w*h)) {}
	T& operator[](const int &i) { 
		return (*pixels)[i];
	}
	int w;
	int h;

	std::shared_ptr<std::vector<uint8_t>> pixels;
};
{% endhighlight %}

This is how I usually do it; keep simple data members as is, since
copying them is not a performance problem, and wrap all your containers in a `shared_ptr`. This is
(I think) also how it works in languages like *python* - top level members gets copied, but anything
referenced gets shared.

Adventures in graphics programming and Emscripten
------------------------------------------------------
So I have been using these techniques in a graphics library I am developing,because it allows
you to do more with less code, and not think about things like memory allocation. Basically, I
want to people to explore programming with the same abandon you used to be able to do it on
old BASIC 8-bit computers :)

This is how a simple program may look like:
{% highlight c++ %}
int main() {

	screen.open();

	int x = 0;
	Font font("data/ObelixPro.ttf", 18);
	Bitmap logo = load_png("data/logo.png");
	Texture tex(logo);

	while(screen.is_open()) {
		screen.clear();
		screen.circle(x += 4, 200, 180, 0xff0000ff);
		screen.draw(tex);
		screen.text(font, "Circle", screen.width()-120, screen.height()-50);
		screen.flip();
	};
	return 0;
}
{% endhighlight %}

Hopefully this is pretty straight forward. *Bitmaps* are basically big arrays of pixels, while *Textures* are OpenGL textures,
represented by a texture ID. Fonts also become textures on creation (allthough they use *distance fields* by default).

Then came the day when I discovered **emscripten**. If you haven't heard of it; it's a LLVM based compiler
that compiles C/C++ code into javascript for the web. And it *works*!

So naturally I ported my library to emscripten. But there is a problem getting the code above to work in javascript.
Can you spot it?

If you answered *you can't have an ongoing render loop like that, since javascript is single threaded and event driven*, 
you are right. Emscripten requires you to pass a function pointer to be excuted to render each frame of graphics, and will
not do anything until `main()` has ended.

So my first solution was to break out everything into it's own class, with an `update()` function, and then pass a function
pointer to be called repeatedly. This works, but makes small examples like this a lot less readable. 
{% highlight c++ %}
struct App {

	App() : x(0), font("data/ObelixPro.ttf", 18), logo(load_png("data/logo.png")), tex(logo) {
	}

	void update() {
		screen.clear();
		screen.circle(x += 4, 200, 180, 0xff0000ff);
		screen.draw(tex);
		screen.text(font, "Circle", screen.width()-120, screen.height()-50);
		screen.flip();
	}

	int x;
	Font font;
	Bitmap logo;
	Texture tex;
};

void runMainLoop() {
	static App app;
	app.update();
}

int main() {
	screen.open();
	screen.run_main_loop(runMainLoop);
	return 0;
}
{% endhighlight %}
But then it hit me; since the classes are designed for pass-by-value, I can
actually make the main loop into a _closure_ and have it executed by my library as a `std::function`. So I added this to my *screen*
object;
{% highlight c++ %}
static function<void()> renderLoopFunction;

#ifdef EMSCRIPTEN
static void runMainLoop() {
		renderLoopFunction();
}
#endif

void Window::render_loop(function<void()> f, int fps) {
	renderLoopFunction = f;
#ifdef EMSCRIPTEN
	emscripten_set_main_loop(runMainLoop, fps, false);
#else
	atexit([](){
		while(screen.is_open()) {
			renderLoopFunction();
		}
	});
#endif
}
{% endhighlight %}
And changed the earlier example into this;
{% highlight c++ %}
int main() {

	screen.open();
	int x = 0;
	Font font("data/ObelixPro.ttf", 18);
	Bitmap logo = load_png("data/logo.png");
	Texture tex(logo);

	screen.set_render_loop([=]() mutable {
		screen.clear();
		screen.circle(x += 4, 200, 180, 0xff0000ff);
		screen.draw(tex);
		screen.text(font, "Circle", screen.width()-120, screen.height()-50);
		screen.flip();
	});
	return 0;
}
{% endhighlight %}
...and it worked perfectly. Note my use of `atexit()` -- the render loop is actually executed after `main()` has exited and
all the variables have gone out of scope. But since the lambda has captured them all they are not destroyed until we actually exit.

More on Lambda expressions
--------------------------

If you haven't used lambda expressions in C++ before, the earlier example may have confused you.
