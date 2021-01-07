<p align="center">
  <img width="160" height="160" src="snap/gui/kurve.png">
</p>

# 

[![Github Actions][img_gh-actions]][gh-actions]
![mit license][img_license]
[![kurve](https://snapcraft.io/kurve/badge.svg)](https://snapcraft.io/kurve)
[![Discord channel](https://img.shields.io/discord/796367578028441600?logo=discord)](https://discord.gg/cGXqBYhS)


[img_license]: https://img.shields.io/badge/License-MIT_or_Apache_2.0-blue.svg
[img_gh-actions]: https://github.com/h-valdes/kurve/workflows/Ubuntu/badge.svg

[gh-actions]: https://github.com/h-valdes/kurve/actions?query=workflow%3AUbuntu

**Kurve** is a remake of the popular game "Achtung, die Kurve!" using C++ 
and OpenGL.

<p align="center">
  <img width="460" height="300" src="docs/img/demo.gif">
</p>

Table of contents
=================

<!--ts-->
   * [Getting Started](#getting-started)
      * [Install using snap](#install-using-snap)
      * [Install (Source)](#install-from-the-source)
      * [Debug](#debug)
   * [Roadmap](#roadmap)
   * [Built With](#built-with)
   * [License and Authors](#license-and-authors)
   * [Screenshots](#screenshots)
<!--te-->

# Getting Started

## Install using snap
Kurve is available in the Snap Store under the name "kurve".

[![Get it from the Snap Store](https://snapcraft.io/static/images/badges/en/snap-store-black.svg)](https://snapcraft.io/kurve)
```
$ sudo snap install kurve
```
To get kurve as a rolling release
```
$ sudo snap install kurve --edge
```

## Install from the source
Clone this project
```
$ git clone https://github.com/h-valdes/kurve.git
```

### Install all the dependencies:
* [GLFW](https://www.glfw.org/)
* [FreeType2](https://www.freetype.org/)
* [json](https://github.com/nlohmann/json)

In Ubuntu 20.04
```
$ sudo apt install libglfw3-dev libfreetype-dev nlohmann-json3-dev
```

In archlinux or Manjaro
```
$ sudo pacman -Syu glfw-x11 freetype2 nlohmann-json 
```

### Build and install the project
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

## Debug
To play without installing the game in your computer (you still need to 
install the dependencies):
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

# Roadmap
- [x] [v0.1-alpha](https://github.com/h-valdes/kurve/releases/tag/v0.1-alpha)
   - [x] Minimal working implementation of "Achtung, die Kurve". It has all the
      features from the 1995 DOS version
   - [x] Random start position of the players
   - [x] Random length of lines (defined in a specific range)
   - [x] Menu to select the players
   - [x] Game Over screen
- [x] [v0.2-alpha](https://github.com/h-valdes/kurve/releases/tag/v0.2-alpha)
   - [x] Sort the name of the players in the Game Over screen depending on the
      number of points
   - [x] Make the terminal output less verbose when the build type is release
   - [x] Read the configuration of the players from a json file
   - [x] Generate a snap package (.snap) with snapcraft
   - [x] End the game when pressing Escape in the Menu
   - [x] Full Screen support
   - [x] Generate a debian package (.deb) with cmake
   - [x] Migrate from the self built classes Point and Vector to GLM
- [ ] v0.3-alpha
   - [x] Use a constant or max. fps. In Manjaro was unplayable, because 
      the fps were too high 
   - [x] Add a starting direction arrow in the transition zone (before starting)
   - [x] Improve the scoring system and add a minimum differences to win
   - [x] Reorganize the static files and structure of the repository
   - [ ] Add a new general Menu (to choose between different options: config,
      exit the game, etc)
   - [ ] Create an abstraction for the Scenes and a SceneManager replacing the
      changes in the class Game
   - [ ] Create an AI to play against
   - [ ] Change the style to differ with the Flash Remake of "Achtung, die Kurve"
   - [ ] Improve the font rendering and the class Font in general. Add blinking
      text throught the class, different types of font (bold, italic, etc) and
      different sizes
   - [ ] Generate an AppImage package
   - [ ] Generate a Flatpak package
   - [ ] Render textures (images) for example the logo of kurve in the main Menu


# Built With

* C++
* OpenGL (glad)
* glm
* GLFW
* FreeType2

# License and Authors
This Project started as a final project for the class Multimedia
at the TU Bergakademie Freiberg by Simon Al Nomer and Hernán Valdés.
The final version for the class corresponds the Release v0.1-alpha.

The class was taught by [Jonas Treumer](https://github.com/JayTee42) and 
[Ben Lorenz](https://github.com/bnlrnz) and inside of the project 
are still traces of the C code programmed by them as an example of the 
functionality of OpenGL and GLFW.

All future release (starting from v0.2-alpha) are developed and maintained by 
Hernán Valdés and licensed under the MIT License.


# Screenshots

![](docs/img/1.png)
![](docs/img/2.png)
![](docs/img/3.png)
