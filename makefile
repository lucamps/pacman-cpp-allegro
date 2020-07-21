all: compile execute

compile: pacman.cpp
	g++ pacman.cpp -lallegro -lallegro_image -lallegro_audio -lallegro_acodec -lallegro_font -lallegro_ttf

execute: a.out
	./a.out