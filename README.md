# Qt DE-Audio Player
A program for playing audio files with the function of saving playlists

<img width="900" height="655" alt="DE-audio interface" src="https://github.com/user-attachments/assets/55388a08-afef-4c2c-9a0b-1015638eaafd"/>

# Project Description
The audio player program is created in C++ using the Qt Creator 5.15.2 development environment. Playlists are saved to a JSON file.

# Launch a project
In order to run this project you need to install g++ and QtCreator 5.15.2

**Installing**

To install the g++ compiler for C++ code, run this on terminal:

	$ sudo apt install g++
You can download the QtCreator development environment [here](https://www.qt.io/download?hsCtaTracking=f24f249b-61fb-4dec-9869-50512342f8d9%7Cf3adf380-4740-4f7e-9e49-d06fa99445fa).

After installing g++ and QtCreator version 5.15.2 or any other current version of Qt5, you need to clone the repository using the following command:

    $ git clone https://github.com/Dark-Engineer/DE-Audio.git
Open QtCreator and select the DE-audio/DE_Audio.pro file as a project.
Please note that the formats supported by this audio player depend on the codec installed on your OS. 
A playlist saved as a JSON file preserves track directories. If you move or delete tracks, they will not load in the playlist.
