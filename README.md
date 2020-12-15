# Kurve

**Kurve** is a remake of the popular game "Achtung, die Kurve!".

<p align="center">
  <img width="460" height="300" src="docs/img/demo.gif">
</p>

Table of contents
=================

<!--ts-->
   * [Getting Started](#getting-started)
      * [Install](#install)
      * [Debug](#debug)
   * [Built With](#built-with)
   * [Screenshots](#screenshots)
<!--te-->

## Getting Started

Clone this project.
```
$ git clone https://github.com/h-valdes/kurve.git
```

Install all the dependencies:
* [GLFW](https://www.glfw.org/)
* [FreeType2](https://www.freetype.org/)

### Install

Build the project
```
$ cd kurve
$ mkdir Release
$ cd Release
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ sudo make install
```

Enjoy
```
$ kurve
```

### Debug
To play without installing the game in your computer:
```
$ cd kurve
$ mkdir Debug
$ cd Debug
$ cmake -DCMAKE_BUILD_TYPE=Debug ..
$ make
```

Enjoy
```
$ ./kurve
```

## Built With

* C++
* OpenGL (glad)
* glm
* GLFW
* FreeType2

## Screenshots

![](docs/img/1.png)
![](docs/img/2.png)
![](docs/img/3.png)
