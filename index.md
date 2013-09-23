---
layout: page
title: The Dark Arts
tagline: Coding for codings sake
---
{% include JB/setup %}

{% for post in site.posts %}
<h1>{{ post.title }} - {{ post.date | date_to_string }}</h1>
<div>{{ post.content }}</div>
{% endfor %}
