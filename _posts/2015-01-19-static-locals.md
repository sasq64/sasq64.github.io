---
layout: post
title: "Static locals"
description: ""
category: 
tags: []
---
{% include JB/setup %}
What's wrong with this code?

{% highlight c %}
class AudioPlayer {
    void update() {
    static vector<int16_t> buffer(16384);
    // ... Do something with buffer here
  };
  
  ~AudioPlayer() {
    update();
    sound_close();
  }
}

{% endhighlight %}

This is stripped down version of code that gave me random crashes for a long time until I figured out something that maybe all you C++ gurus already knows; If *AudioPlayer* is destroyed when the program exits (ie it's a global static or referenced by a smart pointer), there is no gurantee that that the local static inside the *update()* method wont be destroyed *before* AudioPlayer. So it is not safe to call a method with static variables from the destructor.
