# Surakarta Game
A cheese game, 'surakarta', implementing requirements in https://github.com/panjd123/Surakarta

## Install

On Windows, you can download the latest release from [here](https://github.com/surakarta-game/surakarta-game/releases).

On Debian or Ubuntu, you can use the following commands to install this game:
```bash
sudo update
sudo apt install qt6-base-dev libglx-dev libgl1-mesa-dev
git clone https://github.com/surakarta-game/surakarta-game.git --recursive
cd surakarta-game
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
cmake --build . -j
sudo cmake --install .
```
and then you can use command ```surakarta-game``` to play!

On other GNU/Linux distributions or other OS such as MacOS, you need to install Qt6 and use cmake to build this project.

## Development

We recommend you to use VSCode and Qt Designer to develop this project, since clang-tidy is supported better by VSCode than by Qt Creator. 

You must clone this repository recursively, because you need to download dependencies:
```bash
git clone https://github.com/surakarta-game/surakarta-game.git --recursive
```

## Acknowledgements

Our github action workflow for building for windows is based on [this](https://github.com/eyllanesc/69108420) repository. Thanks to [eyllanesc](https://github.com/eyllanesc) for sharing it!
