---
layout: post
title: "In search of the Canonical Book Format"
description: ""
category: 
tags: [ebooks]
---
{% include JB/setup %}
So as I mentioned in my previous post, I have been looking at file formats for representing books, fiction in particular. E-books have been around for a while now and you can find them in all sorts of formats in all sorts of places. The problem with most of the existing formats though, are that they either keep to little of the books original format, or too much.

At the far end of the spectrum we find *PDFs.* They define exactly how the text should be rendered, down to the physical size of the pages. This makes them pretty worthless for anything except printing to paper – unless your reading device happens to be the same size as the PDF. Despite this it is a popular format for encrypted books sold or lent out online. (The recently added “reflow” functionality only helps a bit, as it often destroys the layout of the text.)

At the other end we have plain *TEXT* files – where most of the original formatting is gone. In my opinion however,  they are better than PDFs since they are not too hard to reformat into something more flexible.

I have been looking for something else though – a simple format that can easily be read and rendered to any type of screen size, without loosing too much of the structure of the original book. That’s why I was initially excited about the FB2 format, which is an XML format for representing fiction books. It is currently mostly used by the Russian E-book community, but I didn’t let that stop me, and tried to use it as the target format for my E-book formatting program.

Unfortunately, I soon ran into problems where the format didn’t let me preserve aspects of the original text that I really thought needed preserving. After examining existing books and mailing a bit with one of the inventors I think I have understood the nature of my problem with this format.

FB2 allows you to describe a book semantically - and does a pretty good job of that too. You can tag sections as POEMS, EPIGRAPHS or TITLES for instance. Then when you convert and/or transfer the book to your preferred reading device, you decide how the book should appear visually – by for instance centering all titles and displaying all poems using italics. This sort of control lets you make sure that the book will look good on your particular hardware, be it a mobile phone, a text terminal or a portable E-book reader.

This is all well and good for simple (usually old) books – but modern books contain a whole lot of formatting that can not be represented this way – and where the semantic meaning of the different parts is not clear to anyone – except maybe the author. Take *JPod* by *Douglas Copland* for instance – I dare you to make a readable version of this book in the FB2 format.

So in the end what a good format should describe (IMHO) is how the book should be represented *visually*, but *without* making assumptions about the target page format (or available fonts).  This leads me to describe a book something like this:

* A Book consists of a sequence of Sections
* A Section consists of a sequence of Paragraphs
* A Paragraph consists of a sequence of Text Fragments
* A Text Fragment consists of a sequence of words (no line breaks)
* Sections indicate where page breaking should take place. Normally a single chapter is a single section.
* Sections can be aligned vertically. Paragraphs can be aligned horizontally. Text Fragments can have font styles.
* Paragraphs and Section can also define marigns (around the pages, around the paragraphs and for the first line of a paragraph).

In addition to this, semantic meaning should be applied to the different parts, to help conversions to other formats, and for navigation and table of contents handling. The way to deal with this I think, is to introduce page and paragraph styles that both define semantic and visual properties.

Until next time…