---
layout: page
title: The Dark Arts
tagline: Coding for codings sake
---
{% include JB/setup %}

{% for post in site.posts %}
<h2><a href="{{ post.url }}">{{ post.title }} - {{ post.date | date_to_string }}</a></h2>
<p>{{ post.excerpt }}</p>
<hl>
{% endfor %}
