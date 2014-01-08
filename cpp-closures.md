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
loops, we now have a standard smart pointer called `std::shared_ptr`.

If you don't know how smart pointers work, well, they basically wrap a pointer and a reference
counter, so that the object pointed to can be destroyed when there are no more references to it.
This will emulate how references work in a garbage collected language like javascript.

`std::shared_ptr` makes it much less necessary to use `new` or `delete`. You can almost always create objects on the stack and
pass by value without many of the problems and penalties you used to have.

The normal way to use a shared_ptr is like this;
{% highlight c++ %}
struct Bitmap {
    Bitmap(int w, int h) : w(w), h(h), pixels(w*h) {}
    int w;
    int h;
    vector<uint8_t> pixels;
};

shared_ptr<Bitmap> bitmap(new Bitmap(100,100);
{% endhighlight %}
or preferrably, you can use type inference and the more effecient `make_shared<T>()` instead, 
since it can create the Bitmap and the shared_ptr with only one allocation.
{% highlight c++ %}
auto bitmap = make_shared<Bitmap>(100,100);
{% endhighlight %}

Hiding the shared_ptr
---------------------
Now you can use your `bitmap` variable like a normal pointer, but you dont have to `delete` it.
When it goes out of scope, the shared_ptr destructor will be called and it will delete the object.

But if you are designing an API say, and you want to spare your users from these details, you
can hide the reference counting shared_ptr like this:
{% highlight c++ %}
struct Bitmap {
    struct InternalBitmap {
        InternalBitmap(int w, int h) : w(w), h(h), pixels(w*h) {}
        int w;
        int h;
        vector<uint8_t> pixels;
    };

    Bitmap(int w, int h) : internalBitmap(make_shared<InternalBitmap>(w, h)) {}
    T& operator[](const int &i) { 
        return (*internalBitmap->pixels)[i];
    }
    int width() const { return internalBitmap->w; }
    int height() const { return internalBitmap->h; }

    shared_ptr<InternalBitmap> internalBitmap;
};
{% endhighlight %}
Now your *Bitmap* will create an *InternalBitmap* on construction, and delete it on destruction.
The outer Bitmap contains only the *shared_ptr*, so it's small enough to pass around by value
without any noticable loss in performance.

And so you can now treat your object like *javascript*; pass it around by value and all
places that store it will share the same bitmap -- and when the last reference goes away
it will be destroyed, just like in a garbage collected language.

If you don't need the enitre class to be shared, you can accomplish this in a simpler way;
{% highlight c++ %}
struct Bitmap {
    Bitmap(int w, int h) : w(w), h(h), pixels(make_shared<vector<uint8_t>>(w*h)) {}
    T& operator[](const int &i) { 
        return (*pixels)[i];
    }

    int w;
    int h;
    shared_ptr<vector<uint8_t>> pixels;
};
{% endhighlight %}

This is how I usually do it; keep simple data members as is, since  copying them is
not normally a performance problem, and wrap all your containers in a `shared_ptr`. This is
also how it works in languages like *python* -- top level members gets copied, but anything
referenced gets shared.

Library programming and Emscripten
----------------------------------

I mentioned designing APIs; this is because I have been using these techniques in a graphics
library I am developing. The idea was
that it would make the library easy enough to use for beginners. It is kind of inspired by
my experiences programming 8-bit computers as a kid. 

This is how a (complete) simple program may look like:
{% highlight c++ %}
#include <grappix/grappix.h>

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

Hopefully this is pretty straight forward. *Bitmaps* are just big arrays of pixels, while *Textures* are OpenGL textures,
represented by a texture ID. *Fonts* also become textures on creation (allthough they use *distance fields* by default).

Then came the day when I discovered **emscripten**. If you haven't heard of it; it's a LLVM based compiler
that compiles C/C++ code into javascript for the web. And it *works*!

So naturally I ported my library to emscripten. But there is a problem getting the code above to work in javascript.
Can you spot it?

If you answered *"You can't have an ongoing render loop like that, since javascript is single threaded and event driven"*, 
you are right. Emscripten requires you to pass a function pointer to be excuted to render each frame of graphics, and will
not do anything until `main()` has ended.

So my first solution was to break out everything into it's own class, with an `update()` function, and then pass a function
pointer to be called repeatedly. This works, but makes small examples like this a lot less readable. 
{% highlight c++ %}
#include <grappix/grappix.h>

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

void Window::render_loop(function<void()> f, int fps) {
    renderLoopFunction = f;
#ifdef EMSCRIPTEN
    emscripten_set_main_loop(renderLoopFunction, fps, false);
#else
    atexit([](){
        while(screen.is_open()) {
            renderLoopFunction();
        }
    });
#endif
}
{% endhighlight %}
And changed the earlier mexample into this;
{% highlight c++ %}
#include <grappix/grappix.h>

int main() {

    screen.open();
    int x = 0;
    Font font("data/ObelixPro.ttf", 18);
    Bitmap logo = load_png("data/logo.png");
    Texture tex(logo);

    screen.render_loop([=]() mutable {
        screen.clear();
        screen.circle(x += 4, 200, 180, 0xff0000ff);
        screen.draw(tex);
        screen.text(font, "Circle", screen.width()-120, screen.height()-50);
        screen.flip();
    });
    return 0;
}
{% endhighlight %}
...and it works perfectly, both in javascript and as native code. Note my use of `atexit()` -- the render loop is actually executed after `main()` has exited and
all the variables have gone out of scope. But since the lambda has captured them all they are not destroyed until we actually exit.

More on Lambda expressions
--------------------------

If you haven't used lambda expressions in C++ before, the earlier example may have confused you. And even if you have, you may have
wondered why I used  the `[=]` clause (which means that the lambda catches everything by value), and what the `mutable` keyword does.

Well, we need to capture everything by value since if we catch by reference (using `[&]`) those references will be invalid when our
`main()` function has ended.

`mutable` just means that the lambda object members are changable. We need this to be able to change the `x` variable. Remember that a
lambda generates anonymous class, with all captured variables as members, and a `()` operator so it can be called as a function.

Lambdas are great for simple callbacks - and indeed, this is used a lot in javascript for asynchrounous programming; You start an action
and provide some code that will be executed when the action is done. And now we have all *three* things that makes callbacks really useable
in our C++ code. We have lambdas, we have reference counted *by-value* objects -- and thanks to emscripten -- we are forced to design our program in an event driven way. This means we can make sure our callback is executed in the same thread where it is defined, and thus it can reference surrounding state in a thread-safe manner.

This is a simple example on how I play audio using this techique;
{% highlight c++ %}
    ModPlugin modPlugin;
    auto player = modPlugin.fromFile("data/dna-dream.mod"));
    AudioPlayer audioPlayer([=](uint16_t *target, int len) {
        player.getSamples(target, len);
    });
{% endhighlight %}
The `ModPlugin` can create an object that generates sound from a given music file.
Our `AudioPlayer` takes a callback that should generate audio every time the audio buffer needs to be filled.
It doesn't need to be harder than this.
