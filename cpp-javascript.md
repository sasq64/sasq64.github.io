---
layout: page
title: "Treating C++ like it was Javascript"
description: ""
category: 
tags: []
---
{% include JB/setup %}
*by Jonas Minnberg* -- <sasq64@gmail.com>

In this article I will briefly describe a technique that allows you to program C++
in a way that is more similar to dynamic or functional languages such as python or
javascript.

I assume you know C++, and have at least read up on the new C++11 features -- at least
the new *shared_ptr* as well as *lambda* functions.

It requires that you write your classes in a certain way, and makes the most sense when
you have a clear division between the *client code* that is written to accomplish
a specific task, and the *API* of generic classes and functions that the client code uses.

More specifically, by designing your API using these techniques, the client code using the API
can be written by programmers that are more familiar with higher level languages, and without
managing many of the complexities inherent to C++.

What follows are essentially a description of the *three* ingredients needed:

BY-VALUE CLASSES
----------------

Our first step is to make sure the user does not need to use *new* or *delete*, and
also not make the distinction of accessing objects *by reference* or *by value*;

This is accomplished by designing all your classes so that passing them to a function
*by value* will *internally* work as though it was passed by reference. The way you
would normally do this is letting your class act as a wrapper for an internal class,
which is referenced using a *shared_ptr*.

This means the class is small when seen as a value, and even when it is copied, the
copy will reference the same state through the *shared_ptr*.

Here is an example;
{% highlight c++ %}
struct bitmap {
    bitmap(int w, int h) :
        w(w), h(h), pixels(make_shared<vector<uint8_t>>(w*h)) {
    }

    uint8_t& operator[](const int &i) { 
        return (*pixels)[i];
    }

    const int w;
    const int h;
private:
    shared_ptr<vector<uint8_t>> pixels;
};

void fill(bitmap bm, uint32_t color) {
    for(int i=0; i<bm.w*bm.h; i++)
        bm[i] = color;
}

bitmap myBitmap;
fill(myBitmap, 0x33);

{% endhighlight %}

Notice that our bitmap will also not leak memory and basically works like objects in a garbage
collected language such as *python*.

LAMBDAS AS CLOSURES
-------------------

Our *by-value* classes means we can now *treat lambdas as closures*. By this
I mean that variables captured by a lambda function will be retained, even
if the variables captured goes out of scope.

{% highlight c++ %}
function<bitmap()> fractalGenerator(int w, int h) {
    bitmap bm(w,h);
    return [=]() mutable -> bitmap {
        for(int y=0; y<h; y++)
            for(int x=0; x<w; x++)
                bm[x+y*w] = someFunction(x,y);
        return bm;
    };
}
{% endhighlight %}

The lambda captures *by value* (the `[=]` clause) and thus creates a member variable
that (as shown earlier) references the same pixels. So `fractalGenerator()` returns
a function that can later be called to fill the bitmap.

**NOTE:** Lambdas does not capture class members by value, it will always only catch *this*.
If you want to access members inside a lambda you need to make a copy first:

{% highlight c++ %}
struct MyClass {
    bitmap bm;

    function<bitmap()> fractalGenerator(int w, int h) {
        auto bmCopy = bm; // <- Note!
        return [=]() mutable -> bitmap {
            for(int y=0; y<h; y++)
                for(int x=0; x<w; x++)
                    bmCopy[x+y*w] = someFunction(x,y);
            return bmCopy;
        }
    };
};
{% endhighlight %}


EVENT DRIVEN ARCHITECTURE
-------------------------

The final piece of the puzzle is this; we need to design our program so that it
reacts to events instead of running a main loop that does all the work.

This is what you need to do anyway if you want to compile to a javascript target (using 
*emscripten*), or if you use a third-party library that requires it, such as *GLUT*.

This means that instead of this:
{% highlight c++ %}
int main() {
    bitmap logo = load_png("data/logo.png");
    while(screen.is_open()) {
        screen.clear();
        screen.draw(logo);
        screen.flip();
    };
    return 0;
}
{% endhighlight %}
you do something like this:
{% highlight c++ %}
int main() {
    Bitmap logo = load_png("data/logo.png");
    screen.render_loop([=]() {
        screen.clear();
        screen.draw(logo);
        screen.flip();
    });
    return 0;
}
{% endhighlight %}
In this second version we create a lambda that will be executed regularly by our `screen` object.
This will happen *after* `main()` has exited, but this is not a problem since our bitmap is
captured by the lambda and will outlive the scope in which it was originally created.

While this can seem a bit restricting, it does give you one huge advantage, you
can let the user program using a *single threaded model*, which means his callbacks
will always be called in the same thread as thread as they were created.

This means you can do safely do things like this:
{% highlight c++ %}
    auto player = MusicPlayer::fromFile("data/dna-dream.mod"));
    AudioPlayer audioPlayer([=](uint16_t *target, int len) {
        player.getSamples(target, len);
    });
{% endhighlight %}

Here we create an `AudioPlayer` that takes a function to call every time it needs new samples.
Normally, such a callback needs to take place from a different thread, but since we control
the render loop we can make sure the callback is called *in between* calls to the render loop,
and so in *the same thread* as the render loop.

Together with our *by value* classes and C++11 lambdas, this allows us to use callbacks in
a very powerful way. Here is another example;

{% highlight c++ %}
    my_string title = "Initial text";
    webGetter.getData("http://localhost/getTitle", [=](string result) {
        title = result;
    });
    screen.render_loop([=]() mutable {
        screen.clear();
        screen.text(0,0, title);
        screen.flip();
    });
{% endhighlight %}

In both these examples, the lambda will be called at a later, unknown time -- but its still
safe for it to reference variables captured by the scope.

Note that in the last example, we require a string class that was designed *by value* as described above.
If we don't provide such a string class, we need to revert to using a *shared_ptr* directly.

{% highlight c++ %}
    auto title = make_shared<string>("Initial text");
    webGetter.getData(myUrl, [=](string result) {
        *title = result;
    });
    screen.render_loop([=]() mutable {
        screen.clear();
        screen.text(0, 0, *title);
        screen.flip();
    });
{% endhighlight %}
